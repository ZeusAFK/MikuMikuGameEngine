#pragma once

#include "Graphics.h"

#include "../ResourceManager.h"

class Texture : public UnmanagedResource , public IResource
{
private:
	IDirect3DTexture9Ptr m_pTexture;

public:
	Texture();
	~Texture();

public:
	bool CreateFromFile( const tstring& filePath );
	bool CreateDotColor( D3DXCOLOR color );

	IDirect3DTexture9Ptr GetTexture();

private:
	D3DSURFACE_DESC m_LostDesc;
	BYTE* m_pLostSaveBuffer;

public:
	virtual void OnLostDevice();
	virtual void OnResetDevice();
};

typedef std::tr1::shared_ptr< Texture > TexturePtr;