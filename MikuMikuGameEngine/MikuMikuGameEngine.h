
// MikuMikuGameEngine.h : MikuMikuGameEngine �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C��
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"       // ���C�� �V���{��

class CMikuMikuGameEngineDoc;

// CMikuMikuGameEngineApp:
// ���̃N���X�̎����ɂ��ẮAMikuMikuGameEngine.cpp ���Q�Ƃ��Ă��������B
//

class CMikuMikuGameEngineApp : public CWinAppEx
{
public:
	CMikuMikuGameEngineApp();


// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();

	CMikuMikuGameEngineDoc* GetDocument() const;

// ����
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
