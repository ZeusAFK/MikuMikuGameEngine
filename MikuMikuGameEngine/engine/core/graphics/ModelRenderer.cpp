#include "stdafx.h"

#include "ModelRenderer.h"

#include "../../GameObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ModelRenderer::ModelRenderer()
{
}

ModelRenderer::~ModelRenderer()
{
}

void ModelRenderer::SetModel( ModelPtr pModel )
{
	m_pModel = pModel;
}

ModelPtr ModelRenderer::GetModel()
{
	return m_pModel;
}

void ModelRenderer::SetAssetUUID( const tstring_symbol& assetUUID )
{
	m_assetUUID = assetUUID;
}

const tstring_symbol& ModelRenderer::GetAssetUUID() const
{
	return m_assetUUID;
}

DWORD ModelRenderer::GetMaterialNum()
{
	if( m_pModel && m_pModel->GetMeshContainer() )
	{
		return m_pModel->GetMeshContainer()->materialNum;
	}

	return 0;
}

void ModelRenderer::RenderZPlot( const D3DXMATRIX& matWorld,const sRenderInfo& renderInfo )
{
	if( !m_pModel || !m_pModel->GetMeshContainer() )
	{
		return;
	}

	MeshContainer* pMeshContainer = m_pModel->GetMeshContainer();

	Mesh* pMesh = pMeshContainer->pMesh;

	if( !pMesh )
	{
		return;
	}

	D3DXMATRIX matShadowWorldViewProj = matWorld * renderInfo.matShadow;

	DWORD attrNum = pMesh->GetAttributeRangeNum();

	Graphics* graphics = Graphics::GetInstance();

	graphics->SetRenderState( D3DRS_CULLMODE,D3DCULL_CCW );

	ShaderPtr pDefaultShader = graphics->GetDefaultShader();

	for( DWORD attrIdx = 0 ; attrIdx < attrNum ; attrIdx++ )
	{
		DWORD attrID = pMesh->GetAttributeID( attrIdx );

		sMaterial* pMaterial = &pMeshContainer->pMaterials[ attrID ];

		if( pDefaultShader && pMaterial->colorDiffuse.a != 0.98f )
		{
			ID3DXEffectPtr pEffect = pDefaultShader->GetEffect();
		
			pEffect->SetMatrix( "g_mShadowWorldViewProjection",&matShadowWorldViewProj );

			int cpass = 0;

			pEffect->SetTechnique( "TechZPlot" );
		
			UINT numPass;
			pEffect->Begin( &numPass, 0 );
			pEffect->BeginPass( cpass );

			pMesh->Draw( attrIdx );

			pEffect->EndPass();
			pEffect->End();
		}
	}
}

void ModelRenderer::Render( const D3DXMATRIX& matWorld,const sRenderInfo& renderInfo )
{
	if( !m_pModel || !m_pModel->GetMeshContainer() )
	{
		return;
	}

	MeshContainer* pMeshContainer = m_pModel->GetMeshContainer();

	Mesh* pMesh = pMeshContainer->pMesh;

	if( !pMesh )
	{
		return;
	}

	const D3DXMATRIX& matView = renderInfo.camera.GetViewMatrix();
	const D3DXMATRIX& matProj = renderInfo.camera.GetProjMatrix();

	D3DXMATRIX matWorldView = matWorld * matView;
	D3DXMATRIX matWorldViewProj = matWorldView * matProj;

	D3DXMATRIX matShadowWorldViewProj = matWorld * renderInfo.matShadow;

	DWORD attrNum = pMesh->GetAttributeRangeNum();

	Graphics* graphics = Graphics::GetInstance();

	graphics->SetRenderState( D3DRS_CULLMODE,D3DCULL_CCW );

	ShaderPtr pDefaultShader = graphics->GetDefaultShader();

	D3DXVECTOR3 vLight = renderInfo.lightDir;
	D3DXVECTOR3 vEye = renderInfo.camera.GetPosition();
	//{
	//	D3DXMATRIX matWorldInv;
	//	D3DXMatrixInverse( &matWorldInv,NULL,&matWorld );

	//	D3DXVec3TransformNormal( &vLight,&vLight,&matView );
	//	vLight = -vLight;

	//	D3DXVec3TransformCoord( &vEye,&vEye,&matWorldInv );
	//}

	for( DWORD attrIdx = 0 ; attrIdx < attrNum ; attrIdx++ )
	{
		DWORD attrID = pMesh->GetAttributeID( attrIdx );

		sMaterial* pMaterial = &pMeshContainer->pMaterials[ attrID ];

		if( pDefaultShader )
		{
			ID3DXEffectPtr pEffect = pDefaultShader->GetEffect();
		
			pEffect->SetMatrix( "g_mWorldViewProjection",&matWorldViewProj );
			pEffect->SetMatrix( "g_mWorldView",&matWorldView );
			pEffect->SetMatrix( "g_mWorld",&matWorld );
			pEffect->SetMatrix( "g_mView",&matView );

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
			if( renderInfo.shadowMap )
			{
				techName += "Shadow";
				pEffect->SetTexture( "g_ShadowMapTexture" , renderInfo.shadowMapTexture->GetTexture() );

				pEffect->SetMatrix( "g_mShadowWorldViewProjection",&matShadowWorldViewProj );
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