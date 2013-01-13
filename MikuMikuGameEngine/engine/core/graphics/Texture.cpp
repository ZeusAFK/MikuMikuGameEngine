#include "stdafx.h"

#include "Texture.h"

#include "Graphics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Texture::Texture()
{
}

Texture::~Texture()
{
}

bool Texture::CreateFromFile( const tstring& filePath )
{
	if( m_pTexture )
	{
		m_pTexture.Release();
	}

	HRESULT hr = D3DXCreateTextureFromFile( 
		Graphics::GetInstance()->GetDirect3DDevice(),
		filePath.c_str(),
		&m_pTexture );

	return hr==D3D_OK;
}

IDirect3DTexture9Ptr Texture::GetTexture()
{
	return m_pTexture;
}