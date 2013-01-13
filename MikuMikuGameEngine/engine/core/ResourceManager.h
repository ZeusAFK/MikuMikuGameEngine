#pragma once

#include "util/Charset.h"

class IResource
{
public:
	IResource(){};
	virtual ~IResource(){};
};

typedef std::tr1::shared_ptr< IResource > ResourcePtr;
typedef std::tr1::weak_ptr< IResource > ResourceWeakPtr;

class ResourceManager
{
public:
	static ResourceManager& GetInstance()
	{
		static ResourceManager manager;
		return manager;
	}

private:
	typedef std::map<tstring_symbol,ResourceWeakPtr> MapResource;
	MapResource m_mapResource;

public:
	ResourceManager();
	virtual ~ResourceManager();

public:
	ResourcePtr GetResource( const tstring_symbol& name );

	template< class T >
	std::tr1::shared_ptr<T> GetResource( const tstring_symbol& name )
	{
		return std::tr1::dynamic_pointer_cast<T>( GetResource( name ) );
	}

	void AddResource( const tstring_symbol& name,ResourcePtr resource );
};
