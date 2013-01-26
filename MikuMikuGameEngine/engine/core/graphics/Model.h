#pragma once

#include "Mesh.h"
#include "Texture.h"

#include "Material.h"

#include "../ResourceManager.h"

struct MeshContainer
{
public:
	Mesh* pMesh;

	DWORD materialNum;
	sMaterial* pMaterials;

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

class Model : public IResource
{
private:
	MeshContainer* m_pMeshContainer;

public:
	Model( MeshContainer* pMeshContainer );
	virtual ~Model();

public:
	MeshContainer* GetMeshContainer();
};

typedef std::tr1::shared_ptr< Model > ModelPtr;
typedef std::tr1::weak_ptr< Model > ModelWeakPtr;