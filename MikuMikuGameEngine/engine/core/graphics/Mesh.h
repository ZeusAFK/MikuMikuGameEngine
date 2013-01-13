#pragma once

struct sVertex
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	DWORD color;
	D3DXVECTOR2 uv;
};

class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

private:
	DWORD m_vertexNum;
	sVertex* m_pVertices;

	DWORD m_faceNum;
	DWORD* m_pIndices;

	DWORD m_attributeRangeNum;
	D3DXATTRIBUTERANGE* m_pAttributeRanges;

public:
	bool Create( DWORD vertexNum,DWORD faceNum,DWORD attributeRangeNum );

	void SetVertices( sVertex* pVertices );
	sVertex* GetVertices();

	void SetIndices( DWORD* indices );
	DWORD* GetIndices();

	void SetAttributeRanges( D3DXATTRIBUTERANGE* pAttributeRanges );
	D3DXATTRIBUTERANGE* GetAttributeRanges();

	DWORD GetVertexNum();
	DWORD GetFaceNum();
	DWORD GetAttributeRangeNum();

	DWORD GetAttributeID( DWORD attributeIndex );

	void Release();

public:
	void Draw( DWORD attributeIndex );
};