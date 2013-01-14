#pragma once

#include "Model.h"
#include "Shader.h"

class ModelRenderer
{
private:
	ModelPtr m_pModel;
	MeshContainer* m_pMeshContainer;

	ShaderPtr* m_ppShaders;

public:
	ModelRenderer();
	virtual ~ModelRenderer();

public:
	void SetModel( MeshContainer* pMeshContainer );
	
	DWORD GetMaterialNum();
	void SetShader( DWORD materialIndex,ShaderPtr shader );

	void Render( const D3DXMATRIX& matWorld,const D3DXMATRIX& matView,const D3DXMATRIX& matProj,const D3DXVECTOR3& eyePos,const D3DXVECTOR3& lightDir,const D3DXCOLOR& lightColor );

	void RenderNonShader();
};
