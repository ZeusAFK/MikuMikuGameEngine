#pragma once

#include "ObjectListTree.h"

#include "MikuMikuGameEngineDoc.h"

// ObjectListView

class ObjectListView : public CDockablePane , public TreeCallbackInterface
{
	DECLARE_DYNAMIC(ObjectListView)

public:
	ObjectListView();
	virtual ~ObjectListView();

	void AdjustLayout();

// 属性
protected:
	ObjectListTree m_wndClassView;

private:
	void AddGameObject( GameObject* obj,HTREEITEM hItemParent,bool select );

public:
	void SetObjectName( GameObject* obj,const tstring& name );
	void AddGameObject( GameObject* obj,GameObject* parent,bool select );
	void DeleteObject( GameObject* obj );

	void SetObjectParent( GameObject* obj,GameObject* parent );

public:
	void OnTreeDropItem( HTREEITEM hDragItem,HTREEITEM hDropTargetItem );
	void OnTreeSelectChanged( HTREEITEM hItem );
	void OnTreeLabelChanged( HTREEITEM hItem,LPCTSTR text );
	void OnTreeDeleteItem( HTREEITEM hItem );

public:
	bool IsDropTarget( CWnd* pDropWnd );

// オーバーライド
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnClassAddMemberFunction();
	afx_msg void OnClassAddMemberVariable();
	afx_msg void OnClassDefinition();
	afx_msg void OnClassProperties();
	afx_msg void OnNewFolder();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnAddObject();
};
