#pragma once

#include "../core/util/util.h"

#include "squirrel/squirrel.h"
#include "squirrel/sqstdio.h"
#include "squirrel/sqstdaux.h"

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