#include "stdafx.h"

#include "../core/util/util.h"

#include "squirrel/squirrel.h"
#include "squirrel/sqstdio.h"
#include "squirrel/sqstdaux.h"

void PrintFunc( HSQUIRRELVM vm,const SQChar* format,... )
{
	va_list va;
	va_start( va,format );

	tstring tszDebug;
	tstring_format_va( tszDebug,format,va );

	va_end( va );

	tszDebug+=_T("\n");

	OutputDebugString( tszDebug.c_str() );
}

void TestScript()
{
	tstring fileName = _T("project\\assets\\Script\\test.nut");

	FILE* fp=NULL;
	if( _tfopen_s(&fp,fileName.c_str(),_T("r"))!=0 )
	{
		OutputDebugStringFormat( _T("�X�N���v�g�t�@�C��(%s)���J���܂���ł���\n"),fileName.c_str() );
		return;
	}

	fpos_t fsize = 0;
	fseek(fp,0,SEEK_END);
	fgetpos(fp,&fsize);

	fseek(fp,0,SEEK_SET); 

	size_t fileSize=(size_t)fsize;

	char* fileBuffer = new char[fileSize+1];
	ZeroMemory( fileBuffer,sizeof(char)*(fileSize+1) );
	fread( fileBuffer,1,sizeof(char)*fileSize,fp );

	fclose( fp );

	tstring script = to_tstring( fileBuffer );

	delete[] fileBuffer;

	HSQUIRRELVM sqvm = sq_open(1024);

	sqstd_seterrorhandlers( sqvm );

	sq_setprintfunc( sqvm,PrintFunc,PrintFunc );

	SQRESULT result = sq_compilebuffer( sqvm,script.c_str(),script.length(),fileName.c_str(),SQTrue );
	if( SQ_SUCCEEDED(result) )
	{
		int top = sq_gettop(sqvm);

		sq_pushroottable(sqvm);

		result = sq_call( sqvm,1,SQFalse,SQTrue );
		if( SQ_FAILED(result) )
		{
			OutputDebugStringFormat( _T("�X�N���v�g�̎��s�Ɏ��s���܂���(%x)\n"),result );
		}

		sq_settop( sqvm,top );
	}
	else
	{
		OutputDebugStringFormat( _T("�X�N���v�g�̃R���p�C���Ɏ��s���܂���(%x)\n"),result );
	}

	sq_close( sqvm );
}