#include "stdafx.h"

#include "ScriptManager.h"

void ScriptManager::PrintFunc( HSQUIRRELVM vm,const SQChar* format,... )
{
	va_list va;
	va_start( va,format );

	tstring tszDebug;
	tstring_format_va( tszDebug,format,va );

	va_end( va );

	tszDebug+=_T("\n");

	OutputDebugString( tszDebug.c_str() );
}

ScriptManager::ScriptManager()
	: m_vm(NULL)
{
}

ScriptManager::~ScriptManager()
{
	if( m_vm )
	{
		sq_close( m_vm );
		m_vm = NULL;
	}
}

void ScriptManager::BuildScript( const tstring& filePath )
{
	FILE* fp=NULL;
	if( _tfopen_s(&fp,filePath.c_str(),_T("r"))!=0 )
	{
		OutputDebugStringFormat( _T("スクリプトファイル(%s)が開けませんでした\n"),filePath.c_str() );
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

	sq_compilebuffer( m_vm,script.c_str(),script.length(),filePath.c_str(),SQTrue );
}

void ScriptManager::BuildScript( AssetNode* asset )
{
	AssetNode::Type type = asset->GetType();

	switch( type )
	{
	case AssetNode::NutFile:
		{
			BuildScript( asset->GetPath() );
		}
		break;
	case AssetNode::Directory:
		{
			AssetNode* child = asset->GetChild();
			while( child )
			{
				BuildScript( child );
				child = child->GetSiblingNext();
			}
		}
		break;
	}
}

void ScriptManager::Build( AssetNode* assetRoot )
{
	if( m_vm )
	{
		sq_close( m_vm );
		m_vm = NULL;
	}

	m_vm = sq_open(1024);

	sqstd_seterrorhandlers( m_vm );

	sq_setprintfunc( m_vm,ScriptManager::PrintFunc,ScriptManager::PrintFunc );

	// TODO:ゲームエンジンから提供するクラスなどの設定

	// アセットに登録してある全てのスクリプトをビルドする
	BuildScript( assetRoot );
}