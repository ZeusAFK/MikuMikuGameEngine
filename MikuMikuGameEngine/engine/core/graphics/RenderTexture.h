#pragma once

#include "Graphics.h"

class RenderTexture : public UnmanagedResource
{
private:
	IDirect3DTexture9Ptr m_pTexture;
	IDirect3DSurface9Ptr m_pTextureSurface;
	IDirect3DSurface9Ptr m_pDepthBuffer;

	DWORD m_width;
	DWORD m_height;
	D3DFORMAT m_format;

public:
	RenderTexture();
	virtual ~RenderTexture();

private:
	void Clear();

public:
	bool Create( DWORD width,DWORD height,D3DFORMAT format );

	void SetRenderTarget();

	IDirect3DTexture9Ptr GetTexture();

public:
	virtual void OnLostDevice();
	virtual void OnResetDevice();
};

typedef std::tr1::shared_ptr< RenderTexture > RenderTexturePtr;