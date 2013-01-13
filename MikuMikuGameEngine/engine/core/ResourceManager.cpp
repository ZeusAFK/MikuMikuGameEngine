#include "stdafx.h"

#include "ResourceManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

ResourcePtr ResourceManager::GetResource( const tstring_symbol& name )
{
	MapResource::iterator it = m_mapResource.find( name );
	if( it!=m_mapResource.end() )
	{
		if( !it->second.expired() )
		{
			return it->second.lock();
		}
		else
		{
			m_mapResource.erase( it );
		}
	}
	return ResourcePtr();
}

void ResourceManager::AddResource( const tstring_symbol& name,ResourcePtr resource )
{
	if( !GetResource( name ) )
	{
		m_mapResource.insert( MapResource::value_type( name,resource ) );
	}
}