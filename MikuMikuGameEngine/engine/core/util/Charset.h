#pragma once

// �}���`�o�C�g����������C�h������ɕϊ�(�����R�[�h�w��)
bool to_widechar( const std::string& srcStr,std::wstring& dstStr,UINT CodePage );

// ���C�h��������}���`�o�C�g������ɕϊ�(�����R�[�h�w��)
bool to_multibyte( const std::wstring& srcStr,std::string& dstStr,UINT CodePage );

// SJIS����UNICODE�ɕϊ�
bool sjis_to_unicode( const std::string& srcStr,std::wstring& dstStr );

// UNICODE����SJIS�ɕϊ�
bool unicode_to_sjis( const std::wstring& srcStr,std::string& dstStr );

// UNICODE����UTF8�ɕϊ�
bool unicode_to_utf8( const std::wstring& srcStr,std::string& dstStr );

// UTF8����UNICODE�ɕϊ�
bool utf8_to_unicode( const std::string& srcStr,std::wstring& dstStr );

// SJIS�̃A���t�@�x�b�g��S�đ啶���ɕϊ�
bool sjis_to_upper( std::string& str );

// SJIS�̃A���t�@�x�b�g��S�ď������ɕϊ�
bool sjis_to_lower( std::string& str );

// UNICODE�̃A���t�@�x�b�g��S�đ啶���ɕϊ�
bool unicode_to_upper( std::wstring& str );

// UNICODE�̃A���t�@�x�b�g��S�ď������ɕϊ�
bool unicode_to_lower( std::wstring& str );


// vsprintf��SJIS(std::string)��
int sjis_format_va( std::string& str,LPCSTR format,va_list va );

// sprintf��SJIS(std::string)��
int sjis_format( std::string& str,LPCSTR format,... );

// vsprintf��UNICODE(std::wstring)��
int unicode_format_va( std::wstring& str,LPCWSTR format,va_list va );

// sprintf��UNICODE(std::wstring)��
int unicode_format( std::wstring& str,LPCWSTR format,... );

// std::string��TCHAR��
typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > tstring;

// �o�C�i���ɂ͑Ή����ĂȂ����x���Ŏg���Ȃ��̂Œ��Ӂc�c�B
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

// tstring����UNICODE�ɕϊ�
std::wstring to_wstring( const tstring& str );

// tstring����SJIS�ɕϊ�
std::string to_string( const tstring& str );

// SJIS����tstring�ɕϊ�
tstring to_tstring( const std::string& str );

// UNICODE����tstring�ɕϊ�
tstring to_tstring( const std::wstring& str );

// tstring�̃A���t�@�x�b�g��S�đ啶���ɕϊ�
bool tstring_to_upper( tstring& str );

// tstring�̃A���t�@�x�b�g��S�ď������ɕϊ�
bool tstring_to_lower( tstring& str );

// vsprintf��tstring��
int tstring_format_va( tstring& str,LPCTSTR format,va_list va );

// sprintf��tstring��
int tstring_format( tstring& str,LPCTSTR format,... );