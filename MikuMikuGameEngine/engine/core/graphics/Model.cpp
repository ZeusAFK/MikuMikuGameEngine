#include "stdafx.h"

#include "Model.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Model::Model( MeshContainer* pMeshContainer )
	: m_pMeshContainer(pMeshContainer)
{
}

Model::~Model()
{
	if( m_pMeshContainer )
	{
		delete m_pMeshContainer;
		m_pMeshContainer = NULL;
	}
}

MeshContainer* Model::GetMeshContainer()
{
	return m_pMeshContainer;
}
