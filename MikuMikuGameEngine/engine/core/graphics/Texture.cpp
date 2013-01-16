#include "stdafx.h"

#include "Texture.h"

#include "Graphics.h"

#include "../util/util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Texture::Texture()
	: m_pLostSaveBuffer(NULL)
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
		16, 16, 1, D3DUSAGE_DYNAMIC, 
		D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTexture );

	if( FAILED(hr) )
	{
		return false;
	}

	//IDirect3DSurface9Ptr surface;
	//m_pTexture->GetSurfaceLevel(0, &surface);

	//d3dDevice->ColorFill( surface, NULL, color );

	D3DXFillTexture( m_pTexture,FillTexColor,(LPVOID)&color );
	
	graphics->AddResource( this );

	return true;
}

IDirect3DTexture9Ptr Texture::GetTexture()
{
	return m_pTexture;
}

void Texture::OnLostDevice()
{
	// TODO:OnLostDevice
	if( !m_pTexture )
	{
		return;
	}

	OutputDebugStringFormat( _T("Texture(%p)::OnLostDevice\n"),this );

	m_pTexture->GetLevelDesc(0,&m_LostDesc);

	if( m_LostDesc.Pool==D3DPOOL_DEFAULT )
	{
		if( m_LostDesc.Usage & D3DUSAGE_DYNAMIC )
		{
			D3DLOCKED_RECT lock;
			m_pTexture->LockRect(0,&lock,NULL,0);

			size_t size=lock.Pitch*m_LostDesc.Height;

			m_pLostSaveBuffer=new BYTE[size];

			memcpy(m_pLostSaveBuffer,lock.pBits,size);

			m_pTexture->UnlockRect(0);
		}

		m_pTexture.Release();
	}
}

void Texture::OnResetDevice()
{
	// TODO:OnResetDevice
	if( m_pLostSaveBuffer==NULL )
	{
		return;
	}

	OutputDebugStringFormat( _T("Texture(%p)::OnResetDevice\n"),this );

	if( m_LostDesc.Pool==D3DPOOL_DEFAULT )
	{
		Graphics* graphics = Graphics::GetInstance();
		IDirect3DDevice9Ptr pD3DDevice = graphics->GetDirect3DDevice();
		
		HRESULT hr;
		hr = D3DXCreateTexture( 
			pD3DDevice,
			m_LostDesc.Width, m_LostDesc.Height, 1, m_LostDesc.Usage, 
			m_LostDesc.Format, m_LostDesc.Pool,&m_pTexture );
		//hr=pD3DDevice->CreateTexture(m_LostDesc.Width,m_LostDesc.Height,m_LostDesc.MultiSampleQuality,m_LostDesc.Usage,m_LostDesc.Format,m_LostDesc.Pool,&m_pTexture,NULL);
		if( FAILED(hr) )
		{
			return;
		}

		if( m_LostDesc.Usage & D3DUSAGE_DYNAMIC )
		{
			D3DLOCKED_RECT lock;
			m_pTexture->LockRect(0,&lock,NULL,0);

			size_t size=lock.Pitch*m_LostDesc.Height;
			memcpy(lock.pBits,m_pLostSaveBuffer,size);

			m_pTexture->UnlockRect(0);

			delete[] m_pLostSaveBuffer;
			m_pLostSaveBuffer = NULL;
		}
	}
}