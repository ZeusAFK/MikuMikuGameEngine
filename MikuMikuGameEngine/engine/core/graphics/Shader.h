#pragma once

#include "Graphics.h"

#include "../ResourceManager.h"

class Shader : public UnmanagedResource , public IResource
{
private:
	ID3DXEffectPtr m_pEffect;

public:
	Shader();
	virtual ~Shader();

public:
	bool CreateFromFile( const tstring& filePath );
	bool CreateFromMemory( LPCVOID buffer,size_t size );

	ID3DXEffectPtr GetEffect();

public:
	virtual void OnLostDevice();
	virtual void OnResetDevice();
};

typedef std::tr1::shared_ptr< Shader > ShaderPtr;