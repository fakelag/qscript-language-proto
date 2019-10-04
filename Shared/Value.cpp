#include "Value.h"

namespace Value
{
	CValue::CValue()
	{
		m_DoubleValue		= 0.0;
		m_IntValue			= 0;
		m_ValueType			= VT_INTEGER;
		m_StringDouble		= 0.0;
		m_StringInt			= 0;
	}

	CValue::CValue( const CValue& value )
	{
		m_ValueType			= value.m_ValueType;
		m_DoubleValue		= value.m_DoubleValue;
		m_IntValue			= value.m_IntValue;
		m_StringValue		= value.m_StringValue;
		m_StringDouble		= value.m_StringDouble;
		m_StringInt			= value.m_StringInt;
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

	const std::string& CValue::GetString()
	{
		switch ( m_ValueType )
		{
		case VT_STRING:
			return m_StringValue;
		case VT_DOUBLE:
		{
			if ( m_StringDouble == m_DoubleValue )
			{
				// A cached value remains from a previous call. Use it.
				return m_StringValue;
			}
			else
			{
				// There is no cached value yet, we'll stringify the value and
				// cache it off for next time
				m_StringValue = std::to_string( m_DoubleValue );
				m_StringDouble = m_DoubleValue;
				return m_StringValue;
			}
		}
		case VT_INTEGER:
		{
			if ( m_StringInt == m_IntValue )
			{
				return m_StringValue;
			}
			else
			{
				m_StringValue = std::to_string( m_IntValue );
				m_StringInt = m_IntValue;
				return m_StringValue;
			}
		}
		default:
			throw Exception( "Invalid CValue type" );
		}
	}

	int CValue::GetInt() const
	{
		switch ( m_ValueType )
		{
		case VT_INTEGER:
			return m_IntValue;
		case VT_DOUBLE:
			return ( int ) m_DoubleValue;
		case VT_STRING:
			return m_StringValue.length() > 0;
		default:
			throw Exception( "Invalid CValue type" );
		}
	}

	double CValue::GetDouble() const
	{
		switch ( m_ValueType )
		{
		case VT_DOUBLE:
			return m_DoubleValue;
		case VT_INTEGER:
			return ( double ) m_IntValue;
		case VT_STRING:
			return m_StringValue.length() > 0 ? 1.0 : 0.0;
		default:
			throw Exception( "Invalid CValue type" );
		}
	}

	bool CValue::GetBool() const
	{
		return !!GetInt();
	}

	ValueType CValue::GetType() const
	{
		return m_ValueType;
	}
}
