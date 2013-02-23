#pragma once

#include "core/util/CharSet.h"

class ParameterInterface
{
public:
	ParameterInterface()
	{
	}
	virtual ~ParameterInterface()
	{
	}

public:
	virtual const tstring_symbol& GetTypeName() const = 0;
};

template< class Type>
class Parameter : public ParameterInterface
{
private:
	static const tstring_symbol s_classTypeName;
public:
	static const tstring_symbol& GetClassTypeName()
	{
		return s_classTypeName;
	}

private:
	Type m_value;

public:
	void SetValue( const Type& value )
	{
		m_value = value;
	}
	const Type& GetValue() const
	{
		return m_value;
	}

public:
	const tstring_symbol& GetTypeName() const
	{
		return GetClassTypeName();
	}
};

template< class Type>
const tstring_symbol Parameter<Type>::s_classTypeName = to_tstring(typeid(Type).name());