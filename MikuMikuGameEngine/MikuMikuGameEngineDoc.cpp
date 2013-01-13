
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
}

CMikuMikuGameEngineDoc::~CMikuMikuGameEngineDoc()
{
	if( m_pRoot )
	{
		delete m_pRoot;
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
