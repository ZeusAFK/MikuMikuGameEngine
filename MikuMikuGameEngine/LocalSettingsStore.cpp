#include "StdAfx.h"
#include "LocalSettingsStore.h"
#include <cctype>

#pragma warning(disable: 4996)

// TODO: change the define of your application name, and remove the 
// "undef" line below to make this file compile.
#define APPLICATION_NAME L"MikuMikuGameEngine"
//#undef APPLICATION_NAME 

#define SETTINGS_VERSION_NUMBER 0xE0F00001
#define SETTINGS_END_NUMBER 0x00009F70
#define MAX_VALUE_SIZE (128*1024)
#define MAX_VALUE_NAME_LENGTH 1024


IMPLEMENT_DYNCREATE(CLocalSettingsStore, CSettingsStore)

static int nWrites;
std::map<std::wstring, std::vector<char>> CLocalSettingsStore::keys_;
bool CLocalSettingsStore::dirty_;

static std::wstring Downcase(std::wstring str)
{
	std::wstring ret(str);
	std::transform(ret.begin(), ret.end(), ret.begin(), towlower);
	return ret;
}

CLocalSettingsStore::CLocalSettingsStore(void)
{
	//  Don't re-load keys from disk all the time!
	//  The registry reading/writing is really aggressive :-(
	if (keys_.size() == 0)
	{
		ReadFromFile();
	}
	dirty_ = false;
}

CLocalSettingsStore::~CLocalSettingsStore(void)
{
	if (dirty_)
	{
		++nWrites;
		WriteToFile();
	}
}


HRESULT CLocalSettingsStore::ReadFromFile()
{
	std::wstring name;
	std::vector<char> data;

	std::wstring p=GetSettingFilePath();
	FILE *f = _wfopen(p.c_str(), L"rb+");
	if (!f)
		goto error;
	unsigned int version = 0;
	if (1 != fread(&version, 4, 1, f))
		goto error;
	if (version != SETTINGS_VERSION_NUMBER)
		goto error;
	unsigned short keyCount = 0;
	if (1 != fread(&keyCount, 2, 1, f))
		goto error;
	for (unsigned int i = 0; i != keyCount; ++i)
	{
		unsigned short nameSize;
		if (1 != fread(&nameSize, 2, 1, f))
			goto error;
		//  limit the length of names
		if (nameSize < 1 || nameSize > MAX_VALUE_NAME_LENGTH)
			goto error;
		unsigned short usePrev;
		if (1 != fread(&usePrev, 2, 1, f))
			goto error;
		if (usePrev >= nameSize)
			goto error;
		name.resize(nameSize);
		if (nameSize != fread(&name[0] + usePrev, sizeof(wchar_t), nameSize - usePrev, f))
			goto error;
		unsigned int keySize;
		if (1 != fread(&keySize, 4, 1, f))
			goto error;
		//  limit size to something sane
		if (keySize > MAX_VALUE_SIZE)
			goto error;
		data.resize(keySize);
		if (keySize != 0 && keySize != fread(&data[0], 1, keySize, f))
			goto error;
		keys_[Downcase(name)] = data;
		//      std::wstringstream wss;
	}
	if (1 != fread(&version, 4, 1, f))
		goto error;
	if (version != SETTINGS_END_NUMBER)
		goto error;
	fclose(f);
	return S_OK;

error:
	::OutputDebugString(L"LocalSettingsStore: Corrupted settings found. Reverting to defaults.\r\n");
	if (f)
		fclose(f);
	//  use no settings if the read didn't work
	Clear();
	return E_FAIL;
}

size_t CommonPrefixLength(std::wstring const &a, std::wstring const &b)
{
	std::wstring::const_iterator ap = a.begin(), end = a.end();
	std::wstring::const_iterator bp = b.begin();
	size_t len = 0;
	while (ap != end)
	{
		if (*ap != *bp)
			return len;
		++len;
		++ap;
		++bp;
	}
	return len;
}

HRESULT CLocalSettingsStore::WriteToFile()
{
	std::map<std::wstring, std::vector<char>>::iterator ptr, end;

	std::wstring prev = L"";
	std::wstring p = GetSettingFilePath();
	std::wstring q(p);
	q += L".tmp";
	FILE *f = _wfopen(q.c_str(), L"wb+");
	if (!f)
		goto error;
	unsigned int version = SETTINGS_VERSION_NUMBER;
	if (1 != fwrite(&version, 4, 1, f))
		goto error;
	unsigned short keyCount = (unsigned int)keys_.size();
	if (1 != fwrite(&keyCount, 2, 1, f))
		goto error;
	ptr = keys_.begin(), end = keys_.end();
	for (unsigned int i = 0; i != keyCount; ++i)
	{
		if (ptr == end)
			goto error;
		unsigned short nameSize = (*ptr).first.length();
		//  limit the length of names
		if (nameSize < 1 || nameSize > MAX_VALUE_NAME_LENGTH)
			goto error;
		unsigned short usePrev = CommonPrefixLength(prev, (*ptr).first);
		if (1 != fwrite(&nameSize, 2, 1, f))
			goto error;
		if (1 != fwrite(&usePrev, 2, 1, f))
			goto error;
		if ((nameSize - usePrev) != fwrite(&(*ptr).first[0] + usePrev, sizeof(wchar_t), nameSize - usePrev, f))
			goto error;
		unsigned int keySize = (*ptr).second.size();
		//  limit size to something sane
		if (keySize > 128*1024)
			goto error;
		if (1 != fwrite(&keySize, 4, 1, f))
			goto error;
		if (keySize != 0 && keySize != fwrite(&(*ptr).second[0], 1, keySize, f))
			goto error;
		++ptr;
	}
	version = SETTINGS_END_NUMBER;
	if (1 != fwrite(&version, 4, 1, f))
		goto error;
	fclose(f);
	f = NULL;
	_wremove(p.c_str());
	/* A crash right here would cause the settings to be lost. Oh, well. */
	if (_wrename(q.c_str(), p.c_str()) != 0)
		goto error;
	dirty_ = false;
	return S_OK;
error:
	::OutputDebugString(L"LocalSettingsStore: Error writing settings. Old settings are preserved.\r\n");
	if (f)
	{
		fclose(f);
		_wremove(q.c_str());
	}
	return E_FAIL;
}

void CLocalSettingsStore::Clear()
{
	std::map<std::wstring, std::vector<char>>().swap(keys_);
	dirty_ = true;
}


void CLocalSettingsStore::SetCurKey(wchar_t const *key)
{
	curKey_ = Downcase(key);
	if (!curKey_.empty() && *curKey_.rbegin() != L'\\')
		curKey_.append(L"\\");
}

BOOL CLocalSettingsStore::CreateKey(LPCTSTR lpszPath)
{
	std::wstring key(lpszPath);
	if (key.length() == 0)
		return FALSE;
	SetCurKey(lpszPath);
	keys_[curKey_] = std::vector<char>();
	dirty_ = true;
	return TRUE;
}

BOOL CLocalSettingsStore::Open(LPCTSTR lpszPath)
{
	//  the key is represented by a 0-length value of the key name with a terminating slash
	std::wstring key(lpszPath);
	if (key.length() == 0)
		return FALSE;
	if (*key.rbegin() != '\\')
		key.append(L"\\");
	std::map<std::wstring, std::vector<char>>::iterator pos = keys_.find(Downcase(key));
	if (pos == keys_.end())
		return FALSE;
	SetCurKey(key.c_str());
	return TRUE;
}

void CLocalSettingsStore::Close()
{
	SetCurKey(L"");
}


BOOL CLocalSettingsStore::DeleteValue(LPCTSTR lpszValue)
{
	std::wstring name(lpszValue);
	if (name.length() == 0 || *name.rbegin() == '\\')
		return FALSE;
	std::wstring key(curKey_);
	key += name;
	std::map<std::wstring, std::vector<char>>::iterator ptr = keys_.find(Downcase(key));
	if (ptr == keys_.end())
		return FALSE;
	keys_.erase(ptr);
	dirty_ = true;
	return TRUE;
}

BOOL CLocalSettingsStore::DeleteKey(LPCTSTR lpszPath, BOOL bAdmin)
{
	if (curKey_ == lpszPath)
		return FALSE;
	std::wstring key(lpszPath);
	if (key.length() == 0)
		return FALSE;
	if (*key.rbegin() != '\\')
		key.append(L"\\");
	std::map<std::wstring, std::vector<char>>::iterator ptr = keys_.find(Downcase(key));
	if (ptr == keys_.end())
		return FALSE;
	std::map<std::wstring, std::vector<char>>::iterator end = keys_.end();
	//  erase everything that starts with whatever this key path is
	while (ptr != end)
	{
		if ((*ptr).first.length() < key.length())
			break;
		std::wstring name((*ptr).first);
		name.resize(key.length());
		if (name != key)
			break;
		keys_.erase(ptr++);
	}
	dirty_ = true;
	return TRUE;
}


BOOL CLocalSettingsStore::Write(LPCTSTR lpszValueName, DWORD dwVal)
{
	std::wstring name(lpszValueName);
	if (name.length() == 0 || *name.rbegin() == '\\' || name.length() > MAX_VALUE_NAME_LENGTH)
		return FALSE;
	std::wstring key(curKey_);
	key += name;
	std::vector<char> v;
	v.resize(sizeof(dwVal));
	memcpy(&v[0], &dwVal, v.size());
	keys_[Downcase(key)] = v;
	dirty_ = true;
	return TRUE;
}

BOOL CLocalSettingsStore::Write(LPCTSTR lpszValueName, LPCTSTR lpszVal)
{
	if (lpszVal == NULL)
		lpszVal = L"";
	std::wstring name(lpszValueName);
	if (name.length() == 0 || *name.rbegin() == '\\' || name.length() > MAX_VALUE_NAME_LENGTH)
		return FALSE;
	std::wstring key(curKey_);
	key += name;
	std::vector<char> v;
	v.resize((wcslen(lpszVal) + 1) * sizeof(wchar_t));
	if (v.size() > MAX_VALUE_SIZE)
		return FALSE;
	memcpy(&v[0], lpszVal, v.size());
	keys_[Downcase(key)] = v;
	dirty_ = true;
	return TRUE;
}

BOOL CLocalSettingsStore::Write(LPCTSTR lpszValueName, LPBYTE pData, UINT nBytes)
{
	std::wstring name(lpszValueName);
	if (name.length() == 0 || *name.rbegin() == '\\' || name.length() > MAX_VALUE_NAME_LENGTH)
		return FALSE;
	std::wstring key(curKey_);
	key += name;
	std::vector<char> v;
	v.resize(nBytes);
	if (v.size() > MAX_VALUE_SIZE)
		return FALSE;
	if (v.size() > 0)
		memcpy(&v[0], pData, v.size());
	keys_[Downcase(key)] = v;
	dirty_ = true;
	return TRUE;
}


BOOL CLocalSettingsStore::Read(LPCTSTR lpszValueName, DWORD& dwValue)
{
	std::wstring name(lpszValueName);
	if (name.length() == 0 || *name.rbegin() == '\\')
		return FALSE;
	std::wstring key(curKey_);
	key += name;
	std::map<std::wstring, std::vector<char>>::iterator ptr = keys_.find(Downcase(key));
	if (ptr == keys_.end())
		return FALSE;
	if ((*ptr).second.size() != sizeof(dwValue))
		return FALSE;
	memcpy(&dwValue, &(*ptr).second[0], (*ptr).second.size());
	return TRUE;
}

BOOL CLocalSettingsStore::Read(LPCTSTR lpszValueName, CString& strValue)
{
	std::wstring name(lpszValueName);
	if (name.length() == 0 || *name.rbegin() == '\\')
		return FALSE;
	std::wstring key(curKey_);
	key += name;
	std::map<std::wstring, std::vector<char>>::iterator ptr = keys_.find(Downcase(key));
	if (ptr == keys_.end())
		return FALSE;
	if ((*ptr).second.size() == 0)
		return FALSE;
	if ((*ptr).second.size() % sizeof(wchar_t) != 0)
		return FALSE;
	wchar_t const *str = (wchar_t const *)&(static_cast<char &>((*ptr).second[0]));
	size_t len = (*ptr).second.size() / sizeof(wchar_t) - 1;
	//  must be zero terminated
	if (str[len] != 0)
		return FALSE;
	strValue.SetString(str);
	return TRUE;
}


BOOL CLocalSettingsStore::Read(LPCTSTR lpszValueName, BYTE** ppData, UINT* pcbData)
{
	*ppData = 0;
	std::wstring name(lpszValueName);
	if (name.length() == 0 || *name.rbegin() == '\\')
		return FALSE;
	std::wstring key(curKey_);
	key += name;
	std::map<std::wstring, std::vector<char>>::iterator ptr = keys_.find(Downcase(key));
	if (ptr == keys_.end())
		return FALSE;
	*pcbData = (*ptr).second.size();
	//  Curiously, the BYTE* returned should be allocated with new[].
	//  This is not documented, but is how the library calls it.
	*ppData = new BYTE[*pcbData];
	memcpy(*ppData, &(*ptr).second[0], *pcbData);
	return TRUE;
}

std::wstring CLocalSettingsStore::GetSettingFilePath()
{
	wchar_t path[MAX_PATH];   //パス取得用
	if (::GetModuleFileNameW(NULL, path, MAX_PATH) )    //実行ファイルのフルパスを取得
	{   
		PathRemoveFileSpecW( path );
		PathAppendW( path,L"Settings.bin" );

		return std::wstring(path);
	}

	return std::wstring(L"Settings.bin");
}