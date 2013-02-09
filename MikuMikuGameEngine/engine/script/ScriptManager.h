#pragma once

#include "../core/util/util.h"

#include "squirrel/squirrel.h"
#include "squirrel/sqstdio.h"
#include "squirrel/sqstdaux.h"

#include "../AssetNode.h"

#include <map>

#include "ScriptClassObject.h"

class ScriptManager
{
public:
	static ScriptManager* GetInstance()
	{
		static ScriptManager manager;
		return &manager;
	}

	static void PrintFunc( HSQUIRRELVM vm,const SQChar* format,... );

private:
	ScriptManager();
	~ScriptManager();

private:
	void Clear();
	void BuildScript( const tstring& filePath );
	void BuildScript( AssetNode* assetScript );

	void AddBehaviorObject( const tstring_symbol& className,HSQOBJECT& obj );

public:
	void Build( AssetNode* assetRoot );

	ScriptClassObject* GetClassObject( const tstring_symbol& className );

private:
	HSQUIRRELVM m_vm;

	typedef std::map< tstring_symbol,ScriptClassObject* > _map_ClassObject;
	_map_ClassObject m_classObjects;
};