#pragma once

#include "../core/util/util.h"

#include "squirrel/squirrel.h"
#include "squirrel/sqstdio.h"
#include "squirrel/sqstdaux.h"

#include <map>

class ScriptParameterInterface
{
protected:
	tstring_symbol m_name;
public:
	void SetName( const tstring_symbol& name )
	{
		m_name = name;
	}

	virtual void SetParameter( HSQUIRRELVM vm ) const = 0;
};

typedef std::tr1::shared_ptr< ScriptParameterInterface > ScriptParameterInterfacePtr;

class ScriptParameterInteger : public ScriptParameterInterface
{
private:
	int m_value;
public:
	ScriptParameterInteger();
	void SetValue( int value );

	void SetParameter( HSQUIRRELVM vm ) const;
};

class ScriptParameterBool : public ScriptParameterInterface
{
private:
	bool m_value;
public:
	ScriptParameterBool();
	void SetValue( bool value );

	void SetParameter( HSQUIRRELVM vm ) const;
};

class ScriptParameterFloat : public ScriptParameterInterface
{
private:
	float m_value;
public:
	ScriptParameterFloat();
	void SetValue( float value );

	void SetParameter( HSQUIRRELVM vm ) const;
};

class ScriptParameterString : public ScriptParameterInterface
{
private:
	tstring m_value;
public:
	ScriptParameterString();
	void SetValue( const tstring& value );

	void SetParameter( HSQUIRRELVM vm ) const;
};

class ScriptBehavior
{
private:
	HSQUIRRELVM m_vm;

	bool m_initialize;
	bool m_haveAwake;
	bool m_haveStart;
	bool m_haveUpdate;

	bool m_start;

	HSQOBJECT m_instance;
	HSQOBJECT m_awakeFunction;
	HSQOBJECT m_startFunction;

	HSQOBJECT m_updateFunction;

public:
	ScriptBehavior( HSQUIRRELVM vm,HSQOBJECT classObject,const std::map< tstring_symbol,ScriptParameterInterfacePtr >& scriptParameters );
	virtual ~ScriptBehavior();

public:
	void Awake();
	void Start();
	void Update( float deltaTime );
};