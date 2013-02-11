
#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "MikuMikuGameEngine.h"

#include "engine/core/math/Quaternion.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
	: m_selectedObject(NULL)
{
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar ���b�Z�[�W �n���h��

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CPropertiesWnd::SelectGameObject( GameObject* object )
{
	if( m_selectedObject == object )
	{
		return;
	}

	m_selectedObject = object;

	m_wndPropList.RemoveAll();

	if( m_selectedObject == NULL )
	{
		m_wndPropList.RedrawWindow();
		return;
	}

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("�Q�[���I�u�W�F�N�g"));

	CMFCPropertyGridProperty* pProp;
	{
		CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("���O"), m_selectedObject->GetName().c_str() );
		pGroup1->AddSubItem(pProp);
	}

	{
		CMFCPropertyGridProperty* pPositionProperty = new CMFCPropertyGridProperty(_T("�|�W�V����"), 0, TRUE);

		pProp = new CMFCPropertyGridProperty(_T("X"), (_variant_t)m_selectedObject->GetLocalPosition().x, _T("") );
		//pProp->Enable(FALSE);
		pPositionProperty->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty( _T("Y"), (_variant_t)m_selectedObject->GetLocalPosition().y, _T("") );
		//pProp->Enable(FALSE);
		pPositionProperty->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty( _T("Z"), (_variant_t)m_selectedObject->GetLocalPosition().z, _T("") );
		//pProp->Enable(FALSE);
		pPositionProperty->AddSubItem(pProp);

		pGroup1->AddSubItem( pPositionProperty );
	}

	{
		D3DXVECTOR3 eularAngle;
		D3DXQuaternionToEularZXY( &eularAngle,&m_selectedObject->GetLocalRotation() );

		eularAngle = D3DXToDegree( eularAngle );

		CMFCPropertyGridProperty* pRotationProperty = new CMFCPropertyGridProperty(_T("��]"), 0, TRUE);

		pProp = new CMFCPropertyGridProperty(_T("X"), (_variant_t)eularAngle.x, _T("") );
		pRotationProperty->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty( _T("Y"), (_variant_t)eularAngle.y, _T("") );
		pRotationProperty->AddSubItem(pProp);

		pProp = new CMFCPropertyGridProperty( _T("Z"), (_variant_t)eularAngle.z, _T("") );
		pRotationProperty->AddSubItem(pProp);

		pGroup1->AddSubItem( pRotationProperty );
	}

	m_wndPropList.AddProperty( pGroup1 );
}

void CPropertiesWnd::UpdateGameObject()
{
	if( m_selectedObject == NULL )
	{
		return;
	}

	CMFCPropertyGridProperty* pProp;

	CMFCPropertyGridProperty* pGroup1 = m_wndPropList.GetProperty( 0 );

	{
		pProp = pGroup1->GetSubItem( 0 );

		pProp->SetValue( m_selectedObject->GetName().c_str() );
	}

	{
		CMFCPropertyGridProperty* pPositionProperty = pGroup1->GetSubItem( 1 );

		pProp = pPositionProperty->GetSubItem(0);
		pProp->SetValue( (_variant_t)m_selectedObject->GetLocalPosition().x );

		pProp = pPositionProperty->GetSubItem(1);
		pProp->SetValue( (_variant_t)m_selectedObject->GetLocalPosition().y );

		pProp = pPositionProperty->GetSubItem(2);
		pProp->SetValue( (_variant_t)m_selectedObject->GetLocalPosition().z );
	}

	{
		D3DXVECTOR3 eularAngle;
		D3DXQuaternionToEularZXY( &eularAngle,&m_selectedObject->GetLocalRotation() );

		eularAngle = D3DXToDegree( eularAngle );

		CMFCPropertyGridProperty* pRotationProperty = pGroup1->GetSubItem( 2 );

		pProp = pRotationProperty->GetSubItem( 0 );
		pProp->SetValue( (_variant_t)eularAngle.x );

		pProp = pRotationProperty->GetSubItem( 1 );
		pProp->SetValue( (_variant_t)eularAngle.y );

		pProp = pRotationProperty->GetSubItem( 2 );
		pProp->SetValue( (_variant_t)eularAngle.z );
	}
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("�v���p�e�B �O���b�h���쐬�ł��܂���ł���\n");
		return -1;      // �쐬�ł��Ȃ��ꍇ
	}

	InitPropList();

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if( pWnd != &m_wndPropList )
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// �N���b�N���ꂽ���ڂ̑I��:
	}
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* pCmdUI)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: �����ɃR�}���h �n���h�� �R�[�h��ǉ����܂�
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: �����ɃR�}���h�X�V UI �n���h�� �R�[�h��ǉ����܂�
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: �����ɃR�}���h �n���h�� �R�[�h��ǉ����܂�
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: �����ɃR�}���h�X�V UI �n���h�� �R�[�h��ǉ����܂�
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties(FALSE);

	//CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("�\��"));

	//pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D �\��"), (_variant_t) false, _T("�E�B���h�E�̃t�H���g�������ȊO�ɂȂ�A�܂��A�R���g���[���� 3D �{�[�_�[�ŕ`�悳��܂�")));

	//CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("�r��"), _T("�_�C�A���O�g"), _T("���̂����̂ǂꂩ�ł� : �Ȃ��A�טg�A�T�C�Y�ύX�\�g�A�_�C�A���O�g"));
	//pProp->AddOption(_T("�Ȃ�"));
	//pProp->AddOption(_T("�טg"));
	//pProp->AddOption(_T("�T�C�Y�ύX�\�g"));
	//pProp->AddOption(_T("�_�C�A���O�g"));
	//pProp->AllowEdit(FALSE);

	//pGroup1->AddSubItem(pProp);
	//pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("�L���v�V����"), (_variant_t) _T("�o�[�W�������"), _T("�E�B���h�E�̃^�C�g�� �o�[�ɕ\�������e�L�X�g���w�肵�܂�")));

	//m_wndPropList.AddProperty(pGroup1);

	//CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("�E�B���h�E �T�C�Y"), 0, TRUE);

	//pProp = new CMFCPropertyGridProperty(_T("����"), (_variant_t) 250l, _T("�E�B���h�E�̍������w�肵�܂�"));
	//pProp->EnableSpinControl(TRUE, 50, 300);
	//pSize->AddSubItem(pProp);

	//pProp = new CMFCPropertyGridProperty( _T("��"), (_variant_t) 150l, _T("�E�B���h�E�̕����w�肵�܂�"));
	//pProp->EnableSpinControl(TRUE, 50, 200);
	//pSize->AddSubItem(pProp);

	//m_wndPropList.AddProperty(pSize);

	//CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("�t�H���g"));

	//LOGFONT lf;
	//CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	//font->GetLogFont(&lf);

	//lstrcpy(lf.lfFaceName, _T("�l�r �o�S�V�b�N"));

	//pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("�t�H���g"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("�E�B���h�E�̊���t�H���g���w�肵�܂�")));
	//pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("�V�X�e�� �t�H���g���g�p����"), (_variant_t) true, _T("�E�B���h�E�� MS Shell Dlg �t�H���g���g�p����悤�Ɏw�肵�܂�")));

	//m_wndPropList.AddProperty(pGroup2);

	//CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("���̑�"));
	//pProp = new CMFCPropertyGridProperty(_T("(���O)"), _T("�A�v���P�[�V����"));
	//pProp->Enable(FALSE);
	//pGroup3->AddSubItem(pProp);

	//CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("�E�B���h�E�̐F"), RGB(210, 192, 254), NULL, _T("�E�B���h�E�̊���̐F���w�肵�܂�"));
	//pColorProp->EnableOtherButton(_T("���̑�..."));
	//pColorProp->EnableAutomaticButton(_T("����l"), ::GetSysColor(COLOR_3DFACE));
	//pGroup3->AddSubItem(pColorProp);

	//static TCHAR BASED_CODE szFilter[] = _T("�A�C�R�� �t�@�C�� (*.ico)|*.ico|���ׂẴt�@�C�� (*.*)|*.*||");
	//pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("�A�C�R��"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("�E�B���h�E �A�C�R�����w�肵�܂�")));

	//pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("�t�H���_"), _T("c:\\")));

	//m_wndPropList.AddProperty(pGroup3);

	//CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("�K�w"));

	//CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("1 �Ԗڂ̃T�u���x��"));
	//pGroup4->AddSubItem(pGroup41);

	//CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("2 �Ԗڂ̃T�u���x��"));
	//pGroup41->AddSubItem(pGroup411);

	//pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("���� 1"), (_variant_t) _T("�l 1"), _T("����͐����ł�")));
	//pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("���� 2"), (_variant_t) _T("�l 2"), _T("����͐����ł�")));
	//pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("���� 3"), (_variant_t) _T("�l 3"), _T("����͐����ł�")));

	//pGroup4->Expand(FALSE);
	//m_wndPropList.AddProperty(pGroup4);
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}

LRESULT CPropertiesWnd::OnPropertyChanged(WPARAM wParam,LPARAM lParam)
{
	CMFCPropertyGridProperty* pProp;

	CMFCPropertyGridProperty* pGroup1 = m_wndPropList.GetProperty( 0 );

	{
		pProp = pGroup1->GetSubItem( 0 );

		tstring name = (LPCTSTR)pProp->GetValue().bstrVal;
		if( !name.empty() && m_selectedObject->GetName()!=name )
		{
			theApp.GetDocument()->SetObjectName( m_selectedObject,name );
		}
		else
		{
			pProp->SetValue( (_variant_t)m_selectedObject->GetName().c_str() );
		}
	}

	{
		CMFCPropertyGridProperty* pPositionProperty = pGroup1->GetSubItem( 1 );

		D3DXVECTOR3 localPosition;

		pProp = pPositionProperty->GetSubItem(0);
		localPosition.x = (float)pProp->GetValue().fltVal;

		pProp = pPositionProperty->GetSubItem(1);
		localPosition.y = (float)pProp->GetValue().fltVal;

		pProp = pPositionProperty->GetSubItem(2);
		localPosition.z = (float)pProp->GetValue().fltVal;

		m_selectedObject->SetLocalPosition( localPosition );		
	}

	{
		CMFCPropertyGridProperty* pRotationProperty = pGroup1->GetSubItem( 2 );

		D3DXVECTOR3 eularAngle;

		pProp = pRotationProperty->GetSubItem( 0 );
		eularAngle.x = (float)pProp->GetValue().fltVal;

		pProp = pRotationProperty->GetSubItem( 1 );
		eularAngle.y = (float)pProp->GetValue().fltVal;

		pProp = pRotationProperty->GetSubItem( 2 );
		eularAngle.z = (float)pProp->GetValue().fltVal;

		eularAngle = D3DXToRadian( eularAngle );

		D3DXQUATERNION localRotation;
		D3DXQuaternionRotationYawPitchRoll( &localRotation,eularAngle.y,eularAngle.x,eularAngle.z );

		m_selectedObject->SetLocalRotation( localRotation );
	}

	return 0;
}