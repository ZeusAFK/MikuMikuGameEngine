#include "stdafx.h"

#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned __int64 GetDiskFree()
{
	int const drive = _getdrive();
	struct _diskfree_t diskfree;

	_getdiskfree(drive,&diskfree);

	return (unsigned __int64)diskfree.avail_clusters * (unsigned __int64)diskfree.sectors_per_cluster * (unsigned __int64)diskfree.bytes_per_sector;
}

void GetLastErrorMessage( DWORD dwMessageId,tstring& errorMessage )
{
	LPVOID lpMsgBuf;

	::FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwMessageId,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Šù’è‚ÌŒ¾Œê
		(LPTSTR) &lpMsgBuf,
		0,
		NULL
	);

	errorMessage = (LPCTSTR)lpMsgBuf;

	::LocalFree(lpMsgBuf);
}

void OutputDebugStringFormat( LPCTSTR format,... )
{
	LPTSTR tszDebug;

	va_list va;
	va_start( va,format );

	int len=_vsctprintf( format,va );
	tszDebug = new TCHAR[len+1];
	_vsntprintf_s( tszDebug,_TRUNCATE,len,format,va );
			
	va_end( va );

	OutputDebugString( tszDebug );

	delete[] tszDebug;
}