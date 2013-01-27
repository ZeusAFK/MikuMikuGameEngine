#pragma once

#include "Texture.h"
#include "../util/CharSet.h"

struct sChipInfo
{
	int x;
	int y;
	int width;
	int height;
	int cx;
	int cy;
};

class TextureAtlas : public IResource
{
private:
	TexturePtr m_texture;
	std::map<tstring_symbol,sChipInfo*> m_chipList;

public:
	TextureAtlas();
	virtual ~TextureAtlas();

private:
	bool Load( IXmlReaderPtr reader );

public:
	bool CreateFromFile( const tstring& filePath,TexturePtr texture );
	bool CreateFromMemory(  const BYTE* data,DWORD size,TexturePtr texture );
	bool CreateFromResource( int resourceID,TexturePtr texture );
	const sChipInfo* GetChipInfo( const tstring_symbol& name );
	TexturePtr GetTexture();
};

typedef std::tr1::shared_ptr< TextureAtlas > TextureAtlasPtr;