#pragma once

#include "Mesh.h"
#include "Texture.h"

#include "../util/TreeNode.h"

class Model;
typedef std::tr1::weak_ptr< Model > ModelWeakPtr;

struct sMaterial
{
	D3DXCOLOR colorDiffuse;
	TexturePtr textureDiffuse;

public:
	sMaterial()
		: colorDiffuse( 0xFFFFFFFF )
	{
	}
};

struct MeshContainer
{
public:
	Mesh* pMesh;

	DWORD materialNum;
	sMaterial* pMaterials;

	ModelWeakPtr pModel;

public:
	MeshContainer()
		: pMesh(NULL)
		, materialNum(0)
		, pMaterials(NULL)
	{
	}

	virtual ~MeshContainer()
	{
		if( pMesh )
		{
			delete pMesh;
			pMesh=NULL;
		}
		materialNum = 0;
		if( pMaterials )
		{
			delete[] pMaterials;
			pMaterials = NULL;
		}
	}
};

class ModelFrame : public TreeNode<ModelFrame>
{
private:
	tstring m_name;

	MeshContainer* m_pMeshContainer;

	D3DXMATRIX m_transform;

public:
	ModelFrame();
	virtual ~ModelFrame();

public:
	void SetName( const tstring& name );
	void SetMeshContainer( MeshContainer* pMeshContainer );
	void SetTransform( const D3DXMATRIX& transform );

public:
	const tstring& GetName();
	MeshContainer* GetMeshContainer();
	const D3DXMATRIX& GetTransform();
};