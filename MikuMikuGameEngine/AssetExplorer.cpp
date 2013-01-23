
#include "stdafx.h"
#include "mainfrm.h"
#include "AssetExplorer.h"
#include "Resource.h"
#include "MikuMikuGameEngine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileView

CAssetExplorer::CAssetExplorer()
{
}

CAssetExplorer::~CAssetExplorer()
{
}

BEGIN_MESSAGE_MAP(CAssetExplorer, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_OPEN, OnFileOpen)
	ON_COMMAND(ID_OPEN_WITH, OnFileOpenWith)
	ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_ADD_FOLDER, &CAssetExplorer::OnAddFolder)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar メッセージ ハンドラ

int CAssetExplorer::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// ビューの作成:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS | TVS_TRACKSELECT | TVS_FULLROWSELECT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	//const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("ファイル ビューを作成できませんでした\n");
		return -1;      // 作成できない場合
	}

	// ビューのイメージの読み込み:
	m_AssetExplorerImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndFileView.SetImageList(&m_AssetExplorerImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* ロックされています*/);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// すべてのコマンドが、親フレーム経由ではなくこのコントロール経由で渡されます:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// 静的ツリー ビュー データ (ダミー コード) を入力します
	//FillFileView();
	AdjustLayout();

	return 0;
}

void CAssetExplorer::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CAssetExplorer::FillFileView()
{
	HTREEITEM hRoot = m_wndFileView.InsertItem(_T("FakeApp ファイル"), 0, 0);
	m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	HTREEITEM hSrc = m_wndFileView.InsertItem(_T("FakeApp ソース ファイル"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("FakeApp.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("FakeApp.rc"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("FakeAppDoc.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("FakeAppView.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("MainFrm.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("StdAfx.cpp"), 1, 1, hSrc);

	HTREEITEM hInc = m_wndFileView.InsertItem(_T("FakeApp ヘッダー ファイル"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("FakeApp.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("FakeAppDoc.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("FakeAppView.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("Resource.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("MainFrm.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("StdAfx.h"), 2, 2, hInc);

	HTREEITEM hRes = m_wndFileView.InsertItem(_T("FakeApp リソース ファイル"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("FakeApp.ico"), 2, 2, hRes);
	m_wndFileView.InsertItem(_T("FakeApp.rc2"), 2, 2, hRes);
	m_wndFileView.InsertItem(_T("FakeAppDoc.ico"), 2, 2, hRes);
	m_wndFileView.InsertItem(_T("FakeToolbar.bmp"), 2, 2, hRes);

	m_wndFileView.Expand(hRoot, TVE_EXPAND);
	m_wndFileView.Expand(hSrc, TVE_EXPAND);
	m_wndFileView.Expand(hInc, TVE_EXPAND);
}

void CAssetExplorer::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndFileView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// クリックされた項目の選択:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		
		pWndTree->SelectItem(hTreeItem);
	}

	pWndTree->SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CAssetExplorer::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndFileView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CAssetExplorer::AddAsset( AssetNode* asset,HTREEITEM hItemParent,bool select )
{
	int imageID = 0;
	switch( asset->GetType() )
	{
	case AssetNode::DIRECTORY:
		imageID = 0;
		break;
	case AssetNode::FILE:
		imageID = 2;
		break;
	}

	HTREEITEM hObjItem = NULL;
	if( hItemParent )
	{
		hObjItem = m_wndFileView.InsertItem(asset->GetName().c_str(), imageID, imageID,hItemParent);
	}
	else
	{
		hObjItem = m_wndFileView.InsertItem(asset->GetName().c_str(), imageID, imageID);
	}

	m_wndFileView.SetItemData( hObjItem,(DWORD_PTR)asset );
	
	if( asset->GetType() == AssetNode::DIRECTORY )
	{
		AssetNode* child = asset->GetChild();
		while( child )
		{
			AddAsset( child,hObjItem,false );

			child = child->GetSiblingNext();
		}
	}

	if( select )
	{ 
		m_wndFileView.SelectItem( hObjItem );
		m_wndFileView.EditLabel( hObjItem );
	}
}

void CAssetExplorer::AddAsset( AssetNode* asset,AssetNode* parent,bool select )
{
	AddAsset( asset,m_wndFileView.SearchItem( (DWORD_PTR)parent,m_wndFileView.GetRootItem() ),select  );
}

void CAssetExplorer::OnProperties()
{
	AfxMessageBox(_T("プロパティ..."));

}

void CAssetExplorer::OnFileOpen()
{
	// TODO: ここにコマンド ハンドラ コードを追加します
}

void CAssetExplorer::OnFileOpenWith()
{
	// TODO: ここにコマンド ハンドラ コードを追加します
}

void CAssetExplorer::OnDummyCompile()
{
	// TODO: ここにコマンド ハンドラ コードを追加します
}

void CAssetExplorer::OnEditCut()
{
	// TODO: ここにコマンド ハンドラ コードを追加します
}

void CAssetExplorer::OnEditCopy()
{
	// TODO: ここにコマンド ハンドラ コードを追加します
}

void CAssetExplorer::OnEditClear()
{
	// TODO: ここにコマンド ハンドラ コードを追加します
}

void CAssetExplorer::OnPaint()
{
	CPaintDC dc(this); // 描画のデバイス コンテキスト

	CRect rectTree;
	m_wndFileView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CAssetExplorer::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndFileView.SetFocus();
}

void CAssetExplorer::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* ロックされました*/);

	m_AssetExplorerImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_FILE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("ビットマップを読み込めませんでした: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_AssetExplorerImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_AssetExplorerImages.Add(&bmp, RGB(255, 0, 255));

	m_wndFileView.SetImageList(&m_AssetExplorerImages, TVSIL_NORMAL);
}

void CAssetExplorer::OnAddFolder()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	HTREEITEM hItem = m_wndFileView.GetSelectedItem();

	AssetNode* parentAsset = NULL;
	if( hItem )
	{
		AssetNode* selectAsset = (AssetNode*)m_wndFileView.GetItemData(hItem);
		if( selectAsset )
		{
			if( selectAsset->GetType() == AssetNode::DIRECTORY )
			{
				parentAsset = selectAsset;
			}
			else
			{
				parentAsset = selectAsset->GetParent();
			}
		}
	}

	theApp.GetDocument()->AddAssetFolder( _T("新しいフォルダー"),parentAsset,true );
}
