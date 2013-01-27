#include "stdafx.h"

#include "UISpriteRenderer.h"

#include "../../GameObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UISpriteRenderer::UISpriteRenderer()
	: m_spriteChip(NULL)
	, m_color( 1.0f,1.0f,1.0f,1.0f )
{
}

UISpriteRenderer::~UISpriteRenderer()
{
}

void UISpriteRenderer::SetAtlas( TextureAtlasPtr pAtlas )
{
	m_pAtlas = pAtlas;

	if( m_pAtlas )
	{
		m_spriteChip = m_pAtlas->GetChipInfo( m_spriteName );
	}
}

void UISpriteRenderer::SetSpriteName( const tstring_symbol& spriteName )
{
	m_spriteName = spriteName;

	if( m_pAtlas )
	{
		m_spriteChip = m_pAtlas->GetChipInfo( m_spriteName );
	}
}

void UISpriteRenderer::SetColor( D3DXCOLOR color )
{
	m_color = color;
}

void UISpriteRenderer::Render( float x,float y )
{
	if( !m_pAtlas || !m_pAtlas->GetTexture() || !m_spriteChip )
	{
		return;
	}

	Graphics* graphics = Graphics::GetInstance();

	graphics->SetRenderState( D3DRS_CULLMODE,D3DCULL_NONE );

	ShaderPtr pDefaultShader = graphics->GetDefaultShader();

	if( pDefaultShader )
	{
		ID3DXEffectPtr pEffect = pDefaultShader->GetEffect();
	
		IDirect3DTexture9Ptr texture = m_pAtlas->GetTexture()->GetTexture();
		D3DSURFACE_DESC desc;
		texture->GetLevelDesc( 0,&desc );

		struct sUIVertex
		{
			float x;
			float y;
			float z;
			float rhw;
			DWORD color;
			float u;
			float v;
		};

		DWORD color = m_color;

		float sx = (float)m_spriteChip->x;
		float sy = (float)m_spriteChip->y;
		float sw = (float)m_spriteChip->width;
		float sh = (float)m_spriteChip->height;

		float texW = (float)desc.Width;
		float texH = (float)desc.Height;

		float left = x;
		float right = left+sw;
		float top = y;
		float bottom = y+sh;

		float u0 = sx/texW;
		float u1 = (sx+sw)/texW;
		float v0 = sy/texH;
		float v1 = (sy+sh)/texH;

		sUIVertex vertex[4]={
			{  left,   top,0.0f,1.0f,color,u0,v0},
			{ right,   top,0.0f,1.0f,color,u1,v0},
			{  left,bottom,0.0f,1.0f,color,u0,v1},
			{ right,bottom,0.0f,1.0f,color,u1,v1},
		};

		pEffect->SetTechnique( "TechUserInterface2D" );

		pEffect->SetTexture( "g_Texture",texture );
	
		UINT numPass;
		pEffect->Begin( &numPass, 0 );
		pEffect->BeginPass( 0 );

		graphics->SetFVF( D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1 );
		graphics->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP,2,vertex,sizeof(sUIVertex) );

		pEffect->EndPass();
		pEffect->End();
	}
}