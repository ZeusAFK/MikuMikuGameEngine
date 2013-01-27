#pragma once

#include "Mesh.h"

#include "PMDModel.h"
#include "PMDBone.h"
#include "Shader.h"

class GameObject;
struct sRenderInfo;

class PMDModelRenderer
{
private:
	GameObject* m_gameObject;

	PMDModelPtr m_pModel;
	Mesh* m_pMesh;
	Mesh* m_pEdgeMesh;

	PMDBone** m_ppBoneList;
	D3DXMATRIX* m_pOffsetMatrices;

	DWORD m_rootBoneCount;
	PMDBone** m_ppRootBoneList;

	struct SkinWeight
	{
		float weight;
		bool changed;
	};
	
	SkinWeight* m_skinWeights;

public:
	PMDModelRenderer();
	virtual ~PMDModelRenderer();

public:
	void SetGameObject( GameObject* gameObject );
	void SetModel( PMDModelPtr pModel );
	
	DWORD GetMaterialNum();

	DWORD GetBoneNum();
	PMDBone* GetBone( DWORD index );
	PMDBone* SearchBone( const tstring_symbol& name );

	DWORD GetSkinNum();
	int SearchSkin( const tstring_symbol& name );
	void SetSkinWeight( int index,float weight );

	void UpdateBone();

	void UpdateMesh( const D3DXMATRIX& matWorld,const sRenderInfo& renderInfo );

	void RenderZPlot( const D3DXMATRIX& matWorld,const sRenderInfo& renderInfo );
	void Render( const D3DXMATRIX& matWorld,const sRenderInfo& renderInfo );
};
