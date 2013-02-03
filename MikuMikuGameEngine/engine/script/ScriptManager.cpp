#include "stdafx.h"

#include "ScriptManager.h"

#include "squirrel/sqstdblob.h"
#include "squirrel/sqstdmath.h"
#include "squirrel/sqstdstring.h"

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
	Clear();
}

void ScriptManager::Clear()
{
	if( m_vm )
	{
		_map_ClassObject::iterator it = m_classObjects.begin();
		while( it!=m_classObjects.end() )
		{
			sq_release( m_vm,&it->second );
			++it;
		}
		m_classObjects.clear();

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
	Clear();

	m_vm = sq_open(1024);

	sqstd_seterrorhandlers( m_vm );

	sq_setprintfunc( m_vm,ScriptManager::PrintFunc,ScriptManager::PrintFunc );

	// ゲームエンジンから提供するクラスなどの設定
	{
		sq_pushroottable(m_vm);

		// squirrel標準ライブラリ
		sqstd_register_iolib(m_vm);
		sqstd_register_bloblib(m_vm);
		sqstd_register_mathlib(m_vm);
		sqstd_register_stringlib(m_vm);

		// TODO:オリジナルライブラリの設定

		sq_pop(m_vm,1);
	}

	// アセットに登録してある全てのスクリプトをビルドする
	BuildScript( assetRoot );

	// 一度実行してクラスオブジェクトを定義する
	{
		int top = sq_gettop(m_vm);

		sq_pushroottable(m_vm);

		SQRESULT result = sq_call( m_vm,1,SQFalse,SQTrue );
		if( SQ_FAILED(result) )
		{
			OutputDebugStringFormat( _T("スクリプトの実行に失敗しました(%x)\n"),result );
		}

		sq_settop( m_vm,top );
	}

	// typeが"Behavior"のクラスを列挙
	{
		int top = sq_gettop(m_vm);

		sq_pushroottable(m_vm);

		sq_pushnull(m_vm);

		while( SQ_SUCCEEDED( sq_next(m_vm,-2) ) )
		{
			HSQOBJECT obj;

			if( SQ_SUCCEEDED( sq_getstackobj(m_vm, -1, &obj) ) && obj._type == OT_CLASS )
			{
				tstring className;

				const SQChar* key = NULL;
				if( SQ_SUCCEEDED( sq_getstring( m_vm,-2,&key) ) )
				{
					className = key;
				}

				tstring typeValue;

				int top = sq_gettop(m_vm);

				sq_pushobject(m_vm, obj);

				sq_pushnull(m_vm);
				
				if( SQ_SUCCEEDED( sq_getattributes(m_vm,-2 ) ) )
				{
					sq_pushstring( m_vm,_SC("type"),-1 );

					if( SQ_SUCCEEDED( sq_get(m_vm,-2 ) ) )
					{
						const SQChar* value = NULL;

						if( SQ_SUCCEEDED( sq_getstring( m_vm,-1,&value) ) )
						{
							typeValue = value;
						}
					}
				}

				sq_settop( m_vm,top );

				if( typeValue==_T("Behavior") )
				{
					sq_addref( m_vm,&obj );

					// TODO:必要なメンバーを追加する

					m_classObjects.insert( _map_ClassObject::value_type( className,obj ) );
				}
			}

			sq_pop( m_vm,2 );
		}

		sq_pop( m_vm,1 );

		sq_settop( m_vm,top );
	}
}