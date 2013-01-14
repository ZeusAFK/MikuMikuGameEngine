
// MikuMikuGameEngineView.cpp : CMikuMikuGameEngineView クラスの実装
//

#include "stdafx.h"
#include "MikuMikuGameEngine.h"

#include "MikuMikuGameEngineDoc.h"
#include "MikuMikuGameEngineView.h"

#include "engine/core/graphics/Graphics.h"

#include "engine/core/graphics/XFileLoader.h"
#include "engine/core/graphics/PMDFileLoader.h"
#include "engine/core/graphics/VMDFileLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMikuMikuGameEngineView

IMPLEMENT_DYNCREATE(CMikuMikuGameEngineView, CView)

BEGIN_MESSAGE_MAP(CMikuMikuGameEngineView, CView)
ON_WM_RBUTTONUP()
//	ON_WM_CLOSE()
ON_WM_DESTROY()
ON_WM_SIZE()
//ON_WM_ERASEBKGND()
ON_WM_RBUTTONDOWN()
ON_WM_MOUSEMOVE()
ON_WM_MBUTTONDOWN()
ON_WM_MBUTTONUP()
ON_WM_MOUSEWHEEL()
ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()

// CMikuMikuGameEngineView コンストラクション/デストラクション

CMikuMikuGameEngineView::CMikuMikuGameEngineView()
	: m_initialized(false)
{
	// TODO: 構築コードをここに追加します。

}

CMikuMikuGameEngineView::~CMikuMikuGameEngineView()
{
}

BOOL CMikuMikuGameEngineView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

	return CView::PreCreateWindow(cs);
}

// CMikuMikuGameEngineView 描画

void CMikuMikuGameEngineView::OnDraw(CDC* /*pDC*/)
{
	CMikuMikuGameEngineDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: この場所にネイティブ データ用の描画コードを追加します。
}


// CMikuMikuGameEngineView 診断

#ifdef _DEBUG
void CMikuMikuGameEngineView::AssertValid() const
{
	CView::AssertValid();
}

void CMikuMikuGameEngineView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMikuMikuGameEngineDoc* CMikuMikuGameEngineView::GetDocument() const // デバッグ以外のバージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMikuMikuGameEngineDoc)));
	return (CMikuMikuGameEngineDoc*)m_pDocument;
}
#endif //_DEBUG


// CMikuMikuGameEngineView メッセージ ハンドラ

GameObject* CMikuMikuGameEngineView::CreateGameObjectFromModelFrame( GameObject* parent,ModelFrame* pModelFrame,bool select )
{
	GameObject* gameObject = new GameObject;

	gameObject->SetName( pModelFrame->GetName() );

	if( pModelFrame->GetMeshContainer() )
	{
		ModelRenderer* pModelRenderer = new ModelRenderer;

		pModelRenderer->SetModel( pModelFrame->GetMeshContainer() );

		tstring filePath = _T("project/assets/shader.fx");
		ShaderPtr pShader = ResourceManager::GetInstance().GetResource<Shader>( filePath );
		if( !pShader )
		{
			pShader = ShaderPtr(new Shader);
			if( pShader->CreateFromFile( filePath ) )
			{
				ResourceManager::GetInstance().AddResource( filePath,pShader );
			}
			else
			{
				pShader = ShaderPtr();
			}
		}

		if( pShader )
		{
			DWORD materialNum = pModelRenderer->GetMaterialNum();
			for( DWORD matIdx = 0;matIdx<materialNum;matIdx++ )
			{
				pModelRenderer->SetShader( matIdx,pShader );
			}
		}
		gameObject->SetModelRenderer( pModelRenderer );
	}

	D3DXVECTOR3 position;
	D3DXQUATERNION rotation;
	D3DXVECTOR3 scale;

	D3DXMatrixDecompose( &scale,&rotation,&position,&pModelFrame->GetTransform() );

	gameObject->SetLocalPosition( position );
	gameObject->SetLocalRotation( rotation );
	gameObject->SetLocalScale( scale );

	GetDocument()->AddGameObject( gameObject,parent,select );
	
	ModelFrame* childFrame = pModelFrame->GetChild();
	if( childFrame )
	{
		CreateGameObjectFromModelFrame( gameObject,childFrame,false );
	}

	ModelFrame* nextFrame = pModelFrame->GetSiblingNext();
	if( nextFrame )
	{
		CreateGameObjectFromModelFrame( parent,nextFrame,false );
	}

	return gameObject;
}

void CMikuMikuGameEngineView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	HRESULT hr = Graphics::GetInstance()->Initialize( GetSafeHwnd() );
	if( FAILED(hr) )
	{
		::MessageBox( NULL,_T("DirectXが初期化できませんでした。"),_T("error"),MB_ICONERROR );
	}

	m_initialized = true;

	//{
	//	tstring xFilePath = _T("project\\assets\\stage01\\stage01.x");
	//	//tstring xFilePath = _T("project\\assets\\Sample5.x");]

	//	TCHAR path[MAX_PATH];
	//	_tcscpy_s( path,PathFindFileName( xFilePath.c_str() ) );

	//	PathRemoveExtension( path );

	//	ModelPtr pModel = ResourceManager::GetInstance().GetResource<Model>( xFilePath );
	//	if( !pModel )
	//	{
	//		XFileLoader xFileLoader;
	//		pModel = xFileLoader.Open( xFilePath,10.0f );

	//		ResourceManager::GetInstance().AddResource( xFilePath,pModel );
	//	}

	//	if( pModel )
	//	{
	//		GameObject* gameObject = CreateGameObjectFromModelFrame( GetDocument()->GetRootGameObject(),pModel->GetFrame(),true );
	//	}
	//}

	{
		tstring pmdFilePath = _T("project\\assets\\Model\\初音ミク.pmd");

		TCHAR path[MAX_PATH];
		_tcscpy_s( path,PathFindFileName( pmdFilePath.c_str() ) );

		PathRemoveExtension( path );

		PMDModelPtr pModel = ResourceManager::GetInstance().GetResource<PMDModel>( pmdFilePath );
		if( !pModel )
		{
			PMDFileLoader pmdFileLoader;
			pModel = pmdFileLoader.Open( pmdFilePath );

			ResourceManager::GetInstance().AddResource( pmdFilePath,pModel );
		}

		if( pModel )
		{
			GameObject* gameObject = new GameObject;

			gameObject->SetName( path );

			PMDModelRenderer* pmdModelRenderer = new PMDModelRenderer;
			pmdModelRenderer->SetGameObject( gameObject );
			pmdModelRenderer->SetModel( pModel );

			//tstring filePath = _T("project/assets/shader.fx");
			//ShaderPtr pShader = ResourceManager::GetInstance().GetResource<Shader>( filePath );
			//if( !pShader )
			//{
			//	pShader = ShaderPtr(new Shader);
			//	if( pShader->CreateFromFile( filePath ) )
			//	{
			//		ResourceManager::GetInstance().AddResource( filePath,pShader );
			//	}
			//	else
			//	{
			//		pShader = ShaderPtr();
			//	}
			//}

			//if( pShader )
			//{
			//	DWORD materialNum = pmdModelRenderer->GetMaterialNum();
			//	for( DWORD matIdx = 0;matIdx<materialNum;matIdx++ )
			//	{
			//		pmdModelRenderer->SetShader( matIdx,pShader );
			//	}
			//}
			gameObject->SetPMDModelRenderer( pmdModelRenderer );

			GetDocument()->AddGameObject( gameObject,GetDocument()->GetRootGameObject(),true );

			//tstring vmdFilePath = _T("project\\assets\\love_and_joy.vmd");

			//VMDAnimationClipPtr pAnimationClip = ResourceManager::GetInstance().GetResource<VMDAnimationClip>( vmdFilePath );
			//if( !pAnimationClip )
			//{
			//	VMDFileLoader vmdFileLoader;
			//	pAnimationClip = vmdFileLoader.Open( vmdFilePath );

			//	ResourceManager::GetInstance().AddResource( vmdFilePath,pAnimationClip );
			//}

			//if( pAnimationClip )
			//{
			//	gameObject->SetVMDAnimationClip( pAnimationClip );
			//}
		}
	}

	//{
	//	tstring pmdFilePath = _T("project\\assets\\Model\\つむじ風\\つむじ風.pmd");

	//	TCHAR path[MAX_PATH];
	//	_tcscpy_s( path,PathFindFileName( pmdFilePath.c_str() ) );

	//	PathRemoveExtension( path );

	//	PMDModelPtr pModel = ResourceManager::GetInstance().GetResource<PMDModel>( pmdFilePath );
	//	if( !pModel )
	//	{
	//		PMDFileLoader pmdFileLoader;
	//		pModel = pmdFileLoader.Open( pmdFilePath );

	//		ResourceManager::GetInstance().AddResource( pmdFilePath,pModel );
	//	}

	//	if( pModel )
	//	{
	//		GameObject* gameObject = new GameObject;

	//		gameObject->SetName( path );

	//		PMDModelRenderer* pmdModelRenderer = new PMDModelRenderer;
	//		pmdModelRenderer->SetGameObject( gameObject );
	//		pmdModelRenderer->SetModel( pModel );

	//		//tstring filePath = _T("project/assets/shader.fx");
	//		//ShaderPtr pShader = ResourceManager::GetInstance().GetResource<Shader>( filePath );
	//		//if( !pShader )
	//		//{
	//		//	pShader = ShaderPtr(new Shader);
	//		//	if( pShader->CreateFromFile( filePath ) )
	//		//	{
	//		//		ResourceManager::GetInstance().AddResource( filePath,pShader );
	//		//	}
	//		//	else
	//		//	{
	//		//		pShader = ShaderPtr();
	//		//	}
	//		//}

	//		//if( pShader )
	//		//{
	//		//	DWORD materialNum = pmdModelRenderer->GetMaterialNum();
	//		//	for( DWORD matIdx = 0;matIdx<materialNum;matIdx++ )
	//		//	{
	//		//		pmdModelRenderer->SetShader( matIdx,pShader );
	//		//	}
	//		//}
	//		gameObject->SetPMDModelRenderer( pmdModelRenderer );

	//		gameObject->SetLocalPosition( D3DXVECTOR3(10.0f,0.0f,0.0f) );

	//		GetDocument()->AddGameObject( gameObject,GetDocument()->GetRootGameObject(),true );

	//		tstring vmdFilePath = _T("project\\assets\\つむじ風.vmd");

	//		VMDAnimationClipPtr pAnimationClip = ResourceManager::GetInstance().GetResource<VMDAnimationClip>( vmdFilePath );
	//		if( !pAnimationClip )
	//		{
	//			VMDFileLoader vmdFileLoader;
	//			pAnimationClip = vmdFileLoader.Open( vmdFilePath );

	//			ResourceManager::GetInstance().AddResource( vmdFilePath,pAnimationClip );
	//		}

	//		if( pAnimationClip )
	//		{
	//			gameObject->SetVMDAnimationClip( pAnimationClip );
	//		}
	//	}
	//}

	//{
	//	tstring pmdFilePath = _T("project\\assets\\Model\\ＭＭＤのエンジンをＭＭＤエンジンで\\MMDのエンジンをMMDエンジンで.pmd");

	//	TCHAR path[MAX_PATH];
	//	_tcscpy_s( path,PathFindFileName( pmdFilePath.c_str() ) );

	//	PathRemoveExtension( path );

	//	PMDModelPtr pModel = ResourceManager::GetInstance().GetResource<PMDModel>( pmdFilePath );
	//	if( !pModel )
	//	{
	//		PMDFileLoader pmdFileLoader;
	//		pModel = pmdFileLoader.Open( pmdFilePath );

	//		ResourceManager::GetInstance().AddResource( pmdFilePath,pModel );
	//	}

	//	if( pModel )
	//	{
	//		GameObject* gameObject = new GameObject;

	//		gameObject->SetName( path );

	//		PMDModelRenderer* pmdModelRenderer = new PMDModelRenderer;
	//		pmdModelRenderer->SetGameObject( gameObject );
	//		pmdModelRenderer->SetModel( pModel );

	//		//tstring filePath = _T("project/assets/shader.fx");
	//		//ShaderPtr pShader = ResourceManager::GetInstance().GetResource<Shader>( filePath );
	//		//if( !pShader )
	//		//{
	//		//	pShader = ShaderPtr(new Shader);
	//		//	if( pShader->CreateFromFile( filePath ) )
	//		//	{
	//		//		ResourceManager::GetInstance().AddResource( filePath,pShader );
	//		//	}
	//		//	else
	//		//	{
	//		//		pShader = ShaderPtr();
	//		//	}
	//		//}

	//		//if( pShader )
	//		//{
	//		//	DWORD materialNum = pmdModelRenderer->GetMaterialNum();
	//		//	for( DWORD matIdx = 0;matIdx<materialNum;matIdx++ )
	//		//	{
	//		//		pmdModelRenderer->SetShader( matIdx,pShader );
	//		//	}
	//		//}
	//		gameObject->SetPMDModelRenderer( pmdModelRenderer );

	//		gameObject->SetLocalPosition( D3DXVECTOR3(-10.0f,0.0f,0.0f) );

	//		GetDocument()->AddGameObject( gameObject,GetDocument()->GetRootGameObject(),true );

	//		tstring vmdFilePath = _T("project\\assets\\MMDエンジンのエンジン.vmd");

	//		VMDAnimationClipPtr pAnimationClip = ResourceManager::GetInstance().GetResource<VMDAnimationClip>( vmdFilePath );
	//		if( !pAnimationClip )
	//		{
	//			VMDFileLoader vmdFileLoader;
	//			pAnimationClip = vmdFileLoader.Open( vmdFilePath );

	//			ResourceManager::GetInstance().AddResource( vmdFilePath,pAnimationClip );
	//		}

	//		if( pAnimationClip )
	//		{
	//			gameObject->SetVMDAnimationClip( pAnimationClip );
	//		}
	//	}
	//}

	//{
	//	tstring pmdFilePath = _T("project\\assets\\Model\\MMDエンジン原理可視モデル\\MMDエンジンかしもでる.pmd");

	//	TCHAR path[MAX_PATH];
	//	_tcscpy_s( path,PathFindFileName( pmdFilePath.c_str() ) );

	//	PathRemoveExtension( path );

	//	PMDModelPtr pModel = ResourceManager::GetInstance().GetResource<PMDModel>( pmdFilePath );
	//	if( !pModel )
	//	{
	//		PMDFileLoader pmdFileLoader;
	//		pModel = pmdFileLoader.Open( pmdFilePath );

	//		ResourceManager::GetInstance().AddResource( pmdFilePath,pModel );
	//	}

	//	if( pModel )
	//	{
	//		GameObject* gameObject = new GameObject;

	//		gameObject->SetName( path );

	//		PMDModelRenderer* pmdModelRenderer = new PMDModelRenderer;
	//		pmdModelRenderer->SetGameObject( gameObject );
	//		pmdModelRenderer->SetModel( pModel );

	//		//tstring filePath = _T("project/assets/shader.fx");
	//		//ShaderPtr pShader = ResourceManager::GetInstance().GetResource<Shader>( filePath );
	//		//if( !pShader )
	//		//{
	//		//	pShader = ShaderPtr(new Shader);
	//		//	if( pShader->CreateFromFile( filePath ) )
	//		//	{
	//		//		ResourceManager::GetInstance().AddResource( filePath,pShader );
	//		//	}
	//		//	else
	//		//	{
	//		//		pShader = ShaderPtr();
	//		//	}
	//		//}

	//		//if( pShader )
	//		//{
	//		//	DWORD materialNum = pmdModelRenderer->GetMaterialNum();
	//		//	for( DWORD matIdx = 0;matIdx<materialNum;matIdx++ )
	//		//	{
	//		//		pmdModelRenderer->SetShader( matIdx,pShader );
	//		//	}
	//		//}
	//		gameObject->SetPMDModelRenderer( pmdModelRenderer );

	//		gameObject->SetLocalPosition( D3DXVECTOR3(-20.0f,0.0f,0.0f) );

	//		GetDocument()->AddGameObject( gameObject,GetDocument()->GetRootGameObject(),true );

	//		tstring vmdFilePath = _T("project\\assets\\MMDエンジン原理可視モデルでのテストぐるぐる.vmd");

	//		VMDAnimationClipPtr pAnimationClip = ResourceManager::GetInstance().GetResource<VMDAnimationClip>( vmdFilePath );
	//		if( !pAnimationClip )
	//		{
	//			VMDFileLoader vmdFileLoader;
	//			pAnimationClip = vmdFileLoader.Open( vmdFilePath );

	//			ResourceManager::GetInstance().AddResource( vmdFilePath,pAnimationClip );
	//		}

	//		if( pAnimationClip )
	//		{
	//			gameObject->SetVMDAnimationClip( pAnimationClip );
	//		}
	//	}
	//}

	m_cameraPosition = D3DXVECTOR3( 0.0f,10.0f,0.0f );

	D3DXQuaternionIdentity( &m_cameraRotation );
	m_cameraRadius = 35.0f;
	m_cameraState = 0;

	tstring filePath = _T("project/assets/editor.fx");
	m_editorShader = ResourceManager::GetInstance().GetResource<Shader>( filePath );
	if( !m_editorShader )
	{
		m_editorShader = ShaderPtr(new Shader);
		m_editorShader->CreateFromFile( filePath );

		ResourceManager::GetInstance().AddResource( filePath,m_editorShader );
	}

	m_coordinateAxis = new CoordinateAxis;

	m_nowTime = timeGetTime();
}

void CMikuMikuGameEngineView::OnIdle()
{
	if( !m_initialized )
	{
		return;
	}

	DWORD time=timeGetTime();
	float elapsedTime=(time-m_nowTime)/1000.0f;
	m_nowTime=time;

	GetDocument()->GetRootGameObject()->UpdateAnimation( elapsedTime );

	{
		Graphics* graphics = Graphics::GetInstance();
		//Locker lock( graphics->GetCriticalSection() );

		sRenderInfo renderInfo;

		D3DXMatrixPerspectiveFovLH( &renderInfo.matProj, D3DXToRadian(45.0f), (float)graphics->GetBackBufferWidth()/(float)graphics->GetBackBufferHeight(), 1.0f, 10000.0f);

		renderInfo.lightDir = D3DXVECTOR3(-0.5f,-1.0f,0.5f);
		D3DXVec3Normalize( &renderInfo.lightDir,&renderInfo.lightDir );
		renderInfo.lightColor = D3DXCOLOR( 0xFF9A9A9A );

		renderInfo.eyePos = D3DXVECTOR3(0.0f,0.0f,-1.0f);
		{
			D3DXVECTOR3 vecLookPt(0.0f,0.0f,1.0f);
			D3DXVECTOR3 vecUpVec(0.0f,1.0f,0.0f);

			D3DXMATRIX matCameraRotate;

			D3DXMatrixRotationQuaternion( &matCameraRotate,&m_cameraRotation );

			D3DXVec3TransformNormal( &vecLookPt,&vecLookPt,&matCameraRotate );
			D3DXVec3TransformNormal( &renderInfo.eyePos,&renderInfo.eyePos,&matCameraRotate );
			D3DXVec3TransformNormal( &vecUpVec,&vecUpVec,&matCameraRotate );

			D3DXVec3Scale( &renderInfo.eyePos,&renderInfo.eyePos,m_cameraRadius );
			D3DXVec3Add( &renderInfo.eyePos,&m_cameraPosition,&renderInfo.eyePos );

			D3DXVec3Add( &vecLookPt,&renderInfo.eyePos,&vecLookPt );

			D3DXMatrixLookAtLH( &renderInfo.matView, &renderInfo.eyePos, &vecLookPt, &vecUpVec );
		}

		D3DXMatrixOrthoLH( &renderInfo.matLightProj, (float)graphics->GetBackBufferWidth(),(float)graphics->GetBackBufferHeight(), 1.0f, 10000.0f);
		
		D3DXVECTOR3 lightPos = renderInfo.lightDir * (-1000.0f);
		{
			D3DXVECTOR3 vecLookPt(0.0f,0.0f,0.0f);
			D3DXVECTOR3 vecUpVec(0.0f,1.0f,0.0f);

			D3DXMatrixLookAtLH( &renderInfo.matLightView, &renderInfo.eyePos, &vecLookPt, &vecUpVec );
		}

		HRESULT hr = graphics->BeginScene();
		if( SUCCEEDED(hr) )
		{
			// TODO: ここにコードを挿入してください。
			// 描画開始

			graphics->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 255, 255, 255 ), 1.0f, 0 );

			{
				m_coordinateAxis->Render( renderInfo.matView,renderInfo.matProj );
			}

			graphics->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			graphics->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			graphics->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			graphics->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE,TRUE );
			graphics->SetRenderState( D3DRS_SRCBLENDALPHA,D3DBLEND_ONE );
			graphics->SetRenderState( D3DRS_DESTBLENDALPHA,D3DBLEND_ONE );
			graphics->SetRenderState( D3DRS_BLENDOPALPHA,D3DBLENDOP_ADD );

			GetDocument()->GetRootGameObject()->Render( renderInfo );

			graphics->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE,FALSE );
			graphics->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

			//GameObject* selectObject = GetDocument()->GetSelectGameObject();
			//if( selectObject )
			//{
			//	D3DXMATRIX matWorldViewProj = selectObject->GetWorldMatrix() * matView * matProj;

			//	ID3DXEffectPtr pEffect = m_editorShader->GetEffect();

			//	pEffect->SetTechnique( "Tec_Default" );
			//	UINT numPass;
			//	pEffect->Begin( &numPass, 0 );

			//	pEffect->SetMatrix( "matWorldViewProj", &matWorldViewProj );

			//	D3DXCOLOR color(0xFFB4B4B4);
			//	pEffect->SetValue( "colorDiffuse",&color,sizeof(D3DXCOLOR) );

			//	graphics->SetRenderState( D3DRS_FILLMODE,D3DFILL_WIREFRAME );

			//	pEffect->BeginPass(0);

			//	selectObject->RenderNonShader();

			//	pEffect->EndPass();

			//	graphics->SetRenderState( D3DRS_FILLMODE,D3DFILL_SOLID );

			//	graphics->SetRenderState( D3DRS_ZENABLE,D3DZB_FALSE );

			//	struct sVertex
			//	{
			//		D3DXVECTOR3 position;
			//		D3DCOLOR color;
			//	};

			//	sVertex vertices[] =
			//	{
			//		{ D3DXVECTOR3( 0.0f, 0.0f, 0.0f),0xFFFF0000 },
			//		{ D3DXVECTOR3(10.0f, 0.0f, 0.0f),0xFFFF0000 },
			//		{ D3DXVECTOR3( 0.0f, 0.0f, 0.0f),0xFF00FF00 },
			//		{ D3DXVECTOR3( 0.0f,10.0f, 0.0f),0xFF00FF00 },
			//		{ D3DXVECTOR3( 0.0f, 0.0f, 0.0f),0xFF0000FF },
			//		{ D3DXVECTOR3( 0.0f, 0.0f,10.0f),0xFF0000FF },
			//	};

			//	graphics->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );

			//	color=0xFFFFFFFF;
			//	pEffect->SetValue( "colorDiffuse",&color,sizeof(D3DXCOLOR) );

			//	pEffect->BeginPass(0);

			//	graphics->DrawPrimitiveUP( D3DPT_LINELIST,3,&vertices[0],sizeof(sVertex) );

			//	pEffect->EndPass();

			//	graphics->SetRenderState( D3DRS_ZENABLE,D3DZB_TRUE );

			//	pEffect->End();
			//}

			graphics->EndScene();
		}
	}

}

//void CMikuMikuGameEngineView::OnClose()
//{
//	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
//
//	Graphics::GetInstance()->Cleanup();
//
//	CView::OnClose();
//}

void CMikuMikuGameEngineView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: ここにメッセージ ハンドラ コードを追加します。

	delete m_coordinateAxis;

	m_editorShader.reset();

	Graphics::GetInstance()->Cleanup();
}

BOOL CMikuMikuGameEngineView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。

	switch( pMsg->message )
	{
	case WM_USER_RESETDEVICE:
		{
			Graphics::GetInstance()->ResetDevice();
		}
		return TRUE;
	case WM_USER_LOSTDEVICE:
		{
			Graphics::GetInstance()->LostDevice();
		}
		return TRUE;
	case WM_SIZING:
		{
			::OutputDebugString( _T("WM_SIZING\n") );

			RECT rc = *(RECT*)pMsg->lParam;

			RECT clientRect;
			::GetClientRect( pMsg->hwnd,&clientRect );

			POINT ptLeftTop={clientRect.left,clientRect.top};
			::ClientToScreen( pMsg->hwnd,&ptLeftTop );

			POINT ptRightBottom={clientRect.right,clientRect.bottom};
			::ClientToScreen( pMsg->hwnd,&ptRightBottom );

			clientRect.left = ptLeftTop.x;
			clientRect.top = ptLeftTop.y;
			clientRect.right = ptRightBottom.x;
			clientRect.bottom = ptRightBottom.y;

			RECT windowRect;
			::GetWindowRect( pMsg->hwnd,&windowRect );

			rc.left += clientRect.left-windowRect.left;
			rc.top += clientRect.top-windowRect.top;
			rc.right -= windowRect.right-clientRect.right;
			rc.bottom -= windowRect.bottom-clientRect.bottom;

			SIZE size;

			size.cx = rc.right-rc.left;
			size.cy = rc.bottom-rc.top;

			Locker lock( Graphics::GetInstance()->GetCriticalSection() );

			Graphics::GetInstance()->EnableRender(false);

			// TODO:デバイスロスト発生処理

			Graphics::GetInstance()->Resize( size.cx, size.cy );

			// TODO:デバイス復帰処理

			Graphics::GetInstance()->EnableRender(true);
		}
		break;
	}

	return CView::PreTranslateMessage(pMsg);
}

void CMikuMikuGameEngineView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: ここにメッセージ ハンドラ コードを追加します。

	if( m_initialized )
	{
		Locker lock( Graphics::GetInstance()->GetCriticalSection() );

		Graphics::GetInstance()->EnableRender(false);

		// TODO:デバイスロスト発生処理

		Graphics::GetInstance()->Resize( cx, cy );

		// TODO:デバイス復帰処理

		Graphics::GetInstance()->EnableRender(true);
	}
}

//BOOL CMikuMikuGameEngineView::OnEraseBkgnd(CDC* pDC)
//{
//	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
//
//	return TRUE;
//	//return CView::OnEraseBkgnd(pDC);
//}

void CMikuMikuGameEngineView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

	if( m_cameraState == 0 )
	{
		POINT pt;
		pt.x = point.x;
		pt.y = point.y;

		m_ptCursorDown = pt;
		m_cameraState = 1;

		SetCapture();
	}

	CView::OnRButtonDown(nFlags, point);
}

void CMikuMikuGameEngineView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

	if( m_cameraState == 1 )
	{
		POINT pt;
		pt.x = point.x;
		pt.y = point.y;

		m_cameraState = 0;

		ReleaseCapture();
	}

	CView::OnRButtonUp(nFlags, point);
}

void CMikuMikuGameEngineView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

	POINT pt;
	pt.x = point.x;
	pt.y = point.y;

	int x = pt.x-m_ptCursorDown.x;
	int y = pt.y-m_ptCursorDown.y;

	switch( m_cameraState )
	{
	case 1:
		{
			m_ptCursorDown = pt;

			if( x!=0 )
			{
				D3DXQUATERNION q;
				D3DXQuaternionRotationAxis( &q,&D3DXVECTOR3(0.0f,1.0f,0.0f),D3DXToRadian( x*0.573f ) );

				m_cameraRotation = m_cameraRotation * q;
			}

			if( y!=0 )
			{
				D3DXQUATERNION q;
				D3DXQuaternionRotationAxis( &q,&D3DXVECTOR3(1.0f,0.0f,0.0f),D3DXToRadian( y*0.573f ) );

				m_cameraRotation = q * m_cameraRotation;
			}
		}
		break;
	case 2:
		{
			m_ptCursorDown = pt;

			D3DXVECTOR3 move((float)-x,(float)y,0.0f);
			move *= 0.05f;

			D3DXMATRIX matCameraRotate;
			D3DXMatrixRotationQuaternion( &matCameraRotate,&m_cameraRotation );

			D3DXVec3TransformNormal( &move,&move,&matCameraRotate );

			m_cameraPosition += move;
		}
		break;
	}
	CView::OnMouseMove(nFlags, point);
}

void CMikuMikuGameEngineView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

	if( m_cameraState == 0 )
	{
		POINT pt;
		pt.x = point.x;
		pt.y = point.y;

		m_ptCursorDown = pt;
		m_cameraState = 2;

		SetCapture();
	}

	CView::OnMButtonDown(nFlags, point);
}

void CMikuMikuGameEngineView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

	if( m_cameraState == 2 )
	{
		POINT pt;
		pt.x = point.x;
		pt.y = point.y;

		m_cameraState = 0;

		ReleaseCapture();
	}

	CView::OnMButtonUp(nFlags, point);
}

BOOL CMikuMikuGameEngineView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。

	m_cameraRadius -= ((float)zDelta/120.0f)*6.0f;
	
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CMikuMikuGameEngineView::OnCaptureChanged(CWnd *pWnd)
{
	// TODO: ここにメッセージ ハンドラ コードを追加します。

	if( m_cameraState != 0 )
	{
		m_cameraState = 0;
	}

	CView::OnCaptureChanged(pWnd);
}
