#pragma once

#include "Graphics.h"

#include "../ResourceManager.h"

class Texture : public IResource
{
private:
	IDirect3DTexture9Ptr m_pTexture;

public:
	Texture();
	~Texture();

public:
	bool CreateFromFile( const tstring& filePath );
	bool CreateDotColor( D3DXCOLOR color );
	bool CreateFromResource( int resourceID );

	IDirect3DTexture9Ptr GetTexture();
};

typedef std::tr1::shared_ptr< Texture > TexturePtr;