
#pragma once

#include "MikuMikuGameEngineDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CViewTree �E�B���h�E

class CViewTree : public CTreeCtrl
{
// �R���X�g���N�V����
public:
	CViewTree();

// �I�[�o�[���C�h
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// ����
public:
	virtual ~CViewTree();

protected:
	DECLARE_MESSAGE_MAP()
};
