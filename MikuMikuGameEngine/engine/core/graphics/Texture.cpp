#include "stdafx.h"

#include "Texture.h"

#include "Graphics.h"

#include "../util/util.h"

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

VOID WINAPI FillTexColor(D3DXVECTOR4* pOut, CONST D3DXVECTOR2* pTexCoord,CONST D3DXVECTOR2* pTexelSize, LPVOID pData)
{
	D3DXCOLOR color = *(D3DXCOLOR*)pData;

	*pOut = D3DXVECTOR4(color.r, color.g, color.b, color.a);
}

bool Texture::CreateDotColor( D3DXCOLOR color )
{
	if( m_pTexture )
	{
		m_pTexture.Release();
	}

	Graphics* graphics = Graphics::GetInstance();
	IDirect3DDevice9Ptr d3dDevice = graphics->GetDirect3DDevice();

	HRESULT hr = D3DXCreateTexture( 
		d3dDevice,
		1, 1, 1, 0, 
		D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pTexture );

	if( FAILED(hr) )
	{
		return false;
	}

	//IDirect3DSurface9Ptr surface;
	//m_pTexture->GetSurfaceLevel(0, &surface);

	//d3dDevice->ColorFill( surface, NULL, color );

	D3DXFillTexture( m_pTexture,FillTexColor,(LPVOID)&color );
	
	return true;
}

bool Texture::CreateFromResource( int resourceID )
{
	if( m_pTexture )
	{
		m_pTexture.Release();
	}

	Graphics* graphics = Graphics::GetInstance();
	IDirect3DDevice9Ptr d3dDevice = graphics->GetDirect3DDevice();

	HRESULT hr = D3DXCreateTextureFromResource( d3dDevice, NULL, MAKEINTRESOURCE( resourceID ), &m_pTexture );

	if( FAILED(hr) )
	{
		return false;
	}

	return true;
}

IDirect3DTexture9Ptr Texture::GetTexture()
{
	return m_pTexture;
}
