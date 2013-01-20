
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

	m_shadowMap=RenderTexturePtr( new RenderTexture );
	m_shadowMap->Create( 2048,2048,D3DFMT_R32F );

	{
		//tstring xFilePath = _T("project\\assets\\Stage\\ゲキド街v3.0.x");
		tstring xFilePath = _T("project\\assets\\stage01\\stage01.x");
		//tstring xFilePath = _T("project\\assets\\Sample5.x");

		TCHAR path[MAX_PATH];
		_tcscpy_s( path,PathFindFileName( xFilePath.c_str() ) );

		PathRemoveExtension( path );

		ModelPtr pModel = ResourceManager::GetInstance().GetResource<Model>( xFilePath );
		if( !pModel )
		{
			XFileLoader xFileLoader;
			pModel = xFileLoader.Open( xFilePath,10.0f );

			ResourceManager::GetInstance().AddResource( xFilePath,pModel );
		}

		if( pModel )
		{
			GameObject* gameObject = CreateGameObjectFromModelFrame( GetDocument()->GetRootGameObject(),pModel->GetFrame(),true );
		}
	}

	{
		//tstring pmdFilePath = _T("project\\assets\\Model\\Lat式ミクVer2.3\\Lat式ミクVer2.3_Normal.pmd");
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

	{
		tstring pmdFilePath = _T("project\\assets\\Model\\つむじ風\\つむじ風.pmd");

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

			gameObject->SetPMDModelRenderer( pmdModelRenderer );

			gameObject->SetLocalPosition( D3DXVECTOR3(10.0f,0.0f,0.0f) );

			GetDocument()->AddGameObject( gameObject,GetDocument()->GetRootGameObject(),true );

			tstring vmdFilePath = _T("project\\assets\\つむじ風.vmd");

			VMDAnimationClipPtr pAnimationClip = ResourceManager::GetInstance().GetResource<VMDAnimationClip>( vmdFilePath );
			if( !pAnimationClip )
			{
				VMDFileLoader vmdFileLoader;
				pAnimationClip = vmdFileLoader.Open( vmdFilePath );

				ResourceManager::GetInstance().AddResource( vmdFilePath,pAnimationClip );
			}

			if( pAnimationClip )
			{
				gameObject->SetVMDAnimationClip( pAnimationClip );
			}
		}
	}

	{
		tstring pmdFilePath = _T("project\\assets\\Model\\ＭＭＤのエンジンをＭＭＤエンジンで\\MMDのエンジンをMMDエンジンで.pmd");

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

			gameObject->SetPMDModelRenderer( pmdModelRenderer );

			gameObject->SetLocalPosition( D3DXVECTOR3(-10.0f,0.0f,0.0f) );

			GetDocument()->AddGameObject( gameObject,GetDocument()->GetRootGameObject(),true );

			tstring vmdFilePath = _T("project\\assets\\MMDエンジンのエンジン.vmd");

			VMDAnimationClipPtr pAnimationClip = ResourceManager::GetInstance().GetResource<VMDAnimationClip>( vmdFilePath );
			if( !pAnimationClip )
			{
				VMDFileLoader vmdFileLoader;
				pAnimationClip = vmdFileLoader.Open( vmdFilePath );

				ResourceManager::GetInstance().AddResource( vmdFilePath,pAnimationClip );
			}

			if( pAnimationClip )
			{
				gameObject->SetVMDAnimationClip( pAnimationClip );
			}
		}
	}

	{
		tstring pmdFilePath = _T("project\\assets\\Model\\MMDエンジン原理可視モデル\\MMDエンジンかしもでる.pmd");

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

			gameObject->SetPMDModelRenderer( pmdModelRenderer );

			gameObject->SetLocalPosition( D3DXVECTOR3(-20.0f,0.0f,0.0f) );

			GetDocument()->AddGameObject( gameObject,GetDocument()->GetRootGameObject(),true );

			tstring vmdFilePath = _T("project\\assets\\MMDエンジン原理可視モデルでのテストぐるぐる.vmd");

			VMDAnimationClipPtr pAnimationClip = ResourceManager::GetInstance().GetResource<VMDAnimationClip>( vmdFilePath );
			if( !pAnimationClip )
			{
				VMDFileLoader vmdFileLoader;
				pAnimationClip = vmdFileLoader.Open( vmdFilePath );

				ResourceManager::GetInstance().AddResource( vmdFilePath,pAnimationClip );
			}

			if( pAnimationClip )
			{
				gameObject->SetVMDAnimationClip( pAnimationClip );
			}
		}
	}

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

#include "engine/core/math/BoundingFrustum.h"
#include "engine/core/math/BoundingBox.h"
#include "engine/core/math/BoundingCone.h"
#include "engine/core/math/Ray.h"

//D3DXMATRIX CalcShadowMatrix( sRenderInfo& renderInfo )
//{
//	const float ZFAR_MAX = 800.0f;
//	const float Z_EPSILON=0.0001f;
//	const float ZNEAR_MIN = 1.0f;
//	const float W_EPSILON = 0.001f;
//
//	float fMinInfinityZ = 1.5f;
//	bool bSlideBack = true;
//	bool bUnitCubeClip = true;
//
//	D3DXMATRIX lightView, lightProj, virtualCameraViewProj, virtualCameraView, virtualCameraProj;
//
//	const D3DXVECTOR3 yAxis  ( 0.f, 1.f, 0.f);
//	const D3DXVECTOR3 zAxis  ( 0.f, 0.f, 1.f);
//
//	const D3DXMATRIX& matView = renderInfo.camera.GetViewMatrix();
//
//	D3DXVECTOR3 frustumCorners[ BoundingFrustum::CornerCount ];
//	renderInfo.camera.GetFrustumCorners( 10.0f, frustumCorners);
//	for(int index = 0; index < BoundingFrustum::CornerCount; index++ )
//	{
//		D3DXVec3TransformCoord( &frustumCorners[index],&frustumCorners[index], &matView );
//	}
//
//	BoundingBox lightBox = BoundingBox::CreateFromPoints(frustumCorners,BoundingFrustum::CornerCount);
//
//	float zNear = std::min<float>( lightBox.GetMin().z,FLT_MAX );
//	float zFar = std::max<float>( lightBox.GetMax().z,0.0f );
//
//	zNear = std::max<float>( ZNEAR_MIN, zNear );
//    zFar = std::min<float>( ZFAR_MAX, zFar );
//
//	float slideBack = 0.0f;
//	
//	//  compute a slideback, that force some distance between the infinity plane and the view-box
//    float infinity = zFar/(zFar-zNear);
//    float fInfinityZ = fMinInfinityZ;
//    if ( (infinity<=fInfinityZ) && bSlideBack )
//    {
//        float additionalSlide = fInfinityZ*(zFar-zNear) - zFar + Z_EPSILON;
//        slideBack = additionalSlide;
//        zFar += additionalSlide;
//        zNear += additionalSlide;
//    }
//
//	if( bSlideBack )
//    {
//        //  clamp the view-cube to the slid back objects...
//		const D3DXVECTOR3 eyePt(0.f, 0.f, 0.f);
//        const D3DXVECTOR3 eyeDir(0.f, 0.f, 1.f);
//        D3DXMatrixTranslation(&virtualCameraView, 0.f, 0.f, slideBack);
//
//        if ( bUnitCubeClip )
//        {
//            BoundingCone bc( lightBox, virtualCameraView, eyePt, eyeDir );
//            D3DXMatrixPerspectiveLH( &virtualCameraProj, 2.f*tanf(bc.GetFovX())*zNear, 2.f*tanf(bc.GetFovY())*zNear, zNear, zFar );
//        }
//        else
//        {
//            const float viewHeight = ZFAR_MAX * 0.57735026919f;  // tan(0.5f*VIEW_ANGLE)*ZFAR_MAX       
//			float viewWidth  = viewHeight * renderInfo.camera.GetAspect();
//            float halfFovy = atanf( viewHeight / (ZFAR_MAX+slideBack) );
//            float halfFovx = atanf( viewWidth  / (ZFAR_MAX+slideBack) );
//
//            D3DXMatrixPerspectiveLH( &virtualCameraProj, 2.f*tanf(halfFovx)*zNear, 2.f*tanf(halfFovy)*zNear, zNear, zFar );
//        }
////		D3DXMatrixPerspectiveFovLH( &virtualCameraProj, 2.f*halfFovy, halfFovx/halfFovy, m_zNear, m_zFar);
//    }
//    else
//    {
//        D3DXMatrixIdentity( &virtualCameraView );
//        D3DXMatrixPerspectiveFovLH( &virtualCameraProj, D3DXToRadian(60.f), renderInfo.camera.GetAspect(), zNear, zFar );
//    }
//
//	D3DXMatrixMultiply(&virtualCameraViewProj, &matView, &virtualCameraView);
//	D3DXMatrixMultiply(&virtualCameraViewProj, &virtualCameraViewProj, &virtualCameraProj);
//   
//	D3DXMATRIX eyeToPostProjectiveVirtualCamera;
//	D3DXMatrixMultiply(&eyeToPostProjectiveVirtualCamera, &virtualCameraView, &virtualCameraProj);
//
//	D3DXVECTOR3 eyeLightDir;
//	D3DXVec3TransformNormal(&eyeLightDir, &(-renderInfo.lightDir), &matView );
//
//	//  directional light becomes a point on infinity plane in post-projective space
//	D3DXVECTOR4 lightDirW( eyeLightDir.x, eyeLightDir.y, eyeLightDir.z, 0.f );
//	D3DXVECTOR4 ppLight;
//
//	D3DXVec4Transform( &ppLight, &lightDirW, &virtualCameraProj );
//
//	bool bShadowTestInverted = (ppLight.w < 0.f);
//
//	 //  compute the projection matrix...
//    //  if the light is >= 1000 units away from the unit box, use an ortho matrix (standard shadow mapping)
//    if( (fabsf(ppLight.w) <= W_EPSILON) )  // orthographic matrix; uniform shadow mapping
//    {
//        D3DXVECTOR3 ppLightDirection(ppLight.x, ppLight.y, ppLight.z);
//        D3DXVec3Normalize(&ppLightDirection, &ppLightDirection);
//
//        BoundingBox ppUnitBox( D3DXVECTOR3(-1, -1, 0),D3DXVECTOR3(1, 1, 1) );
//        D3DXVECTOR3 cubeCenter = (ppUnitBox.GetMax() + ppUnitBox.GetMin())*0.5f;
//        float t;
//
//		Ray ray( cubeCenter, ppLightDirection );
//
//        ppUnitBox.Intersect( ray,&t );
//        D3DXVECTOR3 lightPos = cubeCenter + 2.f*t*ppLightDirection;
//        D3DXVECTOR3 axis = yAxis;
//        
//        //  if the yAxis and the view direction are aligned, choose a different up vector, to avoid singularity
//        //  artifacts
//        if ( fabsf(D3DXVec3Dot(&ppLightDirection, &yAxis))>0.99f )
//            axis = zAxis;
//
//        D3DXMatrixLookAtLH(&lightView, &lightPos, &cubeCenter, &axis);
//		ppUnitBox.Transform( lightView);
//
//		const D3DXVECTOR3& vMin = ppUnitBox.GetMin();
//		const D3DXVECTOR3& vMax = ppUnitBox.GetMax();
//        D3DXMatrixOrthoOffCenterLH(&lightProj, vMin.x, vMax.x, vMin.y, vMax.y, vMin.z, vMax.z);
//    }
//    else  // otherwise, use perspective shadow mapping
//    {
//        D3DXVECTOR3 ppLightPos;
//        float wRecip = 1.0f / ppLight.w;
//        ppLightPos.x = ppLight.x * wRecip;
//        ppLightPos.y = ppLight.y * wRecip;
//        ppLightPos.z = ppLight.z * wRecip;
//
//        D3DXMATRIX eyeToPostProjectiveLightView;
//
//        const float ppCubeRadius = 1.5f;  // the post-projective view box is [-1,-1,0]..[1,1,1] in DirectX, so its radius is 1.5
//        const D3DXVECTOR3 ppCubeCenter(0.f, 0.f, 0.5f);
//
//        if( bShadowTestInverted )  // use the inverse projection matrix
//        {
//			//  project the entire unit cube into the shadow map  
//			BoundingBox unitCube( D3DXVECTOR3(-1.f, -1.f, 0.f),D3DXVECTOR3( 1.f, 1.f, 1.f ) );
//
//			D3DXMATRIX tmpProjection;
//			D3DXMatrixIdentity(&tmpProjection);
//
//			if( bUnitCubeClip )
//            {
//                //  clip the shadow map to just the used portions of the unit box.
//				unitCube = lightBox;
//				tmpProjection = eyeToPostProjectiveVirtualCamera;
//            }
//
//			BoundingCone viewCone( unitCube, tmpProjection, ppLightPos );
//            
//            //  construct the inverse projection matrix -- clamp the fNear value for sanity (clamping at too low
//            //  a value causes significant underflow in a 24-bit depth buffer)
//            //  the multiplication is necessary since I'm not checking shadow casters
//
//			float fFar , fNear;
//            fFar  = max(0.001f, viewCone.GetNear()*0.3f);
//            fNear = -fFar;
//            lightView = viewCone.GetLookAt();
//            D3DXMatrixPerspectiveLH( &lightProj, 2.f*tanf(viewCone.GetFovX())*fNear, 2.f*tanf(viewCone.GetFovY())*fNear, fNear, fFar );
//            //D3DXMatrixPerspectiveFovLH(&lightProj, 2.f*viewCone.fovy, viewCone.fovx/viewCone.fovy, m_ppNear, m_ppFar);
//        }
//        else  // regular projection matrix
//        {
//            float fFovy, fAspect, fFar, fNear;
//            if( !bUnitCubeClip )
//            {
//                D3DXVECTOR3 lookAt = ppCubeCenter - ppLightPos;
//                
//                float distance = D3DXVec3Length(&lookAt);
//                lookAt = lookAt / distance;
//                
//                D3DXVECTOR3 axis = yAxis;
//                //  if the yAxis and the view direction are aligned, choose a different up vector, to avoid singularity
//                //  artifacts
//                if ( fabsf(D3DXVec3Dot(&yAxis, &lookAt))>0.99f )
//				{
//                    axis = zAxis;
//				}
//
//                //  this code is super-cheese.  treats the unit-box as a sphere
//                //  lots of problems, looks like hell, and requires that MinInfinityZ >= 2
//                D3DXMatrixLookAtLH(&lightView, &ppLightPos, &ppCubeCenter, &axis);
//                fFovy = 2.f*atanf(ppCubeRadius/distance);
//                fAspect = 1.f;
//                fNear = max(0.001f, distance - 2.f*ppCubeRadius);
//                fFar = distance + 2.f*ppCubeRadius;
//                D3DXMatrixMultiply(&eyeToPostProjectiveLightView, &eyeToPostProjectiveVirtualCamera, &lightView);
//            }
//            else
//            {
//                //  unit cube clipping
//                //  fit a cone to the bounding geometries of all shadow receivers (incl. terrain) in the scene
//                BoundingCone bc( lightBox, eyeToPostProjectiveVirtualCamera, ppLightPos );
//                lightView = bc.GetLookAt();
//                D3DXMatrixMultiply(&eyeToPostProjectiveLightView, &eyeToPostProjectiveVirtualCamera, &lightView);
//                float fDistance = D3DXVec3Length(&(ppLightPos-ppCubeCenter));
//                fFovy = 2.f * bc.GetFovY();
//                fAspect = bc.GetFovX() / bc.GetFovY();
//                fFar = bc.GetFar();
//                //  hack alert!  adjust the near-plane value a little bit, to avoid clamping problems
//                fNear = bc.GetNear() * 0.6f;
//            }
//           
//            fNear = max(0.001f, fNear);
//            D3DXMatrixPerspectiveFovLH(&lightProj, fFovy, fAspect, fNear, fFar);
//        }
//    }
//
//	D3DXMATRIX matLightViewProj;
//
//    //  build the composite matrix that transforms from world space into post-projective light space
//    D3DXMatrixMultiply(&matLightViewProj, &lightView, &lightProj);
//    D3DXMatrixMultiply(&matLightViewProj, &virtualCameraViewProj, &matLightViewProj);
//
//	return matLightViewProj;
//}

//D3DXMATRIX CalcShadowMatrix( sRenderInfo& renderInfo )
//{
//	D3DXMATRIX matLightViewProj( 
//		0.0201246124f, 0.0041079191f,0.0003380607f, 0.0020539595f, 
//		-0.0100623062f, 0.0082158381f,0.0006761214f, 0.0041079191f, 
//		0.0000000000f,0.0205395948f,0.0058238176f, 0.0102697974f, 
//		0.1006230563f,-0.3632724881f,0.3004102707f, 1.3183637857f );
//
//	D3DXVECTOR3 cameraCenter( 0.0f,10.0f,0.0f);
//	D3DXQUATERNION cameraRotation( 0.0f,0.0f,0.0f,1.0f );
//	float cameraRadius = 35.0f;
//
//	D3DXVECTOR3 lookDir(0.0f,0.0f,1.0f);
//
//	D3DXMATRIX matView;
//	{
//		D3DXVECTOR3 cameraPos(0.0f,0.0f,-1.0f);
//		D3DXVECTOR3 vecLookPt(0.0f,0.0f,1.0f);
//		D3DXVECTOR3 vecUpVec(0.0f,1.0f,0.0f);
//
//		D3DXMATRIX matCameraRotate;
//
//		D3DXMatrixRotationQuaternion( &matCameraRotate,&cameraRotation );
//
//		D3DXVec3TransformNormal( &vecLookPt,&vecLookPt,&matCameraRotate );
//		D3DXVec3TransformNormal( &lookDir,&cameraPos,&matCameraRotate );
//		D3DXVec3TransformNormal( &vecUpVec,&vecUpVec,&matCameraRotate );
//
//		D3DXVec3Scale( &cameraPos,&cameraPos,cameraRadius );
//		D3DXVec3Add( &cameraPos,&cameraCenter,&cameraPos );
//
//		D3DXVec3Add( &vecLookPt,&cameraPos,&vecLookPt );
//
//		D3DXMatrixLookAtLH( &matView, &cameraPos, &vecLookPt, &vecUpVec );
//	}
//
//	float screenWidth = 1280.0f;
//	float screenHeight = 768.0f;
//
//	float fov = D3DXToRadian( 45.0f );
//
//	D3DXMATRIX matProj;
//	{
//		D3DXMatrixPerspectiveFovLH( &matProj, fov, screenWidth/screenHeight, 1.0f, 1000000.0f);
//	}
//
//	D3DXMATRIX virtualCameraView;
//	D3DXMATRIX virtualCameraProj;
//	D3DXMATRIX virtualCameraViewProj;
//
//	D3DXMATRIX matViewInv = matView * matProj;
//	D3DXMatrixInverse( &matViewInv,NULL,&matViewInv );
//
//	virtualCameraViewProj = matViewInv * matLightViewProj;
//
//	D3DXVECTOR3 lightDir( -0.5f,-1.0f,0.5f );
//
//	D3DXMATRIX matLightView;
//	{
//		D3DXVECTOR3 vLook = lookDir;
//		D3DXVECTOR3 vUp;
//		D3DXVec3Cross( &vUp,&vLook,&(-lightDir) );
//		D3DXVec3Normalize( &vUp,&vUp );
//		D3DXVec3Cross( &vUp,&vUp,&vLook );
//		D3DXVec3Normalize( &vUp,&vUp );
//
//		//ライトの位置を元にしたビューマトリックス生成
//		//ライトのプロジェクションマトリックスを使う
//		//パースペクティブシャドウマップの場合は視錐台のプロジェクションを使う。これによりパースペクティブシャドウマップになる
//		
//		D3DXMatrixLookAtLH( &matLightView,&D3DXVECTOR3(0.0f,0.0f,0.0f),&lightDir,&vUp );
//	}
//
//	D3DXMATRIX matLightViewInv;
//	D3DXMatrixInverse( &matLightViewInv,NULL,&matLightView );
//
//	virtualCameraViewProj = matLightViewInv * virtualCameraViewProj;
//	
//	//D3DXMatrixMultiply(&virtualCameraViewProj, &matView, &virtualCameraView);
//    //D3DXMatrixMultiply(&virtualCameraViewProj, &virtualCameraViewProj, &virtualCameraProj);
// 
//	bool shadowMode1 = true;
//
//	float shadowDistance = 8875.0f;
//
//	float shadowTexWidth = 2048.0f;
//	float shadowTexHeight = 2048.0f;
//
//	const float SKII1 = 1500.0f;
//
//	return matLightViewProj;
//}

D3DXMATRIX CalcShadowMatrix( sRenderInfo& renderInfo )
{
	//表示行列設定
	D3DXVECTOR3 lightDir = renderInfo.lightDir;

	const D3DXMATRIX& matView = renderInfo.camera.GetViewMatrix();
	const D3DXMATRIX& matProj = renderInfo.camera.GetViewMatrix();

	//光の方向に回転する回転行列を生成
	D3DXMATRIX lightRotation;
	D3DXMatrixLookAtLH( &lightRotation,&D3DXVECTOR3(0.0f,0.0f,0.0f),&lightDir,&D3DXVECTOR3(0.0f,1.0f,0.0f) );
	//視錐台から錐台のコーナー情報取得

	D3DXVECTOR3 frustumCorners[ BoundingFrustum::CornerCount ];
	renderInfo.camera.GetFrustumCorners( 100.0f, frustumCorners);
	//光の方向に回転
	for(int index = 0; index < BoundingFrustum::CornerCount; index++ )
	{
		D3DXVec3TransformCoord( &frustumCorners[index],&frustumCorners[index], &lightRotation );
	}

	//錐台を含む最小の箱型空間を生成
	BoundingBox lightBox = BoundingBox::CreateFromPoints(frustumCorners,BoundingFrustum::CornerCount);
	//箱型空間のサイズ取得
	D3DXVECTOR3 boxSize = lightBox.GetMax() - lightBox.GetMin();
	D3DXVECTOR3 halfBoxSize = boxSize * 0.5f;

	//ライトの位置計算
	//平行光源におけるライトの位置は箱型空間のバックパネル中央。
	D3DXVECTOR3 lightPosition = lightBox.GetMin() + halfBoxSize;
	lightPosition.z = lightBox.GetMin().z;

	//ライトの位置を元の座標系に戻す
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

	//ライトの位置を元にしたビューマトリックス生成
	//ライトのプロジェクションマトリックスを使う
	//パースペクティブシャドウマップの場合は視錐台のプロジェクションを使う。これによりパースペクティブシャドウマップになる
	
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

	GetDocument()->GetRootGameObject()->UpdateAnimation( elapsedTime );

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
			// TODO: ここにコードを挿入してください。
			// 描画開始

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

				GetDocument()->GetRootGameObject()->RenderZPlot( renderInfo );

				pD3DDevice->SetRenderTarget( 0,pOldBackBuffer );
				pD3DDevice->SetDepthStencilSurface( pOldDepthBuffer );
				pD3DDevice->SetViewport( &oldViewport );				
			}

			graphics->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 255, 255, 255 ), 1.0f, 0 );

			//{
			//	m_coordinateAxis->Render( renderInfo.camera.GetViewMatrix(),renderInfo.camera.GetProjMatrix() );
			//}

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
			//	D3DXMATRIX matWorldViewProj = selectObject->GetWorldMatrix() * renderInfo.camera.GetViewMatrix() * renderInfo.camera.GetProjMatrix();

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

			ShaderPtr pDefaultShader = graphics->GetDefaultShader();
			if( pDefaultShader )
			{
				ID3DXEffectPtr pEffect = pDefaultShader->GetEffect();

				graphics->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

				graphics->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 );

				pEffect->SetTechnique( "TechScreenDiffuseTexture" );
				
				UINT passes;
				pEffect->Begin( &passes,0 );
				
				struct sVertex
				{
					float x;
					float y;
					float z;
					float rhw;
					float u;
					float v;
				};

				float right = (float)graphics->GetBackBufferWidth();
				float left = right-400.0f;
				float top = 0.0f;
				float bottom = top+400.0f;

				sVertex v[4]={
					{  left,   top,0.0f,1.0f,0.0f,0.0f},
					{ right,   top,0.0f,1.0f,1.0f,0.0f},
					{  left,bottom,0.0f,1.0f,0.0f,1.0f},
					{ right,bottom,0.0f,1.0f,1.0f,1.0f},
				};

				D3DXVECTOR4 color( 1.0f,1.0f,1.0f,1.0f );
				pEffect->SetVector( "g_materialDiffuse" , &color );
				pEffect->SetTexture( "g_Texture",m_shadowMap->GetTexture() );

				for( UINT cpass = 0; cpass<passes; cpass++ )
				{
					pEffect->BeginPass( cpass );

					graphics->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP,2,v,sizeof(sVertex) );

					pEffect->EndPass();
				}

				pEffect->End();

				graphics->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			}

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

	m_shadowMap.reset();

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
