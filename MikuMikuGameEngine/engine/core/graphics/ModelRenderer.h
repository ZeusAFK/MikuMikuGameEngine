#pragma once

#include "Model.h"
#include "Shader.h"

struct sRenderInfo;

class ModelRenderer
{
private:
	ModelPtr m_pModel;
	MeshContainer* m_pMeshContainer;

public:
	ModelRenderer();
	virtual ~ModelRenderer();

public:
	void SetModel( MeshContainer* pMeshContainer );
	
	DWORD GetMaterialNum();

	void Render( const D3DXMATRIX& matWorld,const sRenderInfo& renderInfo );

	void RenderNonShader();
};
