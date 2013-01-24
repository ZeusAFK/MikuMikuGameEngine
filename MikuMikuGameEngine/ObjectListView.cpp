// ObjectListView.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "MikuMikuGameEngine.h"

#include "ObjectListView.h"

#include "MainFrm.h"


// ObjectListView

IMPLEMENT_DYNAMIC(ObjectListView, CDockablePane)

ObjectListView::ObjectListView()
{
}

ObjectListView::~ObjectListView()
{
}


BEGIN_MESSAGE_MAP(ObjectListView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CLASS_ADD_MEMBER_FUNCTION, OnClassAddMemberFunction)
	ON_COMMAND(ID_CLASS_ADD_MEMBER_VARIABLE, OnClassAddMemberVariable)
	ON_COMMAND(ID_CLASS_DEFINITION, OnClassDefinition)
	ON_COMMAND(ID_CLASS_PROPERTIES, OnClassProperties)
	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_ADD_OBJECT, &ObjectListView::OnAddObject)
END_MESSAGE_MAP()



// ObjectListView ���b�Z�[�W �n���h��

int ObjectListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// �r���[�̍쐬:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS | TVS_TRACKSELECT | TVS_FULLROWSELECT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	
	if (!m_wndClassView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("�N���X �r���[���쐬�ł��܂���ł���\n");
		return -1;      // �쐬�ł��Ȃ��ꍇ
	}

	m_wndClassView.SetCallBack( this );

	return 0;
}

void ObjectListView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void ObjectListView::SetObjectName( GameObject* obj,const tstring& name )
{
	HTREEITEM hItem = m_wndClassView.GetRootItem();

	hItem = m_wndClassView.SearchItem( (DWORD_PTR)obj,hItem );
	if( hItem )
	{
		m_wndClassView.SetItemText(hItem,name.c_str());
	}
}

void ObjectListView::AddGameObject( GameObject* obj,HTREEITEM hItemParent,bool select )
{
	HTREEITEM hObjItem = NULL;
	if( hItemParent )
	{
		hObjItem = m_wndClassView.InsertItem(obj->GetName().c_str(), 3, 3,hItemParent);
	}
	else
	{
		hObjItem = m_wndClassView.InsertItem(obj->GetName().c_str(), 3, 3);
	}

	m_wndClassView.SetItemData( hObjItem,(DWORD_PTR)obj );
	
	GameObject* child = obj->GetChild();
	while( child )
	{
		AddGameObject( child,hObjItem,false );

		child = child->GetSiblingNext();
	}

	if( select )
	{ 
		m_wndClassView.SelectItem( hObjItem );
	}

	//EditLabel( hObjItem );
}

void ObjectListView::AddGameObject( GameObject* obj,GameObject* parent,bool select )
{
	AddGameObject( obj,m_wndClassView.SearchItem( (DWORD_PTR)parent,m_wndClassView.GetRootItem() ),select  );
}

void ObjectListView::DeleteObject( GameObject* obj )
{
	HTREEITEM hItem = m_wndClassView.GetRootItem();

	hItem = m_wndClassView.SearchItem( (DWORD_PTR)obj,hItem );
	if( hItem )
	{
		m_wndClassView.DeleteItem( hItem );
	}
}

void ObjectListView::SetObjectParent( GameObject* obj,GameObject* parent )
{
	HTREEITEM hRootItem = m_wndClassView.GetRootItem();
	HTREEITEM hItem = m_wndClassView.SearchItem( (DWORD_PTR)obj,hRootItem );
	HTREEITEM hParentItem = m_wndClassView.SearchItem( (DWORD_PTR)parent,hRootItem );

	HTREEITEM hRetItem = m_wndClassView.CopyItem( hItem,hParentItem );
	m_wndClassView.DeleteItem( hItem );

	m_wndClassView.SelectItem( hRetItem );
}

void ObjectListView::OnTreeDropItem( HTREEITEM hDragItem,HTREEITEM hDropTargetItem )
{
	// �A�C�e�����R�s�[���A�R�s�[���A�C�e�����폜
	GameObject* obj = (GameObject*)m_wndClassView.GetItemData(hDragItem);
	GameObject* parent = NULL;
	if( hDropTargetItem )
	{
		parent = (GameObject*)m_wndClassView.GetItemData(hDropTargetItem);
	}

	theApp.GetDocument()->SetObjectParent( obj,parent );
}

void ObjectListView::OnTreeSelectChanged( HTREEITEM hItem )
{
	GameObject* obj = NULL;
	if( hItem )
	{
		obj = (GameObject*)m_wndClassView.GetItemData( hItem );
	}
	theApp.GetDocument()->SetSelectObject( obj );
}

void ObjectListView::OnTreeLabelChanged( HTREEITEM hItem,LPCTSTR text )
{
	GameObject* obj = (GameObject*)m_wndClassView.GetItemData( hItem );
	theApp.GetDocument()->SetObjectName( obj,text );
}

void ObjectListView::OnTreeDeleteItem( HTREEITEM hItem )
{
	GameObject* obj = (GameObject*)m_wndClassView.GetItemData(hItem);
	if( obj )
	{
		if( MessageBox( _T("�폜���܂���?"),_T("�m�F"),MB_YESNO | MB_ICONQUESTION ) == IDYES )
		{
			theApp.GetDocument()->DeleteObject( obj );
		}
	}
}

void ObjectListView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndClassView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// �N���b�N���ꂽ���ڂ̑I��:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		
		pWndTree->SelectItem(hTreeItem);
	}

	pWndTree->SetFocus();
	CMenu menu;
	menu.LoadMenu(IDR_POPUP_OBJECTLIST);

	CMenu* pSubMenu = menu.GetSubMenu(0);

	if( pSubMenu != NULL )
    {
        pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
    }

	//if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	//{
	//	CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

	//	if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
	//		return;

	//	((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
	//	UpdateDialogControls(this, FALSE);
	//}
}

void ObjectListView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_wndClassView.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void ObjectListView::OnClassAddMemberFunction()
{
	AfxMessageBox(_T("�����o�֐��̒ǉ�..."));
}

void ObjectListView::OnClassAddMemberVariable()
{
	// TODO: �����ɃR�}���h �n���h�� �R�[�h��ǉ����܂�
}

void ObjectListView::OnClassDefinition()
{
	// TODO: �����ɃR�}���h �n���h�� �R�[�h��ǉ����܂�
}

void ObjectListView::OnClassProperties()
{
	// TODO: �����ɃR�}���h �n���h�� �R�[�h��ǉ����܂�
}

void ObjectListView::OnNewFolder()
{
	AfxMessageBox(_T("�V�����t�H���_..."));
}

void ObjectListView::OnPaint()
{
	CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

	CRect rectTree;
	m_wndClassView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void ObjectListView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndClassView.SetFocus();
}

void ObjectListView::OnAddObject()
{
	// TODO: �����ɃR�}���h �n���h�� �R�[�h��ǉ����܂��B
	HTREEITEM hItem = m_wndClassView.GetSelectedItem();

	GameObject* parent = NULL;
	if( hItem )
	{
		parent = (GameObject*)m_wndClassView.GetItemData(hItem);
	}

	theApp.GetDocument()->AddGameObject( new GameObject,parent,true );
}

