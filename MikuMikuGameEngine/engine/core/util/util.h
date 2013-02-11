#pragma once

#include "Charset.h"

#define KILOBYTE (1024)
#define MEGABYTE (1024*KILOBYTE)
#define GIGABYTE (1024*MEGABYTE)

unsigned __int64 GetDiskFree();

void GetLastErrorMessage( DWORD dwMessageId,tstring& errorMessage );
void OutputDebugStringFormat( LPCTSTR format,... );

void CreateUUIDSymbol( tstring_symbol& symbol );