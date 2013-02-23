#pragma once

#include <map>

#include "Parameter.h"

class GameObject;

class Behavior
{
public:
	typedef std::map<tstring_symbol,ParameterInterface*> ParameterMap;
protected:
	GameObject* m_gameObject;
	ParameterMap m_parameters;

public:
	Behavior(GameObject* gameObject);
	virtual ~Behavior();

public:
	void AddParameter( const tstring& name,ParameterInterface* parameter );
	void DeleteParameter( const tstring& name );
	ParameterInterface* GetParameter( const tstring& name );

	ParameterMap::iterator GetParameterBegin();
	ParameterMap::iterator GetParameterEnd();

public:
	virtual const tstring_symbol& GetTypeName() const = 0;
};

typedef std::tr1::shared_ptr< Behavior > BehaviorPtr;