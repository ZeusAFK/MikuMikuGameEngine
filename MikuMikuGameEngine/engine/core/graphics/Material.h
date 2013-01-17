#pragma once

enum eSPHEREMAP
{
	eSPHEREMAP_MUL,
	eSPHEREMAP_ADD,
};

struct sMaterial
{
	D3DXCOLOR colorDiffuse;
	D3DXCOLOR colorSpecular;
	D3DXCOLOR colorAmbient;
	D3DXCOLOR colorEmissive;
	D3DXCOLOR colorToon;

	float     specularPower;

	TexturePtr textureDiffuse;

	TexturePtr textureSphere;

	TexturePtr textureToon;

	eSPHEREMAP spheremap;

	bool edge;

public:
	sMaterial()
		: colorDiffuse( 0xFF000000 )
		, colorSpecular( 0x00000000UL )
		, colorAmbient( 0x00000000UL )
		, colorEmissive( 0x00000000UL )
		, specularPower(1.0f)
		, colorToon( 0xFFFFFFFF )
		, spheremap(eSPHEREMAP_MUL)
		, edge(false)
	{
	}
};