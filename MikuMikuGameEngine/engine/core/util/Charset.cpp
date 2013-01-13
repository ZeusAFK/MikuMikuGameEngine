#include "stdafx.h"
#include "Charset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// マルチバイト文字列をワイド文字列に変換(文字コード指定)
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

// ワイド文字列をマルチバイト文字列に変換(文字コード指定)
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

// SJISからUNICODEに変換
bool sjis_to_unicode( const std::string& srcStr,std::wstring& dstStr )
{
	return to_widechar( srcStr,dstStr,CP_THREAD_ACP );
}

// UNICODEからSJISに変換
bool unicode_to_sjis( const std::wstring& srcStr,std::string& dstStr )
{
	return to_multibyte( srcStr,dstStr,CP_THREAD_ACP );
}

// UNICODEからUTF8に変換
bool unicode_to_utf8( const std::wstring& srcStr,std::string& dstStr )
{
	return to_multibyte( srcStr,dstStr,CP_UTF8 );
}

// UTF8からUNICODEに変換
bool utf8_to_unicode( const std::string& srcStr,std::wstring& dstStr )
{
	return to_widechar( srcStr,dstStr,CP_UTF8 );
}

// SJISのアルファベットを全て大文字に変換
bool sjis_to_upper( std::string& str )
{
	std::transform( str.begin(),str.end(),str.begin(),toupper );
	return true;
}

// SJISのアルファベットを全て小文字に変換
bool sjis_to_lower( std::string& str )
{
	std::transform( str.begin(),str.end(),str.begin(),tolower );
	return true;
}

// UNICODEのアルファベットを全て大文字に変換
bool unicode_to_upper( std::wstring& str )
{
	std::transform( str.begin(),str.end(),str.begin(),towupper );
	return true;
}

// UNICODEのアルファベットを全て小文字に変換
bool unicode_to_lower( std::wstring& str )
{
	std::transform( str.begin(),str.end(),str.begin(),towlower );
	return true;
}


// vsprintfのSJIS(std::string)版
int sjis_format_va( std::string& str,LPCSTR format,va_list va )
{
	int len=_vscprintf( format,va );
	str.resize( len );
	_vsnprintf_s( const_cast<char*>(str.c_str()),_TRUNCATE,str.size(),format,va );

	return len;
}

// sprintfのSJIS(std::string)版
int sjis_format( std::string& str,LPCSTR format,... )
{
	va_list va;
	va_start( va,format );

	int len = sjis_format_va( str,format,va );

	va_end( va );

	return len;
}

// vsprintfのUNICODE(std::wstring)版
int unicode_format_va( std::wstring& str,LPCWSTR format,va_list va )
{
	int len=_vscwprintf( format,va );
	str.resize( len );
	_vsnwprintf_s( const_cast<wchar_t*>(str.c_str()),_TRUNCATE,str.size(),format,va );

	return len;
}

// sprintfのUNICODE(std::wstring)版
int unicode_format( std::wstring& str,LPCWSTR format,... )
{
	va_list va;
	va_start( va,format );

	int len = unicode_format_va( str,format,va );

	va_end( va );

	return len;
}

// tstringからUNICODEに変換
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

// tstringからSJISに変換
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

// SJISからtstringに変換
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

// UNICODEからtstringに変換
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

// tstringのアルファベットを全て大文字に変換
bool tstring_to_upper( tstring& str )
{
#ifdef	UNICODE
	return unicode_to_upper( str );
#else
	return sjis_to_upper( str );
#endif
}

// tstringのアルファベットを全て小文字に変換
bool tstring_to_lower( tstring& str )
{
#ifdef	UNICODE
	return unicode_to_lower( str );
#else
	return sjis_to_lower( str );
#endif
}

// vsprintfのtstring版
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

// sprintfのtstring版
int tstring_format( tstring& str,LPCTSTR format,... )
{
	va_list va;
	va_start( va,format );

	int len = tstring_format_va( str,format,va );
			
	va_end( va );

	return len;
}