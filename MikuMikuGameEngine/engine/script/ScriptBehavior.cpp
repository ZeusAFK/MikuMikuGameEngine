#include "stdafx.h"

#include "ScriptBehavior.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ScriptParameterInteger::ScriptParameterInteger()
	: m_value(0)
{
}

void ScriptParameterInteger::SetValue( int value )
{
	m_value = value;
}

void ScriptParameterInteger::SetParameter( HSQUIRRELVM vm ) const
{
	sq_pushstring( vm,m_name.key().c_str(),-1 );
	sq_pushinteger( vm,m_value );
	
	sq_set( vm,-3 );
}

ScriptParameterBool::ScriptParameterBool()
	: m_value(false)
{
}

void ScriptParameterBool::SetValue( bool value )
{
	m_value = value;
}

void ScriptParameterBool::SetParameter( HSQUIRRELVM vm ) const
{
	sq_pushstring( vm,m_name.key().c_str(),-1 );
	sq_pushbool( vm,m_value?SQTrue:SQFalse );
	
	sq_set( vm,-3 );
}

ScriptParameterFloat::ScriptParameterFloat()
	: m_value(0.0f)
{
}

void ScriptParameterFloat::SetValue( float value )
{
	m_value = value;
}

void ScriptParameterFloat::SetParameter( HSQUIRRELVM vm ) const
{
	sq_pushstring( vm,m_name.key().c_str(),-1 );
	sq_pushfloat( vm,m_value );
	
	sq_set( vm,-3 );
}

ScriptParameterString::ScriptParameterString()
{
}

void ScriptParameterString::SetValue( const tstring& value )
{
	m_value = value;
}

void ScriptParameterString::SetParameter( HSQUIRRELVM vm ) const
{
	sq_pushstring( vm,m_name.key().c_str(),-1 );
	sq_pushstring( vm,m_value.c_str(),-1 );
	
	sq_set( vm,-3 );
}

ScriptBehavior::ScriptBehavior( HSQUIRRELVM vm,HSQOBJECT classObject,const std::map< tstring_symbol,ScriptParameterInterfacePtr >& scriptParameters  )
	: m_vm(vm)
{
	int top = sq_gettop(m_vm);

	sq_pushobject(m_vm, classObject);

	sq_pushroottable( m_vm );

	m_initialize = false;
	m_haveAwake = false;
	m_haveStart = false;
	m_haveUpdate = false;

	if( SQ_SUCCEEDED( sq_call( m_vm,1,SQTrue,SQTrue) ) )
	{
		if( SQ_SUCCEEDED( sq_getstackobj(m_vm, -1, &m_instance) ) )
		{
			sq_addref( m_vm,&m_instance );

			m_initialize = true;

			sq_pushstring( m_vm,_SC("Awake"),-1 );
			if( SQ_SUCCEEDED( sq_get(m_vm,-2 ) ) )
			{
				if( SQ_SUCCEEDED( sq_getstackobj(m_vm, -1, &m_awakeFunction) ) )
				{
					sq_addref( m_vm,&m_awakeFunction );
					m_haveAwake = true;
				}
			}
			sq_pop(m_vm,1);

			sq_pushstring( m_vm,_SC("Start"),-1 );
			if( SQ_SUCCEEDED( sq_get(m_vm,-2 ) ) )
			{
				if( SQ_SUCCEEDED( sq_getstackobj(m_vm, -1, &m_startFunction) ) )
				{
					sq_addref( m_vm,&m_startFunction );
					m_haveStart = true;
				}
			}
			sq_pop(m_vm,1);

			sq_pushstring( m_vm,_SC("Update"),-1 );
			if( SQ_SUCCEEDED( sq_get(m_vm,-2 ) ) )
			{
				if( SQ_SUCCEEDED( sq_getstackobj(m_vm, -1, &m_updateFunction) ) )
				{
					sq_addref( m_vm,&m_updateFunction );
					m_haveUpdate = true;
				}
			}
			sq_pop(m_vm,1);

			std::map< tstring_symbol,ScriptParameterInterfacePtr >::const_iterator it = scriptParameters.begin();
			while( it!=scriptParameters.end() )
			{
				it->second->SetParameter( m_vm );
				it++;
			}
		}
	}

	sq_settop( m_vm,top );
}

ScriptBehavior::~ScriptBehavior()
{
	if( m_initialize )
	{
		if( m_haveAwake )
		{
			sq_release( m_vm,&m_awakeFunction );
		}
		if( m_haveStart )
		{
			sq_release( m_vm,&m_startFunction );
		}
		if( m_haveUpdate )
		{
			sq_release( m_vm,&m_updateFunction );
		}

		sq_release( m_vm,&m_instance );
	}
}

void ScriptBehavior::Awake()
{
	if( !m_initialize || !m_haveAwake )
	{
		return;
	}

	int top = sq_gettop(m_vm);

	sq_pushobject(m_vm, m_instance);
	sq_pushobject(m_vm, m_awakeFunction);

	sq_push( m_vm,-2 );
	sq_call( m_vm,1,SQFalse,SQTrue );

	sq_settop( m_vm,top );
}

void ScriptBehavior::Start()
{
	if( !m_initialize || !m_haveStart )
	{
		return;
	}

	int top = sq_gettop(m_vm);

	sq_pushobject(m_vm, m_instance);
	sq_pushobject(m_vm, m_startFunction);

	sq_push( m_vm,-2 );
	sq_call( m_vm,1,SQFalse,SQTrue );

	sq_settop( m_vm,top );
}

void ScriptBehavior::Update( float deltaTime )
{
	if( !m_initialize || !m_haveUpdate )
	{
		return;
	}

	int top = sq_gettop(m_vm);

	sq_pushobject(m_vm, m_instance);
	sq_pushobject(m_vm, m_updateFunction);

	sq_push( m_vm,-3 );
	sq_pushfloat( m_vm,deltaTime );
	sq_call( m_vm,2,SQFalse,SQTrue );

	sq_settop( m_vm,top );
}