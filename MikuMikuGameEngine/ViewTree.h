
#pragma once

#include "MikuMikuGameEngineDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CViewTree ウィンドウ

class CViewTree : public CTreeCtrl
{
// コンストラクション
public:
	CViewTree();

// オーバーライド
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// 実装
public:
	virtual ~CViewTree();

protected:
	DECLARE_MESSAGE_MAP()
};
