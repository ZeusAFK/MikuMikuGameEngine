#pragma once

#include "Model.h"

class XFileLoader
{
private:
	tstring m_fileName;
	tstring m_path;

	float m_scale;

	ModelPtr m_pModel;

public:
	XFileLoader();
	virtual ~XFileLoader();

private:
	HRESULT LoadMeshContainer( LPD3DXFILEDATA pXofData,MeshContainer* meshContainer ); 
	HRESULT LoadFrameNode( LPD3DXFILEDATA pXofData,ModelFrame* frame );

public:
	ModelPtr Open( const tstring& filePath,float scale );
};