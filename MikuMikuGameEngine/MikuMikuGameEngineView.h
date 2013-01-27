
// MikuMikuGameEngineView.h : CMikuMikuGameEngineView クラスのインターフェイス
//


#pragma once

#include "engine/core/graphics/Shader.h"
#include "engine/core/graphics/RenderTexture.h"
#include "engine/GameObject.h"

#include "engine/CoordinateAxis.h"

class CMikuMikuGameEngineView : public CView
{
protected: // シリアル化からのみ作成します。
	CMikuMikuGameEngineView();
	DECLARE_DYNCREATE(CMikuMikuGameEngineView)

// 属性
public:
	CMikuMikuGameEngineDoc* GetDocument() const;

// 操作
public:
	void OnIdle();

private:
	DWORD m_nowTime;

	D3DXVECTOR3 m_cameraPosition;
	D3DXQUATERNION m_cameraRotation;
	
	float m_cameraRadius;

	POINT m_ptCursorDown;
	int m_cameraState;

	CoordinateAxis* m_coordinateAxis;

	RenderTexturePtr m_shadowMap;

	TexturePtr m_controlUITexture;

// オーバーライド
public:
	virtual void OnDraw(CDC* pDC);  // このビューを描画するためにオーバーライドされます。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 実装
public:
	virtual ~CMikuMikuGameEngineView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	bool m_initialized;

// 生成された、メッセージ割り当て関数
protected:
	virtual void OnInitialUpdate();
//	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
};

#ifndef _DEBUG  // MikuMikuGameEngineView.cpp のデバッグ バージョン
inline CMikuMikuGameEngineDoc* CMikuMikuGameEngineView::GetDocument() const
   { return reinterpret_cast<CMikuMikuGameEngineDoc*>(m_pDocument); }
#endif

