
#pragma once

#include "engine/core/util/Charset.h"

/////////////////////////////////////////////////////////////////////////////
// COutputList ウィンドウ

class COutputList : public CListBox
{
// コンストラクション
public:
	COutputList();

// 実装
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();

	DECLARE_MESSAGE_MAP()
};

class COutputWnd : public CDockablePane
{
// コンストラクション
public:
	COutputWnd();

// 属性
protected:
	CFont m_Font;

	CEdit m_wndOutputDebug;

protected:
	void FillDebugWindow();

public:
	void AddString( const tstring& text );

// 実装
public:
	virtual ~COutputWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

