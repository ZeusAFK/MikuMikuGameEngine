#pragma once

#include "core/graphics/shader.h"

class CoordinateAxis
{
private:
	struct sVertex
	{
		D3DXVECTOR3 position;
		D3DCOLOR color;
	};

	std::vector<sVertex> m_lines;
	sVertex m_vertices[4];

	ShaderPtr m_editorShader;

public:
	CoordinateAxis();
	virtual ~CoordinateAxis();

public:
	void Render( const D3DXMATRIX& matView,const D3DXMATRIX& matProj );
};