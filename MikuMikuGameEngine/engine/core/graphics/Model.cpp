#include "stdafx.h"

#include "Model.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Model::Model( ModelFrame* pModelFrame )
	: m_pModelFrame(pModelFrame)
{
}

Model::~Model()
{
	if( m_pModelFrame )
	{
		delete m_pModelFrame;
		m_pModelFrame = NULL;
	}
}

ModelFrame* Model::GetFrame()
{
	return m_pModelFrame;
}

void SetModelRef( ModelPtr pModel,ModelFrame* pModelFrame )
{
	MeshContainer* pMeshContainer = pModelFrame->GetMeshContainer();
	if( pMeshContainer )
	{
		pMeshContainer->pModel = pModel;
	}

	ModelFrame* childFrame = pModelFrame->GetChild();
	if( childFrame )
	{
		SetModelRef( pModel,childFrame );
	}

	ModelFrame* nextFrame = pModelFrame->GetSiblingNext();
	if( nextFrame )
	{
		SetModelRef( pModel,nextFrame );
	}
}