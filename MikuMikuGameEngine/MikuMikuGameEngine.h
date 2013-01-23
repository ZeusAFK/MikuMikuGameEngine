
// MikuMikuGameEngine.h : MikuMikuGameEngine アプリケーションのメイン ヘッダー ファイル
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"       // メイン シンボル

class CMikuMikuGameEngineDoc;

// CMikuMikuGameEngineApp:
// このクラスの実装については、MikuMikuGameEngine.cpp を参照してください。
//

class CMikuMikuGameEngineApp : public CWinAppEx
{
public:
	CMikuMikuGameEngineApp();


// オーバーライド
public:
	virtual BOOL InitInstance();

	CMikuMikuGameEngineDoc* GetDocument() const;

// 実装
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
};

extern CMikuMikuGameEngineApp theApp;
