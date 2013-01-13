#pragma once

// マルチバイト文字列をワイド文字列に変換(文字コード指定)
bool to_widechar( const std::string& srcStr,std::wstring& dstStr,UINT CodePage );

// ワイド文字列をマルチバイト文字列に変換(文字コード指定)
bool to_multibyte( const std::wstring& srcStr,std::string& dstStr,UINT CodePage );

// SJISからUNICODEに変換
bool sjis_to_unicode( const std::string& srcStr,std::wstring& dstStr );

// UNICODEからSJISに変換
bool unicode_to_sjis( const std::wstring& srcStr,std::string& dstStr );

// UNICODEからUTF8に変換
bool unicode_to_utf8( const std::wstring& srcStr,std::string& dstStr );

// UTF8からUNICODEに変換
bool utf8_to_unicode( const std::string& srcStr,std::wstring& dstStr );

// SJISのアルファベットを全て大文字に変換
bool sjis_to_upper( std::string& str );

// SJISのアルファベットを全て小文字に変換
bool sjis_to_lower( std::string& str );

// UNICODEのアルファベットを全て大文字に変換
bool unicode_to_upper( std::wstring& str );

// UNICODEのアルファベットを全て小文字に変換
bool unicode_to_lower( std::wstring& str );


// vsprintfのSJIS(std::string)版
int sjis_format_va( std::string& str,LPCSTR format,va_list va );

// sprintfのSJIS(std::string)版
int sjis_format( std::string& str,LPCSTR format,... );

// vsprintfのUNICODE(std::wstring)版
int unicode_format_va( std::wstring& str,LPCWSTR format,va_list va );

// sprintfのUNICODE(std::wstring)版
int unicode_format( std::wstring& str,LPCWSTR format,... );

// std::stringのTCHAR版
typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstring;

// バイナリには対応してないレベルで使えないので注意……。
typedef std::basic_ios<TCHAR, std::char_traits<TCHAR> > tios;
typedef std::basic_streambuf<TCHAR, std::char_traits<TCHAR> >	tstreambuf;
typedef std::basic_istream<TCHAR, std::char_traits<TCHAR> > tistream;
typedef std::basic_ostream<TCHAR, std::char_traits<TCHAR> > tostream;
typedef std::basic_iostream<TCHAR, std::char_traits<TCHAR> > tiostream;
typedef std::basic_stringbuf<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstringbuf;
typedef std::basic_istringstream<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tistringstream;
typedef std::basic_ostringstream<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tostringstream;
typedef std::basic_stringstream<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstringstream;
typedef std::basic_filebuf<TCHAR, std::char_traits<TCHAR> > tfilebuf;
typedef std::basic_ifstream<TCHAR, std::char_traits<TCHAR> > tifstream;
typedef std::basic_ofstream<TCHAR, std::char_traits<TCHAR> > tofstream;
typedef std::basic_fstream<TCHAR, std::char_traits<TCHAR> > tfstream;

typedef stx::basic_symbol<tstring> tstring_symbol;
//typedef tstring tstring_symbol;

// tstringからUNICODEに変換
std::wstring to_wstring( const tstring& str );

// tstringからSJISに変換
std::string to_string( const tstring& str );

// SJISからtstringに変換
tstring to_tstring( const std::string& str );

// UNICODEからtstringに変換
tstring to_tstring( const std::wstring& str );

// tstringのアルファベットを全て大文字に変換
bool tstring_to_upper( tstring& str );

// tstringのアルファベットを全て小文字に変換
bool tstring_to_lower( tstring& str );

// vsprintfのtstring版
int tstring_format_va( tstring& str,LPCTSTR format,va_list va );

// sprintfのtstring版
int tstring_format( tstring& str,LPCTSTR format,... );