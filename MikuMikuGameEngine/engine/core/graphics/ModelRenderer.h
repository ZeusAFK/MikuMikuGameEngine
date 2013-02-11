#pragma once

#include "Model.h"
#include "Shader.h"

struct sRenderInfo;

class ModelRenderer
{
private:
	ModelPtr m_pModel;
	tstring_symbol m_assetUUID;

public:
	ModelRenderer();
	virtual ~ModelRenderer();

public:
	void SetModel( ModelPtr pModel );
	ModelPtr GetModel();
	void SetAssetUUID( const tstring_symbol& assetUUID );
	const tstring_symbol& GetAssetUUID() const;
	
	DWORD GetMaterialNum();

	void RenderZPlot( const D3DXMATRIX& matWorld,const sRenderInfo& renderInfo );
	void Render( const D3DXMATRIX& matWorld,const sRenderInfo& renderInfo );
};
