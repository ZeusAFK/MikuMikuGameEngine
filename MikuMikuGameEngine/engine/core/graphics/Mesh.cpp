#include "stdafx.h"

#include "Graphics.h"

#include "Mesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Mesh::Mesh()
	: m_vertexNum(0)
	, m_pVertices(NULL)
	, m_faceNum(0)
	, m_pIndices(0)
	, m_attributeRangeNum(0)
	, m_pAttributeRanges(NULL)
{
}

Mesh::~Mesh()
{
	Release();
}

void Mesh::Release()
{
	if( m_pVertices )
	{
		delete[] m_pVertices;
		m_pVertices = NULL;
	}
	if( m_pIndices )
	{
		delete[] m_pIndices;
		m_pIndices = NULL;
	}
	if( m_pAttributeRanges )
	{
		delete[] m_pAttributeRanges;
		m_pAttributeRanges = NULL;
	}
}

bool Mesh::Create( DWORD vertexNum,DWORD faceNum,DWORD attributeRangeNum )
{
	Release();

	m_vertexNum = vertexNum;
	m_faceNum = faceNum;
	m_attributeRangeNum = attributeRangeNum;

	if( m_vertexNum == 0 || m_faceNum == 0 || m_attributeRangeNum == 0 )
	{
		return false;
	}
	
	m_pVertices = new sVertex[m_vertexNum];
	ZeroMemory( m_pVertices,sizeof(sVertex)*m_vertexNum );

	m_pIndices = new DWORD[m_faceNum*3];
	ZeroMemory( m_pIndices,sizeof(DWORD)*m_faceNum*3 );

	m_pAttributeRanges = new D3DXATTRIBUTERANGE[m_attributeRangeNum];
	ZeroMemory( m_pAttributeRanges,sizeof(D3DXATTRIBUTERANGE)*m_attributeRangeNum );

	return true;
}

void Mesh::SetVertices( sVertex* pVertices )
{
	if( m_pVertices )
	{
		memcpy( m_pVertices,pVertices,sizeof(sVertex)*m_vertexNum );
	}
}

sVertex* Mesh::GetVertices()
{
	return m_pVertices;
}

void Mesh::SetIndices( DWORD* indices )
{
	if( m_pIndices )
	{
		memcpy( m_pIndices,indices,sizeof(DWORD)*m_faceNum*3 );
	}
}

DWORD* Mesh::GetIndices()
{
	return m_pIndices;
}

void Mesh::SetAttributeRanges( D3DXATTRIBUTERANGE* pAttributeRanges )
{
	if( m_pAttributeRanges )
	{
		memcpy( m_pAttributeRanges,pAttributeRanges,sizeof(D3DXATTRIBUTERANGE)*m_attributeRangeNum );
	}
}

D3DXATTRIBUTERANGE* Mesh::GetAttributeRanges()
{
	return m_pAttributeRanges;
}

DWORD Mesh::GetVertexNum()
{
	return m_vertexNum;
}

DWORD Mesh::GetFaceNum()
{
	return m_faceNum;
}

DWORD Mesh::GetAttributeRangeNum()
{
	return m_attributeRangeNum;
}

DWORD Mesh::GetAttributeID( DWORD attributeIndex )
{
	if( attributeIndex < m_attributeRangeNum && m_pAttributeRanges )
	{
		return m_pAttributeRanges[attributeIndex].AttribId;
	}
	return 0;
}

void Mesh::Draw( DWORD attributeIndex )
{
	if( attributeIndex < m_attributeRangeNum && m_pVertices && m_pIndices && m_pAttributeRanges )
	{
		Graphics* graphics = Graphics::GetInstance();

		D3DXATTRIBUTERANGE* pAttributeRange = &m_pAttributeRanges[attributeIndex];

		graphics->SetFVF( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		graphics->DrawIndexedPrimitiveUP( 
			D3DPT_TRIANGLELIST,
			pAttributeRange->VertexStart,pAttributeRange->VertexCount,
			pAttributeRange->FaceCount,
			&m_pIndices[pAttributeRange->FaceStart*3],
			D3DFMT_INDEX32,
			m_pVertices,sizeof(sVertex) 
		);
	}
}
