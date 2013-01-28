
// MikuMikuGameEngineView.h : CMikuMikuGameEngineView �N���X�̃C���^�[�t�F�C�X
//


#pragma once

#include "engine/core/graphics/Shader.h"
#include "engine/core/graphics/RenderTexture.h"
#include "engine/core/graphics/TextureAtlas.h"
#include "engine/core/graphics/UISpriteRenderer.h"
#include "engine/GameObject.h"

#include "engine/CoordinateAxis.h"

class CMikuMikuGameEngineView : public CView
{
protected: // �V���A��������̂ݍ쐬���܂��B
	CMikuMikuGameEngineView();
	DECLARE_DYNCREATE(CMikuMikuGameEngineView)

// ����
public:
	CMikuMikuGameEngineDoc* GetDocument() const;

// ����
public:
	void OnIdle();

private:
	DWORD m_nowTime;

	D3DXVECTOR3 m_cameraPosition;
	D3DXQUATERNION m_cameraRotation;
	
	float m_cameraRadius;

	POINT m_ptCursorDown;

	enum ControlState
	{
		Idle,
		CameraRotate,
		CameraMove,
		HandleMoveX,
		HandleMoveY,
		HandleMoveZ,
	};
	ControlState m_controlState;

	CoordinateAxis* m_coordinateAxis;

	RenderTexturePtr m_shadowMap;

	TextureAtlasPtr m_controlUITextureAtlas;

	UISpriteRenderer m_handleMoveX;
	bool m_handleMoveXOver;
	UISpriteRenderer m_handleMoveY;
	bool m_handleMoveYOver;
	UISpriteRenderer m_handleMoveZ;
	bool m_handleMoveZOver;

// �I�[�o�[���C�h
public:
	virtual void OnDraw(CDC* pDC);  // ���̃r���[��`�悷�邽�߂ɃI�[�o�[���C�h����܂��B
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// ����
public:
	virtual ~CMikuMikuGameEngineView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	bool m_initialized;

// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
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
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // MikuMikuGameEngineView.cpp �̃f�o�b�O �o�[�W����
inline CMikuMikuGameEngineDoc* CMikuMikuGameEngineView::GetDocument() const
   { return reinterpret_cast<CMikuMikuGameEngineDoc*>(m_pDocument); }
#endif

