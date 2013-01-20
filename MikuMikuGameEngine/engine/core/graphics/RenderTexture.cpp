#include "stdafx.h"

#include "RenderTexture.h"

RenderTexture::RenderTexture()
{
}

RenderTexture::~RenderTexture()
{
	Graphics::GetInstance()->DelResource( this );
}

void RenderTexture::Clear()
{
	if( m_pTextureSurface )
	{
		m_pTextureSurface.Release();
	}
	if( m_pTexture )
	{
		m_pTexture.Release();
	}
	if( m_pDepthBuffer )
	{
		m_pDepthBuffer.Release();
	}
}

bool RenderTexture::Create( DWORD width,DWORD height,D3DFORMAT format )
{
	Clear();

	IDirect3DTexture9Ptr pTexture;
	IDirect3DSurface9Ptr pTextureSurface;
	IDirect3DSurface9Ptr pDepthBuffer;

	m_width = width;
	m_height = height;
	m_format = format;

	Graphics* graphics = Graphics::GetInstance();
	IDirect3DDevice9Ptr d3dDevice = graphics->GetDirect3DDevice();

	HRESULT hr = D3DXCreateTexture( 
		d3dDevice,
		m_width, m_height, 1, D3DUSAGE_RENDERTARGET, 
		m_format, D3DPOOL_DEFAULT, &pTexture );

	if( FAILED(hr) )
	{
		return false;
	}

	pTexture->GetSurfaceLevel( 0, &pTextureSurface );

	hr = d3dDevice->CreateDepthStencilSurface( 
		m_width, m_height,
		D3DFMT_D24X8,D3DMULTISAMPLE_NONE,0,
		TRUE,&pDepthBuffer,NULL );

	if( FAILED(hr) )
	{
		return false;
	}

	m_pTexture = pTexture;
	m_pTextureSurface = pTextureSurface;
	m_pDepthBuffer = pDepthBuffer;

	Graphics::GetInstance()->AddResource( this );

	return true;
}

void RenderTexture::SetRenderTarget()
{
	Graphics* graphics = Graphics::GetInstance();
	IDirect3DDevice9Ptr pD3DDevice = graphics->GetDirect3DDevice();

	D3DVIEWPORT9 viewport = { 0,0,
		m_width,m_height,
		0.0f,1.0f };

	pD3DDevice->SetRenderTarget( 0,m_pTextureSurface );
	pD3DDevice->SetDepthStencilSurface( m_pDepthBuffer );
	pD3DDevice->SetViewport( &viewport );
}

IDirect3DTexture9Ptr RenderTexture::GetTexture()
{
	return m_pTexture;
}

void RenderTexture::OnLostDevice()
{
	Clear();
}

void RenderTexture::OnResetDevice()
{
	Create( m_width,m_height,m_format );
}