#include "stdafx.h"

#include "ModelFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ModelFrame::ModelFrame()
	: m_pMeshContainer(NULL)
{
	D3DXMatrixIdentity( &m_transform );
}

ModelFrame::~ModelFrame()
{
	if( m_pMeshContainer )
	{
		delete m_pMeshContainer;
	}
}

void ModelFrame::SetName( const tstring& name )
{
	m_name = name;
}

void ModelFrame::SetMeshContainer( MeshContainer* pMeshContainer )
{
	m_pMeshContainer = pMeshContainer;
}

void ModelFrame::SetTransform( const D3DXMATRIX& transform )
{
	m_transform = transform;
}

const tstring& ModelFrame::GetName()
{
	return m_name;
}

MeshContainer* ModelFrame::GetMeshContainer()
{
	return m_pMeshContainer;
}

const D3DXMATRIX& ModelFrame::GetTransform()
{
	return m_transform;
}
