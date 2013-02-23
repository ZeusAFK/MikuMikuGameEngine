#include "stdafx.h"

#include "Behavior.h"

#include "GameObject.h"

Behavior::Behavior(GameObject* gameObject)
	: m_gameObject(gameObject)
{
}

Behavior::~Behavior()
{
}

void Behavior::AddParameter( const tstring& name,ParameterInterface* parameter )
{
	m_parameters.insert( ParameterMap::value_type( name,parameter ) );
}

void Behavior::DeleteParameter( const tstring& name )
{
	ParameterMap::iterator it = m_parameters.find( name );
	if( it!=m_parameters.end() )
	{
		delete it->second;
		m_parameters.erase( it );
	}
}

ParameterInterface* Behavior::GetParameter( const tstring& name )
{
	ParameterMap::iterator it = m_parameters.find( name );
	if( it!=m_parameters.end() )
	{
		return it->second;
	}
	return NULL;
}

Behavior::ParameterMap::iterator Behavior::GetParameterBegin()
{
	return m_parameters.begin();
}

Behavior::ParameterMap::iterator Behavior::GetParameterEnd()
{
	return m_parameters.end();
}