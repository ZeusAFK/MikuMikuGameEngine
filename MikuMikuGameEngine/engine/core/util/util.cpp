#include "stdafx.h"

#include "util.h"

#include <Rpc.h>

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
	va_list va;
	va_start( va,format );

	tstring tszDebug;
	tstring_format_va( tszDebug,format,va );

	va_end( va );

	OutputDebugString( tszDebug.c_str() );
}

void CreateUUIDSymbol( tstring_symbol& symbol )
{
	UUID uuid;
    ::UuidCreate(&uuid);

    // convert UUID to LPWSTR
    WCHAR* wszUuid = NULL;
    ::UuidToStringW(&uuid, (RPC_WSTR*)&wszUuid);
    
	//TODO: do something with wszUuid
	symbol = to_tstring( wszUuid );

	// free up the allocated string
    ::RpcStringFree((RPC_WSTR*)&wszUuid);
}
