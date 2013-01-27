#pragma once

#include "Model.h"
#include "Shader.h"

struct sRenderInfo;

class ModelRenderer
{
private:
	ModelPtr m_pModel;

public:
	ModelRenderer();
	virtual ~ModelRenderer();

public:
	void SetModel( ModelPtr pModel );
	
	DWORD GetMaterialNum();

	void RenderZPlot( const D3DXMATRIX& matWorld,const sRenderInfo& renderInfo );
	void Render( const D3DXMATRIX& matWorld,const sRenderInfo& renderInfo );
};
