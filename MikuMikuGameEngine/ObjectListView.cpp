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
	ON_NOTIFY(TVN_BEGINLABELEDIT,2,OnBeginLabelEdit)
	ON_NOTIFY(TVN_ENDLABELEDIT,2,OnEndLabelEdit)
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

	return 0;
}

CMikuMikuGameEngineDoc* ObjectListView::GetDocument() const // �f�o�b�O�ȊO�̃o�[�W�����̓C�����C���ł��B
{
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);

	CDocument* pDocument = pMainFrame->GetActiveDocument();
	ASSERT(pDocument->IsKindOf(RUNTIME_CLASS(CMikuMikuGameEngineDoc)));
	return (CMikuMikuGameEngineDoc*)pDocument;
}

void ObjectListView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void ObjectListView::SetObjectName( GameObject* obj,const tstring& name )
{
	HTREEITEM hItem = m_wndClassView.GetRootItem();

	hItem = m_wndClassView.SearchItem( obj,hItem );
	if( hItem )
	{
		m_wndClassView.SetItemText(hItem,name.c_str());
	}
}

void ObjectListView::AddGameObject( GameObject* obj,GameObject* parent,bool select )
{
	m_wndClassView.AddGameObject( obj,m_wndClassView.SearchItem( parent,m_wndClassView.GetRootItem() ),select  );
}

void ObjectListView::DeleteObject( GameObject* obj )
{
	HTREEITEM hItem = m_wndClassView.GetRootItem();

	hItem = m_wndClassView.SearchItem( obj,hItem );
	if( hItem )
	{
		m_wndClassView.DeleteItem( hItem );
	}
}

void ObjectListView::SetObjectParent( GameObject* obj,GameObject* parent )
{
	HTREEITEM hRootItem = m_wndClassView.GetRootItem();
	HTREEITEM hItem = m_wndClassView.SearchItem( obj,hRootItem );
	HTREEITEM hParentItem = m_wndClassView.SearchItem( parent,hRootItem );

	HTREEITEM hRetItem = m_wndClassView.CopyItem( hItem,hParentItem );
	m_wndClassView.DeleteItem( hItem );

	m_wndClassView.SelectItem( hRetItem );
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

BOOL ObjectListView::PreTranslateMessage(MSG* pMsg)
{
	if( m_wndClassView.GetEditControl() )
	{  
		if( m_wndClassView.SendMessage(WM_GETDLGCODE) & (DLGC_WANTALLKEYS | DLGC_WANTCHARS | DLGC_WANTMESSAGE) )
		{
			::TranslateMessage(pMsg);  
			::DispatchMessage(pMsg);  

			return TRUE;  
		}
	}
	else
	{
		if ( pMsg->message == WM_KEYUP )
		{
			HTREEITEM hItem = m_wndClassView.GetSelectedItem();
			if( hItem!=NULL )
			{
				if( pMsg->wParam == VK_F2 )
				{
					m_wndClassView.EditLabel(hItem);
				}
				else if( pMsg->wParam == VK_DELETE )
				{
					OnDeleteObject();
				}
			}
		} 
	}

	return CDockablePane::PreTranslateMessage(pMsg);
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

void ObjectListView::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)    
{   
    TV_DISPINFO* pTVDI = (TV_DISPINFO*)pNMHDR;
    if(pTVDI->item.lParam==0)
	{
        *pResult = 1;   
	}
    else
	{
        *pResult = 0;
	}
}   

void ObjectListView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)    
{   
    TV_DISPINFO* pTVDI = (TV_DISPINFO*)pNMHDR;
    if(pTVDI->item.pszText!=NULL)
    {   
		GameObject* obj = (GameObject*)m_wndClassView.GetItemData( pTVDI->item.hItem );
		GetDocument()->SetObjectName( obj,pTVDI->item.pszText );
    }   
    *pResult = 0;   
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

	GetDocument()->AddGameObject( new GameObject,parent,true );
}

void ObjectListView::OnDeleteObject()
{
	// TODO: �����ɃR�}���h �n���h�� �R�[�h��ǉ����܂��B

	HTREEITEM hItem = m_wndClassView.GetSelectedItem();
	if( hItem==NULL )
	{
		return;
	}

	GameObject* obj = (GameObject*)m_wndClassView.GetItemData(hItem);
	if( obj )
	{
		if( MessageBox( _T("�폜���܂���?"),_T("�m�F"),MB_YESNO | MB_ICONQUESTION ) == IDYES )
		{
			GetDocument()->DeleteObject( obj );
		}
	}

	SetFocus();
}
