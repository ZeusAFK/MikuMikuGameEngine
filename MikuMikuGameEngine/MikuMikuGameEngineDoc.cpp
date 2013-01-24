
// MikuMikuGameEngineDoc.cpp : CMikuMikuGameEngineDoc クラスの実装
//

#include "stdafx.h"
#include "MikuMikuGameEngine.h"

#include "MikuMikuGameEngineDoc.h"

#include "MainFrm.h"

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
	m_pAssetRoot = new AssetNode(AssetNode::DIRECTORY);
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

void CMikuMikuGameEngineDoc::AddAsset( AssetNode* asset,AssetNode* parent,bool select )
{
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);

	//pMainFrame->GetAssetExplorer()->AddAsset( asset,parent,select );

	if( !parent )
	{
		parent = m_pAssetRoot;
	}

	asset->SetParent( parent );
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
			if( child->GetType() == AssetNode::DIRECTORY && child->GetName() == searchFolderName )
			{
				count++;
				tstring_format( searchFolderName,_T("%s(%d)"),folderName.c_str(),count );
				duplicate = true;
				break;
			}
			child = child->GetSiblingNext();
		}
	}while( duplicate );

	AssetNode* asset = new AssetNode(AssetNode::DIRECTORY);

	asset->SetName( searchFolderName );
	asset->SetParent( parent );

	pMainFrame->GetAssetExplorer()->AddAsset( asset,parent,select );
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
