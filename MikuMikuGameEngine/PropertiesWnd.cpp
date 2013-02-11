
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
// CResourceViewBar メッセージ ハンドラ

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

	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("ゲームオブジェクト"));

	CMFCPropertyGridProperty* pProp;
	{
		CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("名前"), m_selectedObject->GetName().c_str() );
		pGroup1->AddSubItem(pProp);
	}

	{
		CMFCPropertyGridProperty* pPositionProperty = new CMFCPropertyGridProperty(_T("ポジション"), 0, TRUE);

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

		CMFCPropertyGridProperty* pRotationProperty = new CMFCPropertyGridProperty(_T("回転"), 0, TRUE);

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
		TRACE0("プロパティ グリッドを作成できませんでした\n");
		return -1;      // 作成できない場合
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
		// クリックされた項目の選択:
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
	// TODO: ここにコマンド ハンドラ コードを追加します
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: ここにコマンド ハンドラ コードを追加します
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties(FALSE);

	//CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("表示"));

	//pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D 表示"), (_variant_t) false, _T("ウィンドウのフォントが太字以外になり、また、コントロールが 3D ボーダーで描画されます")));

	//CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("罫線"), _T("ダイアログ枠"), _T("次のうちのどれかです : なし、細枠、サイズ変更可能枠、ダイアログ枠"));
	//pProp->AddOption(_T("なし"));
	//pProp->AddOption(_T("細枠"));
	//pProp->AddOption(_T("サイズ変更可能枠"));
	//pProp->AddOption(_T("ダイアログ枠"));
	//pProp->AllowEdit(FALSE);

	//pGroup1->AddSubItem(pProp);
	//pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("キャプション"), (_variant_t) _T("バージョン情報"), _T("ウィンドウのタイトル バーに表示されるテキストを指定します")));

	//m_wndPropList.AddProperty(pGroup1);

	//CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("ウィンドウ サイズ"), 0, TRUE);

	//pProp = new CMFCPropertyGridProperty(_T("高さ"), (_variant_t) 250l, _T("ウィンドウの高さを指定します"));
	//pProp->EnableSpinControl(TRUE, 50, 300);
	//pSize->AddSubItem(pProp);

	//pProp = new CMFCPropertyGridProperty( _T("幅"), (_variant_t) 150l, _T("ウィンドウの幅を指定します"));
	//pProp->EnableSpinControl(TRUE, 50, 200);
	//pSize->AddSubItem(pProp);

	//m_wndPropList.AddProperty(pSize);

	//CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("フォント"));

	//LOGFONT lf;
	//CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	//font->GetLogFont(&lf);

	//lstrcpy(lf.lfFaceName, _T("ＭＳ Ｐゴシック"));

	//pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("フォント"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("ウィンドウの既定フォントを指定します")));
	//pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("システム フォントを使用する"), (_variant_t) true, _T("ウィンドウで MS Shell Dlg フォントを使用するように指定します")));

	//m_wndPropList.AddProperty(pGroup2);

	//CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("その他"));
	//pProp = new CMFCPropertyGridProperty(_T("(名前)"), _T("アプリケーション"));
	//pProp->Enable(FALSE);
	//pGroup3->AddSubItem(pProp);

	//CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("ウィンドウの色"), RGB(210, 192, 254), NULL, _T("ウィンドウの既定の色を指定します"));
	//pColorProp->EnableOtherButton(_T("その他..."));
	//pColorProp->EnableAutomaticButton(_T("既定値"), ::GetSysColor(COLOR_3DFACE));
	//pGroup3->AddSubItem(pColorProp);

	//static TCHAR BASED_CODE szFilter[] = _T("アイコン ファイル (*.ico)|*.ico|すべてのファイル (*.*)|*.*||");
	//pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("アイコン"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("ウィンドウ アイコンを指定します")));

	//pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("フォルダ"), _T("c:\\")));

	//m_wndPropList.AddProperty(pGroup3);

	//CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("階層"));

	//CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("1 番目のサブレベル"));
	//pGroup4->AddSubItem(pGroup41);

	//CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("2 番目のサブレベル"));
	//pGroup41->AddSubItem(pGroup411);

	//pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("項目 1"), (_variant_t) _T("値 1"), _T("これは説明です")));
	//pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("項目 2"), (_variant_t) _T("値 2"), _T("これは説明です")));
	//pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("項目 3"), (_variant_t) _T("値 3"), _T("これは説明です")));

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