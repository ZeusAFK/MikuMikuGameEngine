
// MikuMikuGameEngineDoc.cpp : CMikuMikuGameEngineDoc �N���X�̎���
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


// CMikuMikuGameEngineDoc �R���X�g���N�V����/�f�X�g���N�V����

CMikuMikuGameEngineDoc::CMikuMikuGameEngineDoc()
	: m_pSelectObject(NULL)
{
	// TODO: ���̈ʒu�� 1 �x�����Ă΂��\�z�p�̃R�[�h��ǉ����Ă��������B

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

	// TODO: ���̈ʒu�ɍď�����������ǉ����Ă��������B
	// (SDI �h�L�������g�͂��̃h�L�������g���ė��p���܂��B)

	return TRUE;
}




// CMikuMikuGameEngineDoc �V���A����

void CMikuMikuGameEngineDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �i�[����R�[�h�������ɒǉ����Ă��������B
	}
	else
	{
		// TODO: �ǂݍ��ރR�[�h�������ɒǉ����Ă��������B
	}
}


// CMikuMikuGameEngineDoc �f�f

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


// CMikuMikuGameEngineDoc �R�}���h
