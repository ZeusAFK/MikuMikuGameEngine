#pragma once
#include "viewtree.h"

#include "MikuMikuGameEngineDoc.h"

class ObjectListTree : public CViewTree
{
private:
	CImageList*  m_pDragImage;  // ドラッグイメージを格納するポインタ
	BOOL         m_bDrag;       // ドラッグ中ならTRUE、ドラッグ中でなければFALSE
	HTREEITEM    m_hitemDrag;   // ドラッグアイテム（コピー元アイテム）のハンドル

	CPoint		m_mouseDownPoint;
	HTREEITEM	m_hitemMouseDown;
	UINT_PTR	m_idTimer;
	BOOL		m_bEditLabelPending;

// 属性
public:
	CMikuMikuGameEngineDoc* GetDocument() const;

	CImageList* CreateDragImageEx(HTREEITEM hItem);

public:
	HTREEITEM SearchItem( GameObject* target,HTREEITEM hItem );
	void AddGameObject( GameObject* obj,HTREEITEM hItemParent,bool select );

	HTREEITEM CopyItem( HTREEITEM hItem, HTREEITEM hParent );

public:
	ObjectListTree(void);
	~ObjectListTree(void);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchanging(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};
