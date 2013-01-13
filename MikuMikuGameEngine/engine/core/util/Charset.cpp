#include "stdafx.h"
#include "Charset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// �}���`�o�C�g����������C�h������ɕϊ�(�����R�[�h�w��)
bool to_widechar( const std::string& srcStr,std::wstring& dstStr,UINT CodePage )
{
	int nLen = ::MultiByteToWideChar( CodePage,0,srcStr.c_str(),-1,NULL,0 );
	if( nLen==0 )
	{
		return false;
	}

	LPWSTR buffer = new WCHAR[nLen];

	int nRetLen = ::MultiByteToWideChar( CodePage,0,srcStr.c_str(),srcStr.length()+1,buffer,nLen );
	if( nRetLen != nLen )
	{
		delete[] buffer;
		return false;
	}
	dstStr = buffer;

	delete[] buffer;
	return true;
}

// ���C�h��������}���`�o�C�g������ɕϊ�(�����R�[�h�w��)
bool to_multibyte( const std::wstring& srcStr,std::string& dstStr,UINT CodePage )
{
	int nLen = ::WideCharToMultiByte( CodePage,0,srcStr.c_str(),-1,NULL,0,NULL,NULL );
	if( nLen==0 )
	{
		return false;
	}

	LPSTR buffer = new char[nLen];

	dstStr.resize( nLen );

	int nRetLen = ::WideCharToMultiByte( CodePage,0,srcStr.c_str(),srcStr.length()+1,buffer,nLen,NULL,NULL );
	if( nRetLen != nLen )
	{
		delete[] buffer;
		return false;
	}

	dstStr = buffer;

	delete[] buffer;
	return true;
}

// SJIS����UNICODE�ɕϊ�
bool sjis_to_unicode( const std::string& srcStr,std::wstring& dstStr )
{
	return to_widechar( srcStr,dstStr,CP_THREAD_ACP );
}

// UNICODE����SJIS�ɕϊ�
bool unicode_to_sjis( const std::wstring& srcStr,std::string& dstStr )
{
	return to_multibyte( srcStr,dstStr,CP_THREAD_ACP );
}

// UNICODE����UTF8�ɕϊ�
bool unicode_to_utf8( const std::wstring& srcStr,std::string& dstStr )
{
	return to_multibyte( srcStr,dstStr,CP_UTF8 );
}

// UTF8����UNICODE�ɕϊ�
bool utf8_to_unicode( const std::string& srcStr,std::wstring& dstStr )
{
	return to_widechar( srcStr,dstStr,CP_UTF8 );
}

// SJIS�̃A���t�@�x�b�g��S�đ啶���ɕϊ�
bool sjis_to_upper( std::string& str )
{
	std::transform( str.begin(),str.end(),str.begin(),toupper );
	return true;
}

// SJIS�̃A���t�@�x�b�g��S�ď������ɕϊ�
bool sjis_to_lower( std::string& str )
{
	std::transform( str.begin(),str.end(),str.begin(),tolower );
	return true;
}

// UNICODE�̃A���t�@�x�b�g��S�đ啶���ɕϊ�
bool unicode_to_upper( std::wstring& str )
{
	std::transform( str.begin(),str.end(),str.begin(),towupper );
	return true;
}

// UNICODE�̃A���t�@�x�b�g��S�ď������ɕϊ�
bool unicode_to_lower( std::wstring& str )
{
	std::transform( str.begin(),str.end(),str.begin(),towlower );
	return true;
}


// vsprintf��SJIS(std::string)��
int sjis_format_va( std::string& str,LPCSTR format,va_list va )
{
	int len=_vscprintf( format,va );
	str.resize( len );
	_vsnprintf_s( const_cast<char*>(str.c_str()),_TRUNCATE,str.size(),format,va );

	return len;
}

// sprintf��SJIS(std::string)��
int sjis_format( std::string& str,LPCSTR format,... )
{
	va_list va;
	va_start( va,format );

	int len = sjis_format_va( str,format,va );

	va_end( va );

	return len;
}

// vsprintf��UNICODE(std::wstring)��
int unicode_format_va( std::wstring& str,LPCWSTR format,va_list va )
{
	int len=_vscwprintf( format,va );
	str.resize( len );
	_vsnwprintf_s( const_cast<wchar_t*>(str.c_str()),_TRUNCATE,str.size(),format,va );

	return len;
}

// sprintf��UNICODE(std::wstring)��
int unicode_format( std::wstring& str,LPCWSTR format,... )
{
	va_list va;
	va_start( va,format );

	int len = unicode_format_va( str,format,va );

	va_end( va );

	return len;
}

// tstring����UNICODE�ɕϊ�
std::wstring to_wstring( const tstring& str )
{
	std::wstring dst;
#ifdef  UNICODE
	dst = str;
#else
	sjis_to_unicode( str,dst );
#endif
	return dst;
}

// tstring����SJIS�ɕϊ�
std::string to_string( const tstring& str )
{
	std::string dst;
#ifdef	UNICODE
	unicode_to_sjis( str,dst );
#else
	dst = str;
#endif
	return dst;
}

// SJIS����tstring�ɕϊ�
tstring to_tstring( const std::string& str )
{
	tstring dst;
#ifdef	UNICODE
	sjis_to_unicode( str,dst );
#else
	dst = str;
#endif
	return dst;
}

// UNICODE����tstring�ɕϊ�
tstring to_tstring( const std::wstring& str )
{
	tstring dst;
#ifdef	UNICODE
	dst = str;
#else
	unicode_to_sjis( str,dst );
#endif
	return dst;
}

// tstring�̃A���t�@�x�b�g��S�đ啶���ɕϊ�
bool tstring_to_upper( tstring& str )
{
#ifdef	UNICODE
	return unicode_to_upper( str );
#else
	return sjis_to_upper( str );
#endif
}

// tstring�̃A���t�@�x�b�g��S�ď������ɕϊ�
bool tstring_to_lower( tstring& str )
{
#ifdef	UNICODE
	return unicode_to_lower( str );
#else
	return sjis_to_lower( str );
#endif
}

// vsprintf��tstring��
int tstring_format_va( tstring& str,LPCTSTR format,va_list va )
{
	int len = 0;

#ifdef	UNICODE
	len = unicode_format_va( str,format,va );
#else
	len = sjis_format_va( str,format,va );
#endif

	return len;
}

// sprintf��tstring��
int tstring_format( tstring& str,LPCTSTR format,... )
{
	va_list va;
	va_start( va,format );

	int len = tstring_format_va( str,format,va );
			
	va_end( va );

	return len;
}