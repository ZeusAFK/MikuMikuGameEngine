#pragma once

#include "../core/util/util.h"

#include "squirrel/squirrel.h"
#include "squirrel/sqstdio.h"
#include "squirrel/sqstdaux.h"

#include <map>

#include "ScriptBehavior.h"

class ScriptClassObject
{
private:
	HSQUIRRELVM m_vm;
	HSQOBJECT m_classObject;
	typedef std::map< tstring_symbol,ScriptParameterInterfacePtr > _map_Parameter;
	_map_Parameter m_parameters;

public:
	ScriptClassObject( HSQUIRRELVM vm,HSQOBJECT& classObject );
	virtual ~ScriptClassObject();

public:
	ScriptBehavior* CreateBehavior();
};
