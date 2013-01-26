
#pragma once

#include "ViewTree.h"

class CAssetExplorerToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CAssetExplorer : public CDockablePane , public TreeCallbackInterface
{
// コンストラクション
public:
	CAssetExplorer();

	void AdjustLayout();
	void OnChangeVisualStyle();

// 属性
protected:

	ObjectListTree m_wndFileView;
	CImageList m_AssetExplorerImages;
	CAssetExplorerToolBar m_wndToolBar;

protected:
	void FillFileView();

// 実装
public:
	virtual ~CAssetExplorer();

private:
	void AddAsset( AssetNode* asset,HTREEITEM hItemParent,bool select,bool editLabel );
public:
	void AddAsset( AssetNode* asset,AssetNode* parent,bool select,bool editLabel );
	void SetAssetName( AssetNode* asset,const tstring& name );
	void DeleteAsset( AssetNode* asset );
	void SetAssetParent( AssetNode* asset,AssetNode* parent );

public:
	void OnTreeLabelChanged( HTREEITEM hItem,LPCTSTR text );
	void OnTreeDeleteItem( HTREEITEM hItem );
	void OnTreeDropItem( HTREEITEM hDragItem,HTREEITEM hDropTargetItem );
	void OnTreeDropItem( HTREEITEM hDragItem,CWnd* pDropWnd,const POINT& pt );

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenWith();
	afx_msg void OnDummyCompile();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnAddFolder();
	afx_msg void OnOpenAsset();
};

