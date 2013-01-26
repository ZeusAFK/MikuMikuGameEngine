#include "stdafx.h"

#include "CoordinateAxis.h"

CoordinateAxis::CoordinateAxis()
{
	m_lines.clear();
	for( int i=0;i<21;i++ )
	{
		sVertex vertex;
		vertex.position = D3DXVECTOR3( (i-10)*5.0f,0.0f,-10*5.0f );
		vertex.color = 0xFFB4B4B4;
		m_lines.push_back( vertex );

		vertex.position = D3DXVECTOR3( (i-10)*5.0f,0.0f,10*5.0f );
		vertex.color = 0xFFB4B4B4;
		m_lines.push_back( vertex );
	}

	for( int i=0;i<21;i++ )
	{
		sVertex vertex;
		vertex.position = D3DXVECTOR3( -10*5.0f,0.0f,(i-10)*5.0f );
		vertex.color = 0xFFB4B4B4;
		m_lines.push_back( vertex );

		vertex.position = D3DXVECTOR3( 10*5.0f,0.0f,(i-10)*5.0f );
		vertex.color = 0xFFB4B4B4;
		m_lines.push_back( vertex );
	}

	sVertex vertex;

	// X
	vertex.position = D3DXVECTOR3(0.0f,0.0f,0.0f);
	vertex.color = 0xFFFF0000;
	m_lines.push_back( vertex );

	vertex.position = D3DXVECTOR3(65.0f,0.0f,0.0f);
	vertex.color = 0xFFFF0000;
	m_lines.push_back( vertex );

	// Y
	vertex.position = D3DXVECTOR3(0.0f,0.0f,0.0f);
	vertex.color = 0xFF00FF00;
	m_lines.push_back( vertex );

	vertex.position = D3DXVECTOR3(0.0f,65.0f,0.0f);
	vertex.color = 0xFF00FF00;
	m_lines.push_back( vertex );

	// Z
	vertex.position = D3DXVECTOR3(0.0f,0.0f,0.0f);
	vertex.color = 0xFF0000FF;
	m_lines.push_back( vertex );

	vertex.position = D3DXVECTOR3(0.0f,0.0f,-65.0f);
	vertex.color = 0xFF0000FF;
	m_lines.push_back( vertex );

	m_vertices[0].position = D3DXVECTOR3(-50.0f,0.0f,50.0f);
	m_vertices[0].color = 0xFFFFFFFF;
	m_vertices[1].position = D3DXVECTOR3(50.0f,0.0f,50.0f);
	m_vertices[1].color = 0xFFFFFFFF;
	m_vertices[2].position = D3DXVECTOR3(-50.0f,0.0f,-50.0f);
	m_vertices[2].color = 0xFFFFFFFF;
	m_vertices[3].position = D3DXVECTOR3(50.0f,0.0f,-50.0f);
	m_vertices[3].color = 0xFFFFFFFF;
}

CoordinateAxis::~CoordinateAxis()
{
}

void CoordinateAxis::Render( const D3DXMATRIX& matView,const D3DXMATRIX& matProj )
{
	Graphics* graphics = Graphics::GetInstance();

	graphics->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );

	D3DXMATRIX matWorldViewProj = matView * matProj;

	ShaderPtr pShader = graphics->GetDefaultShader();

	ID3DXEffectPtr pEffect = pShader->GetEffect();

	pEffect->SetMatrix( "g_mWorldViewProjection", &matWorldViewProj );

	pEffect->SetTechnique( "TechVertexColorUnlit" );
	UINT numPass;
	pEffect->Begin( &numPass, 0 );

	pEffect->BeginPass(0);

	graphics->DrawPrimitiveUP( D3DPT_LINELIST,m_lines.size()/2,&m_lines[0],sizeof(sVertex) );

	pEffect->EndPass();
	
	graphics->SetRenderState( D3DRS_COLORWRITEENABLE,0 );

	pEffect->BeginPass(0);

	graphics->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP,2,&m_vertices[0],sizeof(sVertex) );

	pEffect->EndPass();

	graphics->SetRenderState( D3DRS_COLORWRITEENABLE,0xF );

	pEffect->End();

}