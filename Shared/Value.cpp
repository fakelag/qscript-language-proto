#include "Value.h"

namespace Value
{
	CValue::CValue()
	{
		m_DoubleValue = 0.0;
		m_IntValue = 0;
		m_ValueType = VT_INTEGER;
	}

	CValue::CValue( const CValue& value )
	{
		m_ValueType = value.m_ValueType;
		m_DoubleValue = value.m_DoubleValue;
		m_IntValue = value.m_IntValue;
		m_StringValue = value.m_StringValue;
	}

	CValue::CValue( const std::string& string )
	{
		SetString( string );
	}

	CValue::CValue( const char* string )
	{
		SetString( std::string( string ) );
	}

	CValue::CValue( int integer )
	{
		SetInt( integer );
	}

	CValue::CValue( double decimal )
	{
		SetDouble( decimal );
	}

	CValue::CValue( bool boolean )
	{
		SetBool( boolean );
	}
}
