#pragma once

#include "../core/util/util.h"

#include "squirrel/squirrel.h"
#include "squirrel/sqstdio.h"
#include "squirrel/sqstdaux.h"

class ScriptParameterInterface
{
protected:
	tstring_symbol m_name;
public:
	void SetName( const tstring_symbol& name )
	{
		m_name = name;
	}

	virtual void SetParameter( HSQUIRRELVM vm ) = 0;
};

class ScriptParameterInteger : public ScriptParameterInterface
{
private:
	int m_value;
public:
	ScriptParameterInteger();
	void SetValue( int value );

	void SetParameter( HSQUIRRELVM vm );
};

class ScriptBehavior
{
private:
	HSQUIRRELVM m_vm;

	bool m_initialize;
	bool m_haveAwake;
	bool m_haveStart;
	bool m_haveUpdate;

	HSQOBJECT m_instance;
	HSQOBJECT m_awakeFunction;
	HSQOBJECT m_startFunction;

	HSQOBJECT m_updateFunction;

public:
	ScriptBehavior( HSQUIRRELVM vm,HSQOBJECT classObject );
	virtual ~ScriptBehavior();

public:
	void Awake();
	void Start();
	void Update( float deltaTime );
};