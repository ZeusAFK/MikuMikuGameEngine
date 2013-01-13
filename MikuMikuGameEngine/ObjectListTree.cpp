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

CMikuMikuGameEngineDoc* ObjectListTree::GetDocument() const // デバッグ以外のバージョンはインラインです。
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


// ツリーアイテムをドロップ先へコピーする関数
HTREEITEM ObjectListTree::CopyItem( HTREEITEM hItem, HTREEITEM hParent )
{
	// コピー元アイテムの情報をTVITEM構造体に取得する
	TVITEM tvItem;
	tvItem.hItem = hItem;
	tvItem.mask = TVIF_CHILDREN | TVIF_STATE | TVIF_PARAM |
		TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	GetItem(&tvItem);

	// コピー元アイテムのテキスト情報をTVITEM構造体に取得する
	CString strText = GetItemText(hItem);
	tvItem.cchTextMax = strText.GetLength();
	tvItem.pszText = (LPTSTR)(LPCTSTR)strText;

	// アイテム挿入時のマスクを設定
	tvItem.mask = TVIF_STATE | TVIF_PARAM | TVIF_TEXT |
		TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;

	// TVINSERTSTRUCT構造体を用意し、値を設定
	TVINSERTSTRUCT tvInsert;
	tvInsert.hParent = hParent;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item = tvItem;

	DWORD_PTR pItemData = GetItemData( hItem );

	// コピー先の親アイテム下に新しいアイテムを挿入
	HTREEITEM hRetItem = InsertItem(&tvInsert);

	SetItemData( hRetItem,pItemData );

	// コピー元アイテムが子孫を持つ場合にはそれらもコピーする必要がある
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

	// ドラッグするツリーアイテムのハンドルを取得
	m_hitemDrag = HitTest(pNMTreeView->ptDrag, NULL);

	// ドラッグイメージ（イメージリスト）の作成
	if( !(m_pDragImage = CreateDragImageEx(m_hitemDrag)) )
	{
		return;
	}

	// ドラッグ中フラグをONし、CImageList::BeginDrag関数を実行
	m_bDrag = TRUE;
	m_pDragImage->BeginDrag(0, CPoint(0, 0));

	// CImageList::DragEnter関数を実行（座標はスクリーン座標にする必要あり）
	POINT pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	CImageList::DragEnter(NULL, pt);

	// マウスキャプチャー
	SetCapture();

	*pResult = 0;
}

void ObjectListTree::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	if (m_bDrag)
	{
		// CImageList::DragMove関数を実行（座標はスクリーン座標にする必要あり）
		POINT pt = point;
		ClientToScreen( &pt );
		CImageList::DragMove(pt);

		HTREEITEM hItem = HitTest(point, NULL);
		
		// ドロップ先を強調表示する
		
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
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	if (m_bDrag)
	{
		// ドラッグ中フラグをOFFし、マウスキャプチャーを解放
		m_bDrag = FALSE;
		ReleaseCapture();

		// CImageList::DragLeave関数とCImageList::EndDrag関数を実行
		CImageList::DragLeave(NULL);
		CImageList::EndDrag();

		// ドラッグイメージの破棄
		if( m_pDragImage )
		{
			delete m_pDragImage;
			m_pDragImage = NULL;
		}

		// ドロップ先の強調表示を解除
		SelectDropTarget(NULL);

		POINT pt = point;
		ClientToScreen( &pt );

		CWnd* pDropWnd = CWnd::WindowFromPoint(pt);
		if( pDropWnd != this )
		{
			return;
		}

		// ドロップ先ツリーアイテムのハンドルを取得
		HTREEITEM hItemDrop = HitTest(point, NULL);

		// ドロップ先が存在しない場合は何もしない
		// ドラッグアイテムとドロップ先アイテムが等しい場合は何もしない
		if( m_hitemDrag == hItemDrop || hItemDrop == GetParentItem( m_hitemDrag ) )
		{
			return;
		}

		// ドロップ先がドラッグアイテムの子孫の場合は何もしない
		HTREEITEM hItemParent = hItemDrop;
		while( (hItemParent = GetParentItem( hItemParent )) != NULL )
		{
			if( hItemParent == m_hitemDrag )
			{
				return;
			}
		}

		// アイテムをコピーし、コピー元アイテムを削除
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
	// TODO: ここにメッセージ ハンドラ コードを追加します。

	if (m_bDrag)
	{
		// ドラッグ中フラグをOFFし、マウスキャプチャーを解放
		m_bDrag = FALSE;
		ReleaseCapture();

		// CImageList::DragLeave関数とCImageList::EndDrag関数を実行
		CImageList::DragLeave(NULL);
		CImageList::EndDrag();

		// ドラッグイメージの破棄
		if( m_pDragImage )
		{
			delete m_pDragImage;
			m_pDragImage = NULL;
		}

		// ドロップ先の強調表示を解除
		SelectDropTarget(NULL);
	}

	CViewTree::OnCaptureChanged(pWnd);
}

void ObjectListTree::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

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
	// TODO: ここにコントロール通知ハンドラ コードを追加します。

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
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	if( GetSelectedItem() == NULL )
	{
		OutputDebugString( _T("deselect\n") );
	}
	*pResult = 0;
}

void ObjectListTree::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

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
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

	m_bEditLabelPending = FALSE; 

	CViewTree::OnLButtonDblClk(nFlags, point);
}
