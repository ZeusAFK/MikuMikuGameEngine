#pragma once

#include "Model.h"

class XFileLoader
{
private:
	tstring m_fileName;
	tstring m_path;

	float m_scale;

	LPD3DXBUFFER m_pAdjacencyBuf;
	LPD3DXBUFFER m_pMaterialBuf;
	LPD3DXBUFFER m_pEffectInstancesBuf;
	DWORD m_Materials;
	LPD3DXMESH m_pD3DMesh;

	ModelPtr m_pModel;

public:
	XFileLoader();
	virtual ~XFileLoader();

protected:
	MeshContainer* CreateMeshContainer();

public:
	ModelPtr Open( const tstring& filePath,float scale );
	ModelPtr OpenFromResource( int resourceID,float scale );
};