#pragma once

#include "../core/util/util.h"

#include "squirrel/squirrel.h"
#include "squirrel/sqstdio.h"
#include "squirrel/sqstdaux.h"

#include "../AssetNode.h"

#include <map>

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

public:
	void Build( AssetNode* assetRoot );

private:
	HSQUIRRELVM m_vm;

	typedef std::map< tstring_symbol,HSQOBJECT > _map_ClassObject;
	_map_ClassObject m_classObjects;
};