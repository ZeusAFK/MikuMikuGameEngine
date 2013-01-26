#pragma once
#include "viewtree.h"

class TreeCallbackInterface
{
public:
	virtual void OnTreeDropItem( HTREEITEM hDragItem,HTREEITEM hDropTargetItem ){};
	virtual void OnTreeDropItem( HTREEITEM hDragItem,CWnd* pDropWnd,const POINT& pt ){};
	virtual void OnTreeSelectChanged( HTREEITEM hItem ){};
	virtual void OnTreeLabelChanged( HTREEITEM hItem,LPCTSTR text ){};
	virtual void OnTreeDeleteItem( HTREEITEM hItem ){};
};

class ObjectListTree : public CViewTree
{
private:
	CImageList*  m_pDragImage;  // �h���b�O�C���[�W���i�[����|�C���^
	BOOL         m_bDrag;       // �h���b�O���Ȃ�TRUE�A�h���b�O���łȂ����FALSE
	HTREEITEM    m_hitemDrag;   // �h���b�O�A�C�e���i�R�s�[���A�C�e���j�̃n���h��

	CPoint		m_mouseDownPoint;
	HTREEITEM	m_hitemMouseDown;
	UINT_PTR	m_idTimer;
	BOOL		m_bEditLabelPending;

	TreeCallbackInterface* m_callBack;

// ����
public:
	CImageList* CreateDragImageEx(HTREEITEM hItem);

public:
	void SetCallBack( TreeCallbackInterface* callBack );

public:
	HTREEITEM SearchItem( DWORD_PTR target,HTREEITEM hItem );

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

	afx_msg void OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};
