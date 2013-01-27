#pragma once

#include "TextureAtlas.h"

struct sRenderInfo;

class UISpriteRenderer
{
private:
	TextureAtlasPtr m_pAtlas;
	tstring_symbol m_spriteName;
	const sChipInfo* m_spriteChip;

	D3DXCOLOR m_color;

public:
	UISpriteRenderer();
	virtual ~UISpriteRenderer();

public:
	void SetAtlas( TextureAtlasPtr pAtlas );
	void SetSpriteName( const tstring_symbol& spriteName );
	void SetColor( D3DXCOLOR color );
	
	void Render( float x,float y );
};
