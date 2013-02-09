
// MikuMikuGameEngineView.cpp : CMikuMikuGameEngineView �N���X�̎���
//

#include "stdafx.h"
#include "MikuMikuGameEngine.h"

#include "MikuMikuGameEngineDoc.h"
#include "MikuMikuGameEngineView.h"

#include "engine/core/graphics/Graphics.h"

#include "engine/core/graphics/XFileLoader.h"
#include "engine/core/graphics/PMDFileLoader.h"
#include "engine/core/graphics/VMDFileLoader.h"

#include "engine/core/util/util.h"

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
ON_WM_LBUTTONUP()
ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CMikuMikuGameEngineView �R���X�g���N�V����/�f�X�g���N�V����

CMikuMikuGameEngineView::CMikuMikuGameEngineView()
	: m_initialized(false)
{
	// TODO: �\�z�R�[�h�������ɒǉ����܂��B

}

CMikuMikuGameEngineView::~CMikuMikuGameEngineView()
{
}

BOOL CMikuMikuGameEngineView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ���̈ʒu�� CREATESTRUCT cs ���C������ Window �N���X�܂��̓X�^�C����
	//  �C�����Ă��������B

	return CView::PreCreateWindow(cs);
}

// CMikuMikuGameEngineView �`��

void CMikuMikuGameEngineView::OnDraw(CDC* /*pDC*/)
{
	CMikuMikuGameEngineDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���̏ꏊ�Ƀl�C�e�B�u �f�[�^�p�̕`��R�[�h��ǉ����܂��B
}


// CMikuMikuGameEngineView �f�f

#ifdef _DEBUG
void CMikuMikuGameEngineView::AssertValid() const
{
	CView::AssertValid();
}

void CMikuMikuGameEngineView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMikuMikuGameEngineDoc* CMikuMikuGameEngineView::GetDocument() const // �f�o�b�O�ȊO�̃o�[�W�����̓C�����C���ł��B
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMikuMikuGameEngineDoc)));
	return (CMikuMikuGameEngineDoc*)m_pDocument;
}
#endif //_DEBUG


// CMikuMikuGameEngineView ���b�Z�[�W �n���h��

void CMikuMikuGameEngineView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B
	HRESULT hr = Graphics::GetInstance()->Initialize( GetSafeHwnd() );
	if( FAILED(hr) )
	{
		::MessageBox( NULL,_T("DirectX���������ł��܂���ł����B"),_T("error"),MB_ICONERROR );
	}

	m_initialized = true;

	m_shadowMap=RenderTexturePtr( new RenderTexture );
	m_shadowMap->Create( 2048,2048,D3DFMT_R32F );

	TexturePtr controlUITexture = TexturePtr( new Texture );
	if( controlUITexture->CreateFromResource( IDR_TEXTURE_CONTROL_UI ) )
	{
		m_controlUITextureAtlas = TextureAtlasPtr( new TextureAtlas );
		if( !m_controlUITextureAtlas->CreateFromResource( IDR_ATLAS_CONTROL_UI,controlUITexture ) )
		{
			m_controlUITextureAtlas.reset();
		}
	}

	m_handleMoveX.SetAtlas( m_controlUITextureAtlas );
	m_handleMoveX.SetSpriteName( tstring(_T("HandleMoveXNormal")) );
	m_handleMoveXOver = false;

	m_handleMoveY.SetAtlas( m_controlUITextureAtlas );
	m_handleMoveY.SetSpriteName( tstring(_T("HandleMoveYNormal")) );
	m_handleMoveYOver = false;

	m_handleMoveZ.SetAtlas( m_controlUITextureAtlas );
	m_handleMoveZ.SetSpriteName( tstring(_T("HandleMoveZNormal")) );
	m_handleMoveZOver = false;

	m_handleRotateX.SetAtlas( m_controlUITextureAtlas );
	m_handleRotateX.SetSpriteName( tstring(_T("HandleRotateXNormal")) );
	m_handleRotateXOver = false;

	m_handleRotateY.SetAtlas( m_controlUITextureAtlas );
	m_handleRotateY.SetSpriteName( tstring(_T("HandleRotateYNormal")) );
	m_handleRotateYOver = false;

	m_handleRotateZ.SetAtlas( m_controlUITextureAtlas );
	m_handleRotateZ.SetSpriteName( tstring(_T("HandleRotateZNormal")) );
	m_handleRotateZOver = false;

	//{
	//	//tstring pmdFilePath = _T("project\\assets\\Model\\Lat���~�NVer2.3\\Lat���~�NVer2.3_Normal.pmd");
	//	tstring pmdFilePath = _T("project\\assets\\Model\\�����~�N.pmd");

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

	//		gameObject->SetPMDModelRenderer( pmdModelRenderer );

	//		GetDocument()->AddGameObject( gameObject,GetDocument()->GetRootGameObject(),true );

	//		//tstring vmdFilePath = _T("project\\assets\\love_and_joy.vmd");

	//		//VMDAnimationClipPtr pAnimationClip = ResourceManager::GetInstance().GetResource<VMDAnimationClip>( vmdFilePath );
	//		//if( !pAnimationClip )
	//		//{
	//		//	VMDFileLoader vmdFileLoader;
	//		//	pAnimationClip = vmdFileLoader.Open( vmdFilePath );

	//		//	ResourceManager::GetInstance().AddResource( vmdFilePath,pAnimationClip );
	//		//}

	//		//if( pAnimationClip )
	//		//{
	//		//	gameObject->SetVMDAnimationClip( pAnimationClip );
	//		//}
	//	}
	//}

	//{
	//	tstring pmdFilePath = _T("project\\assets\\ShadowTest.pmd");

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

	//		gameObject->SetPMDModelRenderer( pmdModelRenderer );

	//		GetDocument()->AddGameObject( gameObject,GetDocument()->GetRootGameObject(),true );
	//	}
	//}

	//{
	//	tstring pmdFilePath = _T("project\\assets\\Model\\�ނ���\\�ނ���.pmd");

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

	//		gameObject->SetPMDModelRenderer( pmdModelRenderer );

	//		gameObject->SetLocalPosition( D3DXVECTOR3(10.0f,0.0f,0.0f) );

	//		GetDocument()->AddGameObject( gameObject,GetDocument()->GetRootGameObject(),true );

	//		tstring vmdFilePath = _T("project\\assets\\�ނ���.vmd");

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
	//	tstring pmdFilePath = _T("project\\assets\\Model\\�l�l�c�̃G���W�����l�l�c�G���W����\\MMD�̃G���W����MMD�G���W����.pmd");

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

	//		gameObject->SetPMDModelRenderer( pmdModelRenderer );

	//		gameObject->SetLocalPosition( D3DXVECTOR3(-10.0f,0.0f,0.0f) );

	//		GetDocument()->AddGameObject( gameObject,GetDocument()->GetRootGameObject(),true );

	//		tstring vmdFilePath = _T("project\\assets\\MMD�G���W���̃G���W��.vmd");

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
	//	tstring pmdFilePath = _T("project\\assets\\Model\\MMD�G���W�����������f��\\MMD�G���W���������ł�.pmd");

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

	//		gameObject->SetPMDModelRenderer( pmdModelRenderer );

	//		gameObject->SetLocalPosition( D3DXVECTOR3(-20.0f,0.0f,0.0f) );

	//		GetDocument()->AddGameObject( gameObject,GetDocument()->GetRootGameObject(),true );

	//		tstring vmdFilePath = _T("project\\assets\\MMD�G���W�����������f���ł̃e�X�g���邮��.vmd");

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
	m_controlState = Idle;

	m_coordinateAxis = new CoordinateAxis;

	m_nowTime = timeGetTime();
}

#include "engine/core/math/BoundingFrustum.h"
#include "engine/core/math/BoundingBox.h"
#include "engine/core/math/BoundingCone.h"
#include "engine/core/math/Ray.h"

//D3DXMATRIX CalcShadowMatrix( sRenderInfo& renderInfo )
//{
//	//�\���s��ݒ�
//	D3DXVECTOR3 lightDir = renderInfo.lightDir;
//	D3DXVECTOR3 viewDir = renderInfo.camera.GetLookDir();
//
//	const D3DXMATRIX& matView = renderInfo.camera.GetViewMatrix();
//
//	const D3DXMATRIX& matProj = renderInfo.camera.GetViewMatrix();
//
//	D3DXVECTOR3 vUp;
//	D3DXVec3Cross( &vUp,&viewDir,&(-lightDir) );
//	if( D3DXVec3LengthSq( &vUp ) < FLT_EPSILON )
//	{
//		// 
//		// �ʏ�̃V���h�E�}�b�v
//		// MMD���Ɖe��������H
//
//		return matView * matProj;
//	}
//
//	float d = (1.0f-D3DXVec3Dot( &viewDir,&lightDir ))*0.5f;
//
//	D3DXVec3Normalize( &vUp,&vUp );
//	D3DXVec3Cross( &vUp,&vUp,&viewDir );
//	D3DXVec3Normalize( &vUp,&vUp );
//
//	float shadowDistance = 8875*0.01f;
//
//	// �V���h�E�����̈ʒu���t�@�[�N���b�v��

//	float nearClip = 10.0f + shadowDistance;
//
//	const D3DXVECTOR3& camPos = renderInfo.camera.GetPosition() + viewDir * d * 50.0f;
//	D3DXVECTOR3 lightPos = camPos - lightDir * nearClip;
//
//	D3DXMATRIX matLightView;
//	D3DXMatrixLookAtLH( &matLightView,&lightPos,&(lightPos + lightDir),&vUp );
//
//	D3DXMATRIX matLightProj;
//	D3DXMatrixPerspectiveFovLH( &matLightProj,D3DXToRadian(10.0f),1.0f,nearClip,nearClip+1500.0f );
//
//	return matLightView * matLightProj;
//}

D3DXMATRIX CalcShadowMatrix( sRenderInfo& renderInfo )
{
	//�\���s��ݒ�
	D3DXVECTOR3 lightDir = renderInfo.lightDir;

	const D3DXMATRIX& matView = renderInfo.camera.GetViewMatrix();
	const D3DXMATRIX& matProj = renderInfo.camera.GetViewMatrix();

	//���̕����ɉ�]�����]�s��𐶐�
	D3DXMATRIX lightRotation;
	D3DXMatrixLookAtLH( &lightRotation,&D3DXVECTOR3(0.0f,0.0f,0.0f),&lightDir,&D3DXVECTOR3(0.0f,1.0f,0.0f) );
	//�����䂩�琍��̃R�[�i�[���擾

	D3DXVECTOR3 frustumCorners[ BoundingFrustum::CornerCount ];
	renderInfo.camera.GetFrustumCorners( 100.0f, frustumCorners);
	//���̕����ɉ�]
	for(int index = 0; index < BoundingFrustum::CornerCount; index++ )
	{
		D3DXVec3TransformCoord( &frustumCorners[index],&frustumCorners[index], &lightRotation );
	}

	//������܂ލŏ��̔��^��Ԃ𐶐�
	BoundingBox lightBox = BoundingBox::CreateFromPoints(frustumCorners,BoundingFrustum::CornerCount);
	//���^��Ԃ̃T�C�Y�擾
	D3DXVECTOR3 boxSize = lightBox.GetMax() - lightBox.GetMin();
	D3DXVECTOR3 halfBoxSize = boxSize * 0.5f;

	//���C�g�̈ʒu�v�Z
	//���s�����ɂ����郉�C�g�̈ʒu�͔��^��Ԃ̃o�b�N�p�l�������B
	D3DXVECTOR3 lightPosition = lightBox.GetMin() + halfBoxSize;
	lightPosition.z = lightBox.GetMin().z;

	//���C�g�̈ʒu�����̍��W�n�ɖ߂�
	D3DXMatrixInverse(&lightRotation,NULL,&lightRotation);
	D3DXVec3TransformCoord( &lightPosition,&lightPosition, &lightRotation );

	D3DXMATRIX matLightView;
	D3DXMATRIX matLightProj;

	D3DXVECTOR3 vLook = renderInfo.camera.GetLookDir();
	D3DXVECTOR3 vUp;
	D3DXVec3Cross( &vUp,&vLook,&(-lightDir) );
	D3DXVec3Normalize( &vUp,&vUp );
	D3DXVec3Cross( &vUp,&vUp,&vLook );
	D3DXVec3Normalize( &vUp,&vUp );

	//���C�g�̈ʒu�����ɂ����r���[�}�g���b�N�X����
	//���C�g�̃v���W�F�N�V�����}�g���b�N�X���g��
	//�p�[�X�y�N�e�B�u�V���h�E�}�b�v�̏ꍇ�͎�����̃v���W�F�N�V�������g���B����ɂ��p�[�X�y�N�e�B�u�V���h�E�}�b�v�ɂȂ�
	
	D3DXMatrixLookAtLH( &matLightView,&lightPosition,&(lightPosition + lightDir),&vUp );
	//D3DXMatrixPerspectiveFovLH( &matLightProj,D3DXToRadian( 45.0f ) , 1.0f , 1.0f, 1000.0f );
	D3DXMatrixOrthoLH( &matLightProj,boxSize.x, boxSize.y, -boxSize.z, boxSize.z );

	return matLightView * matLightProj;
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

	GameObject* rootGameObject = GetDocument()->GetRootGameObject();

	rootGameObject->UpdateScript( elapsedTime );

	rootGameObject->UpdateAnimation( elapsedTime );

	{
		Graphics* graphics = Graphics::GetInstance();
		IDirect3DDevice9Ptr pD3DDevice = graphics->GetDirect3DDevice();

		//Locker lock( graphics->GetCriticalSection() );

		sRenderInfo renderInfo;

		renderInfo.lightDir = D3DXVECTOR3(-0.5f,-1.0f,0.5f);
		D3DXVec3Normalize( &renderInfo.lightDir,&renderInfo.lightDir );
		renderInfo.lightColor = D3DXCOLOR( 0xFF9A9A9A );

		float nearClip = 1.0f;
		float farClip = 1000000.0f;

		renderInfo.camera.UpdateView( m_cameraPosition,m_cameraRotation,m_cameraRadius );
		renderInfo.camera.UpdateProj( D3DXToRadian(45.0f), (float)graphics->GetBackBufferWidth()/(float)graphics->GetBackBufferHeight(), nearClip, farClip);
		
		renderInfo.shadowMap = true;

		if( renderInfo.shadowMap )
		{
			renderInfo.shadowMapTexture = m_shadowMap;

			float shadowDistance = 12000.0f;

			renderInfo.matShadow = CalcShadowMatrix( renderInfo );
		}

		HRESULT hr = graphics->BeginScene();
		if( SUCCEEDED(hr) )
		{
			// TODO: �����ɃR�[�h��}�����Ă��������B
			// �`��J�n

			GetDocument()->GetRootGameObject()->UpdateMesh( renderInfo );

			if( renderInfo.shadowMap )
			{
				IDirect3DSurface9Ptr pOldBackBuffer;
				IDirect3DSurface9Ptr pOldDepthBuffer;
				D3DVIEWPORT9 oldViewport;
				pD3DDevice->GetRenderTarget( 0,&pOldBackBuffer );
				pD3DDevice->GetDepthStencilSurface( &pOldDepthBuffer );
				pD3DDevice->GetViewport( &oldViewport );

				m_shadowMap->SetRenderTarget();

				graphics->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0 );

				rootGameObject->RenderZPlot( renderInfo );

				pD3DDevice->SetRenderTarget( 0,pOldBackBuffer );
				pD3DDevice->SetDepthStencilSurface( pOldDepthBuffer );
				pD3DDevice->SetViewport( &oldViewport );				
			}

			graphics->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 255, 255, 255 ), 1.0f, 0 );

			{
				m_coordinateAxis->Render( renderInfo.camera.GetViewMatrix(),renderInfo.camera.GetProjMatrix() );
			}

			graphics->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			graphics->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
			graphics->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

			graphics->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE,TRUE );
			graphics->SetRenderState( D3DRS_SRCBLENDALPHA,D3DBLEND_ONE );
			graphics->SetRenderState( D3DRS_DESTBLENDALPHA,D3DBLEND_ONE );
			graphics->SetRenderState( D3DRS_BLENDOPALPHA,D3DBLENDOP_ADD );

			rootGameObject->Render( renderInfo );

			graphics->SetRenderState( D3DRS_SEPARATEALPHABLENDENABLE,FALSE );
			graphics->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

			graphics->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

			float x = (float)graphics->GetBackBufferWidth()-150.0f+0.5f;
			float y = (float)graphics->GetBackBufferHeight()-80.0f+0.5f;

			m_handleRotateX.Render( x,y );
			m_handleRotateY.Render( x+40.0f,y );
			m_handleRotateZ.Render( x+80.0f,y );

			y += 40.0f;

			m_handleMoveX.Render( x,y );
			m_handleMoveY.Render( x+40.0f,y );
			m_handleMoveZ.Render( x+80.0f,y );

			graphics->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

			GameObject* selectObject = GetDocument()->GetSelectGameObject();
			//if( selectObject )
			//{
			//	// TODO:Draw Select Object
			//	ShaderPtr pDefaultShader = graphics->GetDefaultShader();
			//	if( pDefaultShader )
			//	{
			//		ID3DXEffectPtr pEffect = pDefaultShader->GetEffect();

			//		graphics->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

			//		{
			//		graphics->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 );

			//		pEffect->SetTechnique( "TechScreenDiffuseTexture" );
			//		
			//		UINT passes;
			//		pEffect->Begin( &passes,0 );
			//		
			//		struct sVertex
			//		{
			//			float x;
			//			float y;
			//			float z;
			//			float rhw;
			//			float u;
			//			float v;
			//		};

			//		float right = (float)graphics->GetBackBufferWidth()+0.5f;
			//		float left = right-512.0f+0.5f;
			//		float top = 0.0f+0.5f;
			//		float bottom = top+512.0f+0.5f;

			//		sVertex v[4]={
			//			{  left,   top,0.0f,1.0f,0.0f,0.0f},
			//			{ right,   top,0.0f,1.0f,1.0f,0.0f},
			//			{  left,bottom,0.0f,1.0f,0.0f,1.0f},
			//			{ right,bottom,0.0f,1.0f,1.0f,1.0f},
			//		};

			//		D3DXVECTOR4 color( 1.0f,1.0f,1.0f,1.0f );
			//		pEffect->SetVector( "g_materialDiffuse" , &color );
			//		pEffect->SetTexture( "g_Texture",m_controlUITextureAtlas->GetTexture()->GetTexture() );

			//		for( UINT cpass = 0; cpass<passes; cpass++ )
			//		{
			//			pEffect->BeginPass( cpass );

			//			graphics->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP,2,v,sizeof(sVertex) );

			//			pEffect->EndPass();
			//		}

			//		pEffect->End();
			//	}

			//		graphics->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			//	}
			//}

			//// Debug Shadow Map
			//ShaderPtr pDefaultShader = graphics->GetDefaultShader();
			//if( pDefaultShader )
			//{
			//	ID3DXEffectPtr pEffect = pDefaultShader->GetEffect();

			//	graphics->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

			//	{
			//		graphics->SetFVF( D3DFVF_XYZ );

			//		pEffect->SetTechnique( "TechEdge" );
			//		
			//		UINT passes;
			//		pEffect->Begin( &passes,0 );

			//		BoundingFrustum frustum( renderInfo.matShadow );

			//		DWORD indices[] =
			//		{
			//			0,1, // top,near,
			//			0,2, // left,near,
			//			1,3, // right,near,
			//			2,3, // botom,near,

			//			0,4, // left,top
			//			1,5, // right,top
			//			2,6, // left,bottom
			//			3,7, // right,bottom

			//			4,5, // top,near,
			//			4,6, // left,near,
			//			5,7, // right,near,
			//			6,7, // botom,near,
			//		};
			//		
			//		D3DXVECTOR4 color( 0.0f,0.0f,0.0f,1.0f );
			//		pEffect->SetVector( "g_edgeColor" , &color );
			//		pEffect->SetTexture( "g_Texture",m_shadowMap->GetTexture() );

			//		for( UINT cpass = 0; cpass<passes; cpass++ )
			//		{
			//			pEffect->BeginPass( cpass );

			//			graphics->DrawIndexedPrimitiveUP( D3DPT_LINELIST,0,BoundingFrustum::CornerCount,12,indices,D3DFMT_INDEX32,frustum.GetCorners(),sizeof(D3DXVECTOR3) );

			//			pEffect->EndPass();
			//		}

			//		pEffect->End();
			//	}

			//	{
			//		graphics->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 );

			//		pEffect->SetTechnique( "TechScreenDiffuseTexture" );
			//		
			//		UINT passes;
			//		pEffect->Begin( &passes,0 );
			//		
			//		struct sVertex
			//		{
			//			float x;
			//			float y;
			//			float z;
			//			float rhw;
			//			float u;
			//			float v;
			//		};

			//		float right = (float)graphics->GetBackBufferWidth();
			//		float left = right-400.0f;
			//		float top = 0.0f;
			//		float bottom = top+400.0f;

			//		sVertex v[4]={
			//			{  left,   top,0.0f,1.0f,0.0f,0.0f},
			//			{ right,   top,0.0f,1.0f,1.0f,0.0f},
			//			{  left,bottom,0.0f,1.0f,0.0f,1.0f},
			//			{ right,bottom,0.0f,1.0f,1.0f,1.0f},
			//		};

			//		D3DXVECTOR4 color( 1.0f,1.0f,1.0f,1.0f );
			//		pEffect->SetVector( "g_materialDiffuse" , &color );
			//		pEffect->SetTexture( "g_Texture",m_shadowMap->GetTexture() );

			//		for( UINT cpass = 0; cpass<passes; cpass++ )
			//		{
			//			pEffect->BeginPass( cpass );

			//			graphics->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP,2,v,sizeof(sVertex) );

			//			pEffect->EndPass();
			//		}

			//		pEffect->End();
			//	}

			//	graphics->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			//}

			graphics->EndScene();
		}
	}

}

//void CMikuMikuGameEngineView::OnClose()
//{
//	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
//
//	Graphics::GetInstance()->Cleanup();
//
//	CView::OnClose();
//}

void CMikuMikuGameEngineView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����܂��B

	delete m_coordinateAxis;

	m_shadowMap.reset();

	Graphics::GetInstance()->Cleanup();
}

BOOL CMikuMikuGameEngineView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B

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

			// TODO:�f�o�C�X���X�g��������

			Graphics::GetInstance()->Resize( size.cx, size.cy );

			// TODO:�f�o�C�X���A����

			Graphics::GetInstance()->EnableRender(true);
		}
		break;
	}

	return CView::PreTranslateMessage(pMsg);
}

void CMikuMikuGameEngineView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����܂��B

	if( m_initialized )
	{
		Locker lock( Graphics::GetInstance()->GetCriticalSection() );

		Graphics::GetInstance()->EnableRender(false);

		// TODO:�f�o�C�X���X�g��������

		Graphics::GetInstance()->Resize( cx, cy );

		// TODO:�f�o�C�X���A����

		Graphics::GetInstance()->EnableRender(true);
	}
}

//BOOL CMikuMikuGameEngineView::OnEraseBkgnd(CDC* pDC)
//{
//	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
//
//	return TRUE;
//	//return CView::OnEraseBkgnd(pDC);
//}

void CMikuMikuGameEngineView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	switch( m_controlState )
	{
	case Idle:
		{
			if( m_handleMoveXOver )
			{
			}
			else if( m_handleMoveYOver )
			{
			}
			else if( m_handleMoveZOver )
			{
			}
			else if( m_handleRotateXOver )
			{
			}
			else if( m_handleRotateYOver )
			{
			}
			else if( m_handleRotateZOver )
			{
			}
			else
			{
				POINT pt;
				pt.x = point.x;
				pt.y = point.y;

				m_ptCursorDown = pt;
				m_controlState = CameraRotate;

				SetCapture();
			}
		}
		break;
	case CameraRotate:
		{
		}
		break;
	case CameraMove:
		{
		}
		break;
	case HandleMoveX:
		{
		}
		break;
	case HandleMoveY:
		{
		}
		break;
	case HandleMoveZ:
		{
		}
		break;
	case HandleRotateX:
		{
		}
		break;
	case HandleRotateY:
		{
		}
		break;
	case HandleRotateZ:
		{
		}
		break;
	}

	CView::OnRButtonDown(nFlags, point);
}

void CMikuMikuGameEngineView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	switch( m_controlState )
	{
	case Idle:
		{
		}
		break;
	case CameraRotate:
		{
			POINT pt;
			pt.x = point.x;
			pt.y = point.y;

			m_controlState = Idle;

			ReleaseCapture();
		}
		break;
	case CameraMove:
		{
		}
		break;
	case HandleMoveX:
		{
		}
		break;
	case HandleMoveY:
		{
		}
		break;
	case HandleMoveZ:
		{
		}
		break;
	case HandleRotateX:
		{
		}
		break;
	case HandleRotateY:
		{
		}
		break;
	case HandleRotateZ:
		{
		}
		break;
	}

	CView::OnRButtonUp(nFlags, point);
}

void CMikuMikuGameEngineView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	POINT pt;
	pt.x = point.x;
	pt.y = point.y;

	Graphics* graphics = Graphics::GetInstance();

	int x = pt.x-m_ptCursorDown.x;
	int y = pt.y-m_ptCursorDown.y;

	switch( m_controlState )
	{
	case Idle:
		{
			float handleX = (float)graphics->GetBackBufferWidth()-150.0f+0.5f;
			float handleY = (float)graphics->GetBackBufferHeight()-80.0f+0.5f;

			if( handleX <= pt.x && pt.x <= handleX+32.0f &&
				handleY <= pt.y && pt.y <= handleY+32.0f )
			{
				m_handleRotateX.SetSpriteName( tstring(_T("HandleRotateXOver")) );
				m_handleRotateXOver = true;
			}
			else
			{
				m_handleRotateX.SetSpriteName( tstring(_T("HandleRotateXNormal")) );
				m_handleRotateXOver = false;
			}

			handleX+=40.0f;
			if( handleX <= pt.x && pt.x <= handleX+32.0f &&
				handleY <= pt.y && pt.y <= handleY+32.0f )
			{
				m_handleRotateY.SetSpriteName( tstring(_T("HandleRotateYOver")) );
				m_handleRotateYOver = true;
			}
			else
			{
				m_handleRotateY.SetSpriteName( tstring(_T("HandleRotateYNormal")) );
				m_handleRotateYOver = false;
			}

			handleX+=40.0f;
			if( handleX <= pt.x && pt.x <= handleX+32.0f &&
				handleY <= pt.y && pt.y <= handleY+32.0f )
			{
				m_handleRotateZ.SetSpriteName( tstring(_T("HandleRotateZOver")) );
				m_handleRotateZOver = true;
			}
			else
			{
				m_handleRotateZ.SetSpriteName( tstring(_T("HandleRotateZNormal")) );
				m_handleRotateZOver = false;
			}

			handleX -= 80.0f;
			handleY += 40.0f;

			if( handleX <= pt.x && pt.x <= handleX+32.0f &&
				handleY <= pt.y && pt.y <= handleY+32.0f )
			{
				m_handleMoveX.SetSpriteName( tstring(_T("HandleMoveXOver")) );
				m_handleMoveXOver = true;
			}
			else
			{
				m_handleMoveX.SetSpriteName( tstring(_T("HandleMoveXNormal")) );
				m_handleMoveXOver = false;
			}

			handleX+=40.0f;
			if( handleX <= pt.x && pt.x <= handleX+32.0f &&
				handleY <= pt.y && pt.y <= handleY+32.0f )
			{
				m_handleMoveY.SetSpriteName( tstring(_T("HandleMoveYOver")) );
				m_handleMoveYOver = true;
			}
			else
			{
				m_handleMoveY.SetSpriteName( tstring(_T("HandleMoveYNormal")) );
				m_handleMoveYOver = false;
			}

			handleX+=40.0f;
			if( handleX <= pt.x && pt.x <= handleX+32.0f &&
				handleY <= pt.y && pt.y <= handleY+32.0f )
			{
				m_handleMoveZ.SetSpriteName( tstring(_T("HandleMoveZOver")) );
				m_handleMoveZOver = true;
			}
			else
			{
				m_handleMoveZ.SetSpriteName( tstring(_T("HandleMoveZNormal")) );
				m_handleMoveZOver = false;
			}
		}
		break;
	case CameraRotate:
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
	case CameraMove:
		{
			m_ptCursorDown = pt;

			if( x!=0 || y!=0 )
			{
				D3DXVECTOR3 move((float)-x,(float)y,0.0f);
				move *= 0.05f;

				D3DXMATRIX matCameraRotate;
				D3DXMatrixRotationQuaternion( &matCameraRotate,&m_cameraRotation );

				D3DXVec3TransformNormal( &move,&move,&matCameraRotate );

				m_cameraPosition += move;
			}
		}
		break;
	case HandleMoveX:
		{
			if( y!=0 )
			{
				D3DXVECTOR3 move((float)-y,0.0f,0.0f);
				move *= 0.05f;

				GameObject* selectObject = GetDocument()->GetSelectGameObject();
				if( selectObject )
				{
					D3DXVECTOR3 localPosition = selectObject->GetLocalPosition() + move;
					selectObject->SetLocalPosition( localPosition );
				}

				POINT ptCursor = m_ptCursorDown;
				ClientToScreen( &ptCursor );

				SetCursorPos( ptCursor.x,ptCursor.y );

				OutputDebugStringFormat( _T("MouseMouve HandleX(%d,%d)\n"),pt.x,pt.y );
			}
		}
		break;
	case HandleMoveY:
		{
			if( y!=0 )
			{
				D3DXVECTOR3 move(0.0f,(float)-y,0.0f);
				move *= 0.05f;

				GameObject* selectObject = GetDocument()->GetSelectGameObject();
				if( selectObject )
				{
					D3DXVECTOR3 localPosition = selectObject->GetLocalPosition() + move;
					selectObject->SetLocalPosition( localPosition );
				}

				POINT ptCursor = m_ptCursorDown;
				ClientToScreen( &ptCursor );

				SetCursorPos( ptCursor.x,ptCursor.y );
			}
		}
		break;
	case HandleMoveZ:
		{
			if( y!=0 )
			{
				D3DXVECTOR3 move(0.0f,0.0f,(float)y);
				move *= 0.05f;

				GameObject* selectObject = GetDocument()->GetSelectGameObject();
				if( selectObject )
				{
					D3DXVECTOR3 localPosition = selectObject->GetLocalPosition() + move;
					selectObject->SetLocalPosition( localPosition );
				}

				POINT ptCursor = m_ptCursorDown;
				ClientToScreen( &ptCursor );

				SetCursorPos( ptCursor.x,ptCursor.y );
			}
		}
		break;
	case HandleRotateX:
		{
			if( y!=0 )
			{
				D3DXQUATERNION q;
				D3DXQuaternionRotationAxis( &q,&D3DXVECTOR3(1.0f,0.0f,0.0f),D3DXToRadian( y*0.573f ) );

				GameObject* selectObject = GetDocument()->GetSelectGameObject();
				if( selectObject )
				{
					D3DXQUATERNION localRotation = selectObject->GetLocalRotation();
					localRotation = q * localRotation;
					selectObject->SetLocalRotation( localRotation );
				}

				POINT ptCursor = m_ptCursorDown;
				ClientToScreen( &ptCursor );

				SetCursorPos( ptCursor.x,ptCursor.y );

				OutputDebugStringFormat( _T("MouseMouve HandleX(%d,%d)\n"),pt.x,pt.y );
			}
		}
		break;
	case HandleRotateY:
		{
			if( y!=0 )
			{
				D3DXQUATERNION q;
				D3DXQuaternionRotationAxis( &q,&D3DXVECTOR3(0.0f,1.0f,0.0f),D3DXToRadian( y*0.573f ) );

				GameObject* selectObject = GetDocument()->GetSelectGameObject();
				if( selectObject )
				{
					D3DXQUATERNION localRotation = selectObject->GetLocalRotation();
					localRotation = q * localRotation;
					selectObject->SetLocalRotation( localRotation );
				}

				POINT ptCursor = m_ptCursorDown;
				ClientToScreen( &ptCursor );

				SetCursorPos( ptCursor.x,ptCursor.y );

				OutputDebugStringFormat( _T("MouseMouve HandleY(%d,%d)\n"),pt.x,pt.y );
			}
		}
		break;
	case HandleRotateZ:
		{
			if( y!=0 )
			{
				D3DXQUATERNION q;
				D3DXQuaternionRotationAxis( &q,&D3DXVECTOR3(0.0f,0.0f,1.0f),D3DXToRadian( y*0.573f ) );

				GameObject* selectObject = GetDocument()->GetSelectGameObject();
				if( selectObject )
				{
					D3DXQUATERNION localRotation = selectObject->GetLocalRotation();
					localRotation = q * localRotation;
					selectObject->SetLocalRotation( localRotation );
				}

				POINT ptCursor = m_ptCursorDown;
				ClientToScreen( &ptCursor );

				SetCursorPos( ptCursor.x,ptCursor.y );

				OutputDebugStringFormat( _T("MouseMouve HandleZ(%d,%d)\n"),pt.x,pt.y );
			}
		}
		break;
	}
	CView::OnMouseMove(nFlags, point);
}

void CMikuMikuGameEngineView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	switch( m_controlState )
	{
	case Idle:
		{
			if( m_handleMoveXOver )
			{
			}
			else if( m_handleMoveYOver )
			{
			}
			else if( m_handleMoveZOver )
			{
			}
			else
			{
				POINT pt;
				pt.x = point.x;
				pt.y = point.y;

				m_ptCursorDown = pt;
				m_controlState = CameraMove;

				SetCapture();
			}
		}
		break;
	case CameraRotate:
		{
		}
		break;
	case CameraMove:
		{
		}
		break;
	case HandleMoveX:
		{
		}
		break;
	case HandleMoveY:
		{
		}
		break;
	case HandleMoveZ:
		{
		}
		break;
	case HandleRotateX:
		{
		}
		break;
	case HandleRotateY:
		{
		}
		break;
	case HandleRotateZ:
		{
		}
		break;
	}

	CView::OnMButtonDown(nFlags, point);
}

void CMikuMikuGameEngineView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	switch( m_controlState )
	{
	case Idle:
		{
		}
		break;
	case CameraRotate:
		{
		}
		break;
	case CameraMove:
		{
			POINT pt;
			pt.x = point.x;
			pt.y = point.y;

			m_controlState = Idle;

			ReleaseCapture();
		}
		break;
	case HandleMoveX:
		{
		}
		break;
	case HandleMoveY:
		{
		}
		break;
	case HandleMoveZ:
		{
		}
		break;
	case HandleRotateX:
		{
		}
		break;
	case HandleRotateY:
		{
		}
		break;
	case HandleRotateZ:
		{
		}
		break;
	}

	CView::OnMButtonUp(nFlags, point);
}

BOOL CMikuMikuGameEngineView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	m_cameraRadius -= ((float)zDelta/120.0f)*6.0f;
	
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CMikuMikuGameEngineView::OnCaptureChanged(CWnd *pWnd)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����܂��B

	switch( m_controlState )
	{
	case Idle:
		{
			m_controlState = Idle;
		}
		break;
	case CameraRotate:
		{
			m_controlState = Idle;
		}
		break;
	case CameraMove:
		{
			m_controlState = Idle;
		}
		break;
	case HandleMoveX:
		{
			m_controlState = Idle;

			ShowCursor( TRUE );
		}
		break;
	case HandleMoveY:
		{
			m_controlState = Idle;

			ShowCursor( TRUE );
		}
		break;
	case HandleMoveZ:
		{
			m_controlState = Idle;

			ShowCursor( TRUE );
		}
		break;
	case HandleRotateX:
		{
			m_controlState = Idle;

			ShowCursor( TRUE );
		}
		break;
	case HandleRotateY:
		{
			m_controlState = Idle;

			ShowCursor( TRUE );
		}
		break;
	case HandleRotateZ:
		{
			m_controlState = Idle;

			ShowCursor( TRUE );
		}
		break;
	}

	CView::OnCaptureChanged(pWnd);
}

void CMikuMikuGameEngineView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
	switch( m_controlState )
	{
	case Idle:
		{
			if( m_handleMoveXOver )
			{
				POINT pt;
				pt.x = point.x;
				pt.y = point.y;

				m_ptCursorDown = pt;
				m_controlState = HandleMoveX;

				SetCapture();

				ShowCursor( FALSE );
			}
			else if( m_handleMoveYOver )
			{
				POINT pt;
				pt.x = point.x;
				pt.y = point.y;

				m_ptCursorDown = pt;
				m_controlState = HandleMoveY;

				SetCapture();

				ShowCursor( FALSE );
			}
			else if( m_handleMoveZOver )
			{
				POINT pt;
				pt.x = point.x;
				pt.y = point.y;

				m_ptCursorDown = pt;
				m_controlState = HandleMoveZ;

				SetCapture();

				ShowCursor( FALSE );
			}
			else if( m_handleRotateXOver )
			{
				POINT pt;
				pt.x = point.x;
				pt.y = point.y;

				m_ptCursorDown = pt;
				m_controlState = HandleRotateX;

				SetCapture();

				ShowCursor( FALSE );
			}
			else if( m_handleRotateYOver )
			{
				POINT pt;
				pt.x = point.x;
				pt.y = point.y;

				m_ptCursorDown = pt;
				m_controlState = HandleRotateY;

				SetCapture();

				ShowCursor( FALSE );
			}
			else if( m_handleRotateZOver )
			{
				POINT pt;
				pt.x = point.x;
				pt.y = point.y;

				m_ptCursorDown = pt;
				m_controlState = HandleRotateZ;

				SetCapture();

				ShowCursor( FALSE );
			}
			else
			{
			}
		}
		break;
	case CameraRotate:
		{
		}
		break;
	case CameraMove:
		{
		}
		break;
	case HandleMoveX:
		{
		}
		break;
	case HandleMoveY:
		{
		}
		break;
	case HandleMoveZ:
		{
		}
		break;
	case HandleRotateX:
		{
		}
		break;
	case HandleRotateY:
		{
		}
		break;
	case HandleRotateZ:
		{
		}
		break;
	}

	CView::OnLButtonDown(nFlags, point);
}

void CMikuMikuGameEngineView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B

	switch( m_controlState )
	{
	case Idle:
		{
		}
		break;
	case CameraRotate:
		{
		}
		break;
	case CameraMove:
		{
		}
		break;
	case HandleMoveX:
		{
			POINT pt;
			pt.x = point.x;
			pt.y = point.y;

			m_controlState = Idle;

			ReleaseCapture();

			ShowCursor( TRUE );
		}
		break;
	case HandleMoveY:
		{
			POINT pt;
			pt.x = point.x;
			pt.y = point.y;

			m_controlState = Idle;

			ReleaseCapture();

			ShowCursor( TRUE );
		}
		break;
	case HandleMoveZ:
		{
			POINT pt;
			pt.x = point.x;
			pt.y = point.y;

			m_controlState = Idle;

			ReleaseCapture();

			ShowCursor( TRUE );
		}
		break;
	case HandleRotateX:
		{
			POINT pt;
			pt.x = point.x;
			pt.y = point.y;

			m_controlState = Idle;

			ReleaseCapture();

			ShowCursor( TRUE );
		}
		break;
	case HandleRotateY:
		{
			POINT pt;
			pt.x = point.x;
			pt.y = point.y;

			m_controlState = Idle;

			ReleaseCapture();

			ShowCursor( TRUE );
		}
		break;
	case HandleRotateZ:
		{
			POINT pt;
			pt.x = point.x;
			pt.y = point.y;

			m_controlState = Idle;

			ReleaseCapture();

			ShowCursor( TRUE );
		}
		break;
	}

	CView::OnLButtonUp(nFlags, point);
}
