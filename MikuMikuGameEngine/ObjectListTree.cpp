#include "StdAfx.h"
#include "MikuMikuGameEngine.h"

#include "ObjectListTree.h"

#include "MainFrm.h"

#define TCEX_EDITLABEL 1        // Edit label timer event

ObjectListTree::ObjectListTree(void)
{
	m_pDragImage = NULL;
	m_bDrag = FALSE;
	m_hitemDrag = NULL;

	m_hitemMouseDown = NULL;
	m_idTimer = NULL;
	m_bEditLabelPending = FALSE;
}

ObjectListTree::~ObjectListTree(void)
{
}

CMikuMikuGameEngineDoc* ObjectListTree::GetDocument() const // �f�o�b�O�ȊO�̃o�[�W�����̓C�����C���ł��B
{
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(theApp.m_pMainWnd);

	CDocument* pDocument = pMainFrame->GetActiveDocument();
	ASSERT(pDocument->IsKindOf(RUNTIME_CLASS(CMikuMikuGameEngineDoc)));
	return (CMikuMikuGameEngineDoc*)pDocument;
}


HTREEITEM ObjectListTree::SearchItem( GameObject* target,HTREEITEM hItem )
{
	while( hItem )
	{
		GameObject* pObj = (GameObject*)GetItemData( hItem );

		if( pObj == target )
		{
			return hItem;
		}

		HTREEITEM hChild = GetChildItem( hItem );
		HTREEITEM hFindItem = SearchItem( target,hChild );
		if( hFindItem )
		{
			return hFindItem;
		}

		hItem = GetNextSiblingItem( hItem );
	}

	return NULL;
}

void ObjectListTree::AddGameObject( GameObject* obj,HTREEITEM hItemParent,bool select )
{
	HTREEITEM hObjItem = NULL;
	if( hItemParent )
	{
		hObjItem = InsertItem(obj->GetName().c_str(), 3, 3,hItemParent);
	}
	else
	{
		hObjItem = InsertItem(obj->GetName().c_str(), 3, 3);
	}

	SetItemData( hObjItem,(DWORD_PTR)obj );
	
	GameObject* child = obj->GetChild();
	while( child )
	{
		AddGameObject( child,hObjItem,false );

		child = child->GetSiblingNext();
	}

	if( select )
	{ 
		SelectItem( hObjItem );
	}

	//EditLabel( hObjItem );
}


// �c���[�A�C�e�����h���b�v��փR�s�[����֐�
HTREEITEM ObjectListTree::CopyItem( HTREEITEM hItem, HTREEITEM hParent )
{
	// �R�s�[���A�C�e���̏���TVITEM�\���̂Ɏ擾����
	TVITEM tvItem;
	tvItem.hItem = hItem;
	tvItem.mask = TVIF_CHILDREN | TVIF_STATE | TVIF_PARAM |
		TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	GetItem(&tvItem);

	// �R�s�[���A�C�e���̃e�L�X�g����TVITEM�\���̂Ɏ擾����
	CString strText = GetItemText(hItem);
	tvItem.cchTextMax = strText.GetLength();
	tvItem.pszText = (LPTSTR)(LPCTSTR)strText;

	// �A�C�e���}�����̃}�X�N��ݒ�
	tvItem.mask = TVIF_STATE | TVIF_PARAM | TVIF_TEXT |
		TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	// TVINSERTSTRUCT�\���̂�p�ӂ��A�l��ݒ�
	TVINSERTSTRUCT tvInsert;
	tvInsert.hParent = hParent;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item = tvItem;

	DWORD_PTR pItemData = GetItemData( hItem );

	// �R�s�[��̐e�A�C�e�����ɐV�����A�C�e����}��
	HTREEITEM hRetItem = InsertItem(&tvInsert);

	SetItemData( hRetItem,pItemData );

	// �R�s�[���A�C�e�����q�������ꍇ�ɂ͂������R�s�[����K�v������
	HTREEITEM hChild = GetChildItem(hItem);
	while( hChild )
	{
		CopyItem(hChild, hRetItem );
		hChild = GetNextSiblingItem(hChild);
	}

	return hRetItem;
}

BEGIN_MESSAGE_MAP(ObjectListTree, CViewTree)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &ObjectListTree::OnTvnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CAPTURECHANGED()
	//ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &ObjectListTree::OnTvnSelchanged)
	ON_NOTIFY_REFLECT(TVN_SELCHANGING, &ObjectListTree::OnTvnSelchanging)
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// http://www.vchome.net/tech/document/control/DragWithoutImage.htm
CImageList* ObjectListTree::CreateDragImageEx(HTREEITEM hItem)
{
	if(GetImageList(TVSIL_NORMAL) != NULL)
	{
		return CreateDragImage(hItem);
	}

	CRect rect;
	GetItemRect(hItem, rect, TRUE);
	rect.top = rect.left = 0;

	// Create bitmap
	CClientDC dc (this);
	CDC  memDC; 

	if(!memDC.CreateCompatibleDC(&dc))
	{
		return NULL;
	}

	CBitmap bitmap;
	if(!bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height()))
	{
		return NULL;
	}

	CBitmap* pOldMemDCBitmap = memDC.SelectObject( &bitmap );
	CFont* pOldFont = memDC.SelectObject(GetFont());

	memDC.FillSolidRect(&rect, RGB(255, 255, 255)); // Here green is used as mask color
	memDC.SetTextColor(GetSysColor(COLOR_GRAYTEXT));
	memDC.TextOut(rect.left, rect.top, GetItemText(hItem));

	memDC.SelectObject( pOldFont );
	memDC.SelectObject( pOldMemDCBitmap );

	// Create imagelist
	CImageList* pImageList = new CImageList;
	pImageList->Create(rect.Width(), rect.Height(), ILC_COLOR | ILC_MASK, 0, 1);

	pImageList->Add(&bitmap, RGB(255, 255, 255)); // Here green is used as mask color

	return pImageList;
}


void ObjectListTree::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	// �h���b�O����c���[�A�C�e���̃n���h�����擾
	m_hitemDrag = HitTest(pNMTreeView->ptDrag, NULL);

	// �h���b�O�C���[�W�i�C���[�W���X�g�j�̍쐬
	if( !(m_pDragImage = CreateDragImageEx(m_hitemDrag)) )
	{
		return;
	}

	// �h���b�O���t���O��ON���ACImageList::BeginDrag�֐������s
	m_bDrag = TRUE;
	m_pDragImage->BeginDrag(0, CPoint(0, 0));

	// CImageList::DragEnter�֐������s�i���W�̓X�N���[�����W�ɂ���K�v����j
	POINT pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	CImageList::DragEnter(NULL, pt);

	// �}�E�X�L���v�`���[
	SetCapture();

	*pResult = 0;
}

void ObjectListTree::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
	if (m_bDrag)
	{
		// CImageList::DragMove�֐������s�i���W�̓X�N���[�����W�ɂ���K�v����j
		POINT pt = point;
		ClientToScreen( &pt );
		CImageList::DragMove(pt);

		HTREEITEM hItem = HitTest(point, NULL);
		
		// �h���b�v��������\������
		
		CImageList::DragShowNolock(FALSE);
		SelectDropTarget(hItem);
		CImageList::DragShowNolock(TRUE);
	}
	else
	{
		if( m_hitemMouseDown )
		{
			CSize sizeMoved = m_mouseDownPoint-point;   
   
			if ( abs(sizeMoved.cx) > GetSystemMetrics( SM_CXDRAG ) || abs(sizeMoved.cy) > GetSystemMetrics( SM_CYDRAG ) )   
			{   
				CWnd* pWnd = GetParent();   
				if ( pWnd && !( GetStyle() & TVS_DISABLEDRAGDROP ) )   
				{   
					NM_TREEVIEW tv;   
	   
					tv.hdr.hwndFrom = GetSafeHwnd();   
					tv.hdr.idFrom = GetWindowLong( GetSafeHwnd(), GWL_ID );   
					tv.hdr.code = TVN_BEGINDRAG;   
	   
					tv.itemNew.hItem = m_hitemMouseDown;   
					tv.itemNew.state = GetItemState( m_hitemMouseDown, 0xffffffff );   
					tv.itemNew.lParam = GetItemData( m_hitemMouseDown );   
	   
					tv.ptDrag.x = point.x;   
					tv.ptDrag.y = point.y;   
	   
					pWnd->SendMessage( WM_NOTIFY, tv.hdr.idFrom, (LPARAM)&tv );

					m_bEditLabelPending = FALSE;
				}   
	   
				m_hitemMouseDown=NULL;
			}
		}
	}

	CViewTree::OnMouseMove(nFlags, point);
}

void ObjectListTree::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
	if (m_bDrag)
	{
		// �h���b�O���t���O��OFF���A�}�E�X�L���v�`���[�����
		m_bDrag = FALSE;
		ReleaseCapture();

		// CImageList::DragLeave�֐���CImageList::EndDrag�֐������s
		CImageList::DragLeave(NULL);
		CImageList::EndDrag();

		// �h���b�O�C���[�W�̔j��
		if( m_pDragImage )
		{
			delete m_pDragImage;
			m_pDragImage = NULL;
		}

		// �h���b�v��̋����\��������
		SelectDropTarget(NULL);

		POINT pt = point;
		ClientToScreen( &pt );

		CWnd* pDropWnd = CWnd::WindowFromPoint(pt);
		if( pDropWnd != this )
		{
			return;
		}

		// �h���b�v��c���[�A�C�e���̃n���h�����擾
		HTREEITEM hItemDrop = HitTest(point, NULL);

		// �h���b�v�悪���݂��Ȃ��ꍇ�͉������Ȃ�
		// �h���b�O�A�C�e���ƃh���b�v��A�C�e�����������ꍇ�͉������Ȃ�
		if( m_hitemDrag == hItemDrop || hItemDrop == GetParentItem( m_hitemDrag ) )
		{
			return;
		}

		// �h���b�v�悪�h���b�O�A�C�e���̎q���̏ꍇ�͉������Ȃ�
		HTREEITEM hItemParent = hItemDrop;
		while( (hItemParent = GetParentItem( hItemParent )) != NULL )
		{
			if( hItemParent == m_hitemDrag )
			{
				return;
			}
		}

		// �A�C�e�����R�s�[���A�R�s�[���A�C�e�����폜
		GameObject* obj = (GameObject*)GetItemData(m_hitemDrag);
		GameObject* parent = NULL;
		if( hItemDrop )
		{
			parent = (GameObject*)GetItemData(hItemDrop);
		}

		GetDocument()->SetObjectParent( obj,parent );

		m_hitemDrag = NULL;
	}
	else
	{
		m_hitemMouseDown = NULL;

		//UINT flags = 0;
		//HTREEITEM hItem = HitTest(point, &flags);

		//if( ( GetStyle() & TVS_EDITLABELS ) && ( flags & TVHT_ONITEMLABEL ) )
		//{
		//	BOOL bIsClickedItemSelected = (hItem == GetSelectedItem());//GetItemState( hItem, TVIS_SELECTED ) & TVIS_SELECTED;

		//	if( bIsClickedItemSelected )
		//	{
		//		SetFocus();

		//		// Invoke label editing    
		//		m_bEditLabelPending = TRUE;   
		//		m_idTimer = SetTimer(TCEX_EDITLABEL, GetDoubleClickTime(), NULL);   

		//		return;
		//	}
		//}
	}

	CViewTree::OnLButtonUp(nFlags, point);
}
void ObjectListTree::OnCaptureChanged(CWnd *pWnd)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����܂��B

	if (m_bDrag)
	{
		// �h���b�O���t���O��OFF���A�}�E�X�L���v�`���[�����
		m_bDrag = FALSE;
		ReleaseCapture();

		// CImageList::DragLeave�֐���CImageList::EndDrag�֐������s
		CImageList::DragLeave(NULL);
		CImageList::EndDrag();

		// �h���b�O�C���[�W�̔j��
		if( m_pDragImage )
		{
			delete m_pDragImage;
			m_pDragImage = NULL;
		}

		// �h���b�v��̋����\��������
		SelectDropTarget(NULL);
	}

	CViewTree::OnCaptureChanged(pWnd);
}

void ObjectListTree::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	if( GetEditControl() )
	{
		EndEditLabelNow(TRUE);
	}

	UINT flags = 0;
	HTREEITEM hItem = HitTest(point, &flags);

	m_bEditLabelPending = FALSE;

	UINT checkFlags = TVHT_ONITEM;

	if( GetStyle() & TVS_FULLROWSELECT )
	{
		checkFlags |= TVHT_ONITEMINDENT | TVHT_ONITEMRIGHT;
	}

	if( (flags & checkFlags)!=0 )
	{
		SetFocus();

		m_hitemMouseDown = hItem;

		m_mouseDownPoint = point;

		SelectItem(hItem);
	}
	else
	{
		SelectItem(NULL);
		//HTREEITEM hSelItem = GetSelectedItem();
		//if( hSelItem )
		//{
		//	SetItemState( hSelItem,0,TVIS_SELECTED );
		//}
	}

	//CViewTree::OnLButtonDown(nFlags, point);
}

void ObjectListTree::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B

	GameObject* obj = NULL;
	if( pNMTreeView->itemNew.hItem )
	{
		obj = (GameObject*)GetItemData( pNMTreeView->itemNew.hItem );
	}
	GetDocument()->SetSelectObject( obj );

	*pResult = 0;
}

void ObjectListTree::OnTvnSelchanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	if( GetSelectedItem() == NULL )
	{
		OutputDebugString( _T("deselect\n") );
	}
	*pResult = 0;
}

void ObjectListTree::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	if (nIDEvent == TCEX_EDITLABEL)   
    {   
        // Stop the timer.    
        KillTimer(m_idTimer);   
   
        // Invoke label editing.    
        if (m_bEditLabelPending)   
            EditLabel(GetSelectedItem());   
   
        m_bEditLabelPending = FALSE;   
        return;   
    }

	CViewTree::OnTimer(nIDEvent);
}

void ObjectListTree::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	m_bEditLabelPending = FALSE; 

	CViewTree::OnLButtonDblClk(nFlags, point);
}
