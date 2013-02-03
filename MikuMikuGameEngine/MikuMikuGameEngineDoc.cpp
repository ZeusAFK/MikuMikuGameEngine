
// MikuMikuGameEngineDoc.cpp : CMikuMikuGameEngineDoc クラスの実装
//

#include "stdafx.h"
#include "MikuMikuGameEngine.h"

#include "MikuMikuGameEngineDoc.h"

#include "MainFrm.h"

#include "engine/core/graphics/XFileLoader.h"
#include "engine/core/graphics/PMDFileLoader.h"
#include "engine/core/graphics/VMDFileLoader.h"

#include "engine/script/ScriptManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMikuMikuGameEngineDoc

IMPLEMENT_DYNCREATE(CMikuMikuGameEngineDoc, CDocument)

BEGIN_MESSAGE_MAP(CMikuMikuGameEngineDoc, CDocument)
END_MESSAGE_MAP()


// CMikuMikuGameEngineDoc コンストラクション/デストラクション

CMikuMikuGameEngineDoc::CMikuMikuGameEngineDoc()
	: m_pSelectObject(NULL)
{
	// TODO: この位置に 1 度だけ呼ばれる構築用のコードを追加してください。

	m_pRoot = new GameObject();
	m_pAssetRoot = new AssetNode(AssetNode::Directory);
}

CMikuMikuGameEngineDoc::~CMikuMikuGameEngineDoc()
{
	if( m_pRoot )
	{
		delete m_pRoot;
	}
	if( m_pAssetRoot )
	{
		delete m_pAssetRoot;
	}
}

GameObject* CMikuMikuGameEngineDoc::GetRootGameObject()
{
	return m_pRoot;
}

GameObject* CMikuMikuGameEngineDoc::GetSelectGameObject()
{
	return m_pSelectObject;
}

void CMikuMikuGameEngineDoc::AddGameObject( GameObject* obj,GameObject* parent,bool select )
{
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);

	pMainFrame->GetObjectListView()->AddGameObject( obj,parent,select );

	if( !parent )
	{
		parent = m_pRoot;
	}

	obj->SetParent( parent );
}

void CMikuMikuGameEngineDoc::SetObjectName( GameObject* obj, const tstring& name )
{
	if( name.empty() )
	{
		return;
	}

	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);

	pMainFrame->GetObjectListView()->SetObjectName( obj,name );

	obj->SetName( name );
}

void CMikuMikuGameEngineDoc::DeleteObject( GameObject* obj )
{
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);

	pMainFrame->GetObjectListView()->DeleteObject( obj );

	if( m_pSelectObject == obj )
	{
		m_pSelectObject = NULL;
	}

	delete obj;
}

void CMikuMikuGameEngineDoc::SetObjectParent( GameObject* obj,GameObject* parent )
{
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);

	pMainFrame->GetObjectListView()->SetObjectParent( obj,parent );

	if( parent==NULL )
	{
		parent = m_pRoot;
	}
	obj->SetParent( parent );
}

void CMikuMikuGameEngineDoc::SetSelectObject( GameObject* obj )
{
	m_pSelectObject = obj;
}

AssetNode* CMikuMikuGameEngineDoc::GetRootAsset()
{
	return m_pAssetRoot;
}

AssetNode* CMikuMikuGameEngineDoc::GetSelectAsset()
{
	return m_pSelectAsset;
}

void CMikuMikuGameEngineDoc::AddAssetFiles( const std::vector<tstring>& filePaths,AssetNode* parent,bool select )
{
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);

	if( !parent )
	{
		parent = m_pAssetRoot;
	}

	bool findNut = false;

	for( std::vector<tstring>::const_iterator it = filePaths.begin();it!=filePaths.end();++it )
	{
		int count = 0;

		TCHAR fileName[MAX_PATH];
		_tcscpy_s( fileName,PathFindFileName( (*it).c_str() ) );

		TCHAR fileExt[MAX_PATH];
		_tcscpy_s( fileExt,PathFindExtension( fileName ) );

		PathRemoveExtension( fileName );

		AssetNode::Type type = AssetNode::UnknownFile;

		if( _tcscmp( fileExt,_T(".pmd") ) == 0 )
		{
			type = AssetNode::PMDFile;
		}
		else if( _tcscmp( fileExt,_T(".vmd") ) == 0 )
		{
			type = AssetNode::VMDFile;
		}
		else if( _tcscmp( fileExt,_T(".x") ) == 0 )
		{
			type = AssetNode::XFile;
		}
		else if( _tcscmp( fileExt,_T(".fx") ) == 0 )
		{
			type = AssetNode::EffectFile;
		}
		else if( _tcscmp( fileExt,_T(".nut") ) == 0 )
		{
			type = AssetNode::NutFile;
			findNut = true;
		}

		tstring searchFileName = fileName;
		bool duplicate = false;
		do
		{
			AssetNode* child = parent->GetChild();

			duplicate = false;

			while( child )
			{
				if( child->GetType() == type && child->GetName() == searchFileName )
				{
					count++;
					tstring_format( searchFileName,_T("%s(%d)"),fileName,count );
					duplicate = true;
					break;
				}
				child = child->GetSiblingNext();
			}
		}while( duplicate );

		AssetNode* asset = new AssetNode(type);

		asset->SetName( fileName );
		asset->SetPath( (*it) );
		asset->SetParent( parent );

		pMainFrame->GetAssetExplorer()->AddAsset( asset,parent,select,false );
	}

	if( findNut )
	{
		ScriptManager::GetInstance()->Build( m_pAssetRoot );
	}
}


void CMikuMikuGameEngineDoc::AddAssetFolder( const tstring& folderName,AssetNode* parent,bool select )
{
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);

	if( !parent )
	{
		parent = m_pAssetRoot;
	}

	int count = 0;

	tstring searchFolderName = folderName;
	bool duplicate = false;
	do
	{
		AssetNode* child = parent->GetChild();

		duplicate = false;

		while( child )
		{
			if( child->GetType() == AssetNode::Directory && child->GetName() == searchFolderName )
			{
				count++;
				tstring_format( searchFolderName,_T("%s(%d)"),folderName.c_str(),count );
				duplicate = true;
				break;
			}
			child = child->GetSiblingNext();
		}
	}while( duplicate );

	AssetNode* asset = new AssetNode(AssetNode::Directory);

	asset->SetName( searchFolderName );
	asset->SetParent( parent );

	pMainFrame->GetAssetExplorer()->AddAsset( asset,parent,select,true );
}

void CMikuMikuGameEngineDoc::SetAssetName( AssetNode* asset, const tstring& name )
{
	if( name.empty() )
	{
		return;
	}

	AssetNode* parent = asset->GetParent();

	bool duplicate = false;

	AssetNode* child = parent->GetChild();

	while( child )
	{
		if( child != asset && child->GetType() == asset->GetType() && child->GetName() == name )
		{
			duplicate = true;
			break;
		}
		child = child->GetSiblingNext();
	}

	if( duplicate )
	{
		return;
	}

	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);
	pMainFrame->GetAssetExplorer()->SetAssetName( asset,name );

	asset->SetName( name );
}

void CMikuMikuGameEngineDoc::DeleteAsset( AssetNode* asset )
{
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);

	pMainFrame->GetAssetExplorer()->DeleteAsset( asset );

	if( m_pSelectAsset == asset )
	{
		m_pSelectAsset = NULL;
	}

	delete asset;
}

void CMikuMikuGameEngineDoc::SetAssetParent( AssetNode* asset,AssetNode* parent )
{
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);

	bool duplicate = false;

	AssetNode* child = NULL;

	if( parent )
	{	
		child = parent->GetChild();
	}
	else
	{
		child = m_pAssetRoot->GetChild();
	}

	while( child )
	{
		if( child != asset && child->GetType() == asset->GetType() && child->GetName() == asset->GetName() )
		{
			duplicate = true;
			break;
		}
		child = child->GetSiblingNext();
	}

	if( duplicate )
	{
		return;
	}

	pMainFrame->GetAssetExplorer()->SetAssetParent( asset,parent );

	if( parent==NULL )
	{
		parent = m_pAssetRoot;
	}
	asset->SetParent( parent );
}

void CMikuMikuGameEngineDoc::SetSelectAsset( AssetNode* asset )
{
	m_pSelectAsset = asset;
}

void CMikuMikuGameEngineDoc::DropAsset( AssetNode* asset,CWnd* pDropWnd,const POINT& pt )
{
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);

	if( pMainFrame->GetObjectListView()->IsDropTarget(pDropWnd) )
	{
		AssetNode::Type type = asset->GetType();
		switch( type )
		{
		case AssetNode::XFile:
			{
				tstring xFilePath = asset->GetPath();
				
				TCHAR path[MAX_PATH];
				_tcscpy_s( path,PathFindFileName( xFilePath.c_str() ) );

				PathRemoveExtension( path );

				ModelPtr pModel = ResourceManager::GetInstance().GetResource<Model>( xFilePath );
				if( !pModel )
				{
					XFileLoader xFileLoader;
					pModel = xFileLoader.Open( xFilePath,10.0f );

					ResourceManager::GetInstance().AddResource( xFilePath,pModel );
				}

				if( pModel )
				{
					GameObject* gameObject = new GameObject;

					gameObject->SetName( asset->GetName() );

					ModelRenderer* modelRenderer = new ModelRenderer;
					modelRenderer->SetModel( pModel );

					gameObject->SetModelRenderer( modelRenderer );

					AddGameObject( gameObject,GetRootGameObject(),true );
				}
			}
			break;
		case AssetNode::PMDFile:
			{
				tstring pmdFilePath = asset->GetPath();

				TCHAR path[MAX_PATH];
				_tcscpy_s( path,PathFindFileName( pmdFilePath.c_str() ) );

				PathRemoveExtension( path );

				PMDModelPtr pModel = ResourceManager::GetInstance().GetResource<PMDModel>( pmdFilePath );
				if( !pModel )
				{
					PMDFileLoader pmdFileLoader;
					pModel = pmdFileLoader.Open( pmdFilePath );

					ResourceManager::GetInstance().AddResource( pmdFilePath,pModel );
				}

				if( pModel )
				{
					GameObject* gameObject = new GameObject;

					gameObject->SetName( asset->GetName() );

					PMDModelRenderer* pmdModelRenderer = new PMDModelRenderer;
					pmdModelRenderer->SetGameObject( gameObject );
					pmdModelRenderer->SetModel( pModel );

					gameObject->SetPMDModelRenderer( pmdModelRenderer );

					AddGameObject( gameObject,GetRootGameObject(),true );
				}
			}
			break;
		default:
			break;
		}
	}
}

BOOL CMikuMikuGameEngineDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: この位置に再初期化処理を追加してください。
	// (SDI ドキュメントはこのドキュメントを再利用します。)

	return TRUE;
}




// CMikuMikuGameEngineDoc シリアル化

void CMikuMikuGameEngineDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 格納するコードをここに追加してください。
	}
	else
	{
		// TODO: 読み込むコードをここに追加してください。
	}
}


// CMikuMikuGameEngineDoc 診断

#ifdef _DEBUG
void CMikuMikuGameEngineDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMikuMikuGameEngineDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMikuMikuGameEngineDoc コマンド
