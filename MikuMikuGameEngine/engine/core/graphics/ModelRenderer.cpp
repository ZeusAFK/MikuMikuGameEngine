#include "stdafx.h"

#include "ModelRenderer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ModelRenderer::ModelRenderer()
	: m_pMeshContainer(NULL)
	, m_ppShaders(NULL)
{
}

ModelRenderer::~ModelRenderer()
{
	m_pMeshContainer = NULL;
	if( m_ppShaders )
	{
		delete[] m_ppShaders;
		m_ppShaders = NULL;
	}
}

void ModelRenderer::SetModel( MeshContainer* pMeshContainer )
{
	m_pMeshContainer = pMeshContainer;
	if( m_pMeshContainer )
	{
		m_pModel = m_pMeshContainer->pModel.lock();
	}

	if( m_pMeshContainer )
	{
		m_ppShaders = new ShaderPtr[ m_pMeshContainer->materialNum ];
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

void ModelRenderer::SetShader( DWORD materialIndex,ShaderPtr shader )
{
	if( GetMaterialNum() > materialIndex )
	{
		m_ppShaders[materialIndex] = shader;
	}
}

void ModelRenderer::Render( const D3DXMATRIX& matWorldViewProj )
{
	if( !m_pMeshContainer )
	{
		return;
	}

	Mesh* pMesh = m_pMeshContainer->pMesh;

	if( !pMesh || !m_ppShaders )
	{
		return;
	}

	DWORD attrNum = pMesh->GetAttributeRangeNum();

	for( DWORD attrIdx = 0 ; attrIdx < attrNum ; attrIdx++ )
	{
		DWORD attrID = pMesh->GetAttributeID( attrIdx );

		sMaterial* pMaterial = &m_pMeshContainer->pMaterials[ attrID ];
		ShaderPtr pShader = m_ppShaders[attrID];

		if( pShader )
		{
			ID3DXEffectPtr pEffect = pShader->GetEffect();
		
			pEffect->SetMatrix( "matWorldViewProj", &matWorldViewProj );
			
			pEffect->SetValue( "colorDiffuse",&pMaterial->colorDiffuse,sizeof(D3DXCOLOR) );
			if( pMaterial->textureDiffuse )
			{
				pEffect->SetTexture( "textureDiffuse",pMaterial->textureDiffuse->GetTexture() );
			}
			else
			{
				pEffect->SetTexture( "textureDiffuse",NULL );
			}
			
			pEffect->SetTechnique( "Tec_Default" );
			UINT numPass;
			pEffect->Begin( &numPass, 0 );
			pEffect->BeginPass(0);

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