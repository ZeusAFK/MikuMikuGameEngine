#include "stdafx.h"

#include "ScriptClassObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ScriptClassObject::ScriptClassObject( HSQUIRRELVM vm,HSQOBJECT& classObject )
{
	m_vm = vm;
	m_classObject = classObject;

	sq_addref( m_vm,&m_classObject );

	// class‚Ìƒƒ“ƒo[•Ï”‚ð—ñ‹“‚·‚é
	{
		int top = sq_gettop(m_vm);

		sq_pushobject(m_vm, m_classObject);

		sq_pushnull(m_vm);

		while( SQ_SUCCEEDED( sq_next(m_vm,-2) ) )
		{
			const SQChar* key = NULL;
			HSQOBJECT val;

			if( SQ_SUCCEEDED( sq_getstring( m_vm,-2,&key) ) &&
				SQ_SUCCEEDED( sq_getstackobj(m_vm, -1, &val) ) &&
				val._type!=OT_CLOSURE )
			{
				int top = sq_gettop(m_vm);

				sq_pushobject(m_vm, m_classObject);

				sq_pushstring( m_vm,key,-1 );
				if( SQ_SUCCEEDED( sq_getattributes(m_vm,-2 ) ) )
				{
					sq_pushstring( m_vm,_SC("SerializeType"),-1 );
					if( SQ_SUCCEEDED( sq_get(m_vm,-2 ) ) )
					{
						const SQChar* value;

						if( SQ_SUCCEEDED( sq_getstring( m_vm,-1,&value) ) )
						{
							tstring serializeType = value;

							OutputDebugStringFormat( _T("SerializeType %s %s\n"),value,key );

							if( serializeType==_T("int") )
							{
								ScriptParameterInteger* parameter = new ScriptParameterInteger;
								tstring_symbol nameSymbol = tstring(key);
								parameter->SetName( nameSymbol );
								parameter->SetValue( sq_objtointeger(&val) );

								m_parameters.insert( _map_Parameter::value_type( nameSymbol,ScriptParameterInterfacePtr(parameter) ) );
							}
							else if( serializeType==_T("bool") )
							{
								ScriptParameterBool* parameter = new ScriptParameterBool;
								tstring_symbol nameSymbol = tstring(key);
								parameter->SetName( nameSymbol );
								parameter->SetValue( sq_objtobool(&val)!=SQFalse?true:false );

								m_parameters.insert( _map_Parameter::value_type( nameSymbol,ScriptParameterInterfacePtr(parameter) ) );
							}
							else if( serializeType==_T("float") )
							{
								ScriptParameterFloat* parameter = new ScriptParameterFloat;
								tstring_symbol nameSymbol = tstring(key);
								parameter->SetName( nameSymbol );
								parameter->SetValue( sq_objtofloat(&val) );

								m_parameters.insert( _map_Parameter::value_type( nameSymbol,ScriptParameterInterfacePtr(parameter) ) );
							}
							else if( serializeType==_T("string") )
							{
								ScriptParameterString* parameter = new ScriptParameterString;
								tstring_symbol nameSymbol = tstring(key);
								parameter->SetName( nameSymbol );
								parameter->SetValue( tstring(sq_objtostring(&val)) );

								m_parameters.insert( _map_Parameter::value_type( nameSymbol,ScriptParameterInterfacePtr(parameter) ) );
							}
						}

						sq_pop(m_vm,1);
					}
				}

				sq_settop(m_vm,top);
			}

			sq_pop( m_vm,2 );
		}

		sq_settop( m_vm,top );
	}
}

ScriptClassObject::~ScriptClassObject()
{
	sq_release( m_vm,&m_classObject );
}

ScriptBehavior* ScriptClassObject::CreateBehavior()
{
	return new ScriptBehavior( m_vm,m_classObject,m_parameters );
}