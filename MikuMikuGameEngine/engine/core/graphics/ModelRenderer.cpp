#include "stdafx.h"

#include "ModelRenderer.h"

#include "../../GameObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ModelRenderer::ModelRenderer()
	: m_pMeshContainer(NULL)
{
}

ModelRenderer::~ModelRenderer()
{
	m_pMeshContainer = NULL;
}

void ModelRenderer::SetModel( MeshContainer* pMeshContainer )
{
	m_pMeshContainer = pMeshContainer;
	if( m_pMeshContainer )
	{
		m_pModel = m_pMeshContainer->pModel.lock();
	}
}


DWORD ModelRenderer::GetMaterialNum()
{
	if( m_pMeshContainer )
	{
		return m_pMeshContainer->materialNum;
	}

	return 0;
}

void ModelRenderer::Render( const D3DXMATRIX& matWorld,const sRenderInfo& renderInfo )
{
	if( !m_pMeshContainer )
	{
		return;
	}

	Mesh* pMesh = m_pMeshContainer->pMesh;

	if( !pMesh )
	{
		return;
	}

	D3DXMATRIX matWorldView = matWorld * renderInfo.matView;
	D3DXMATRIX matWorldViewProj = matWorldView * renderInfo.matProj;

	DWORD attrNum = pMesh->GetAttributeRangeNum();

	Graphics* graphics = Graphics::GetInstance();

	graphics->SetRenderState( D3DRS_CULLMODE,D3DCULL_CCW );

	ShaderPtr pDefaultShader = graphics->GetDefaultShader();

	D3DXVECTOR3 vLight = renderInfo.lightDir;
	D3DXVECTOR3 vEye = renderInfo.eyePos;
	//{
	//	D3DXMATRIX matWorldInv;
	//	D3DXMatrixInverse( &matWorldInv,NULL,&matWorld );

	//	D3DXVec3TransformNormal( &vLight,&vLight,&renderInfo.matView );
	//	vLight = -vLight;

	//	D3DXVec3TransformCoord( &vEye,&vEye,&matWorldInv );
	//}

	for( DWORD attrIdx = 0 ; attrIdx < attrNum ; attrIdx++ )
	{
		DWORD attrID = pMesh->GetAttributeID( attrIdx );

		sMaterial* pMaterial = &m_pMeshContainer->pMaterials[ attrID ];

		if( pDefaultShader )
		{
			ID3DXEffectPtr pEffect = pDefaultShader->GetEffect();
		
			pEffect->SetMatrix( "g_mWorldViewProjection",&matWorldViewProj );
			pEffect->SetMatrix( "g_mWorldView",&matWorldView );
			pEffect->SetMatrix( "g_mWorld",&matWorld );
			pEffect->SetMatrix( "g_mView",&renderInfo.matView );

			pEffect->SetValue( "g_materialDiffuse",&pMaterial->colorDiffuse,sizeof(D3DXCOLOR) );
			pEffect->SetValue( "g_materialAmbient" , &pMaterial->colorAmbient,sizeof(D3DXCOLOR) );
			pEffect->SetValue( "g_materialEmissive",&pMaterial->colorEmissive,sizeof(D3DXCOLOR) );

			D3DXCOLOR colorSpecular = pMaterial->colorSpecular * 0.1f;

			pEffect->SetValue( "g_materialSpecular",&colorSpecular,sizeof(D3DXCOLOR) );
			pEffect->SetFloat( "g_materialSpecularPower" , pMaterial->specularPower );

			std::string techName = "TechDiffuse";
			if( pMaterial->textureDiffuse )
			{
				techName+="Texture";
				pEffect->SetTexture( "g_Texture",pMaterial->textureDiffuse->GetTexture() );
			}
			if( pMaterial->textureSphere )
			{
				switch( pMaterial->spheremap )
				{
				case eSPHEREMAP_MUL:
					techName+="SphereMul";
					break;
				case eSPHEREMAP_ADD:
					techName+="SphereAdd";
					break;
				}
				pEffect->SetTexture( "g_SphereMapTexture" , pMaterial->textureSphere->GetTexture() );
			}

			D3DXCOLOR lightColorAmbient = renderInfo.lightColor;
			lightColorAmbient.r -= 0.3f;
			lightColorAmbient.g -= 0.3f;
			lightColorAmbient.b -= 0.3f;

			pEffect->SetValue( "g_lightColorDiffuse", &D3DXCOLOR( 0xFFFFFFFF ),sizeof(D3DXCOLOR) );
			pEffect->SetValue( "g_lightColorAmbient", &lightColorAmbient,sizeof(D3DXCOLOR) );
			pEffect->SetValue( "g_lightColorSpecular", &renderInfo.lightColor,sizeof(D3DXCOLOR) );
			pEffect->SetVector( "g_lightDir", &D3DXVECTOR4( vLight.x,vLight.y,vLight.z,1.0f) );
			pEffect->SetVector( "g_eyePos" , &D3DXVECTOR4(vEye.x,vEye.y,vEye.z,1.0f) );

			int cpass = 0;

			pEffect->SetTechnique( techName.c_str() );
		
			UINT numPass;
			pEffect->Begin( &numPass, 0 );
			pEffect->BeginPass( cpass );

			pMesh->Draw( attrIdx );

			pEffect->EndPass();
			pEffect->End();
		}
	}
}

void ModelRenderer::RenderNonShader()
{
	if( !m_pMeshContainer )
	{
		return;
	}

	Mesh* pMesh = m_pMeshContainer->pMesh;

	if( !pMesh )
	{
		return;
	}

	DWORD attrNum = pMesh->GetAttributeRangeNum();

	for( DWORD attrIdx = 0 ; attrIdx < attrNum ; attrIdx++ )
	{
		DWORD attrID = pMesh->GetAttributeID( attrIdx );

		pMesh->Draw( attrIdx );
	}
}