#include "Value.h"

#define REQUIRE_TYPE( valuetype ) \
if ( m_ValueType != valuetype ) throw Exception( "CValue is not of type " #valuetype );

#define REQUIRE_INITIALIZED( ) \
if ( m_ValueType == VT_UNINITIALIZED ) throw Exception( "CValue is uninitialized" );

namespace Value
{
	CValue::CValue()
	{
		Uninitialize();
	}

	CValue::CValue( const CValue& value )
	{
		m_ValueType			= value.m_ValueType;
		m_DoubleValue		= value.m_DoubleValue;
		m_IntValue			= value.m_IntValue;
		m_StringValue		= value.m_StringValue;
		m_StringDouble		= value.m_StringDouble;
		m_StringInt			= value.m_StringInt;
		m_ArrayValue		= value.m_ArrayValue;
		m_StringCacheDouble = value.m_StringCacheDouble;
		m_StringCacheInt 	= value.m_StringCacheInt;
	}

	CValue::CValue( const std::string& string )
	{
		SetString( string );
		InitStringCache();
	}

	CValue::CValue( const char* string )
	{
		SetString( std::string( string ) );
		InitStringCache();
	}

	CValue::CValue( int integer )
	{
		SetInt( integer );
		InitStringCache();
	}

	CValue::CValue( double decimal )
	{
		SetDouble( decimal );
		InitStringCache();
	}

	CValue::CValue( bool boolean )
	{
		SetBool( boolean );
		InitStringCache();
	}

	CValue::CValue( const std::vector< CValue >& values )
	{
		SetArray( values );
		InitStringCache();
	}

	void CValue::Uninitialize()
	{
		m_DoubleValue		= 0.0;
		m_IntValue			= 0;
		m_ValueType			= VT_UNINITIALIZED;
		InitStringCache();
	}

	void CValue::InitStringCache()
	{
		m_StringDouble			= 0.0;
		m_StringInt				= 0;
		m_StringCacheDouble 	= "0.0000";
		m_StringCacheInt 		= "0";
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
				return m_StringCacheDouble;
			}
			else
			{
				// There is no cached value yet, we'll stringify the value and
				// cache it off for next time
				m_StringCacheDouble = std::to_string( m_DoubleValue );
				m_StringDouble = m_DoubleValue;
				return m_StringCacheDouble;
			}
		}
		case VT_BOOLEAN:
		case VT_INTEGER:
		{
			if ( m_StringInt == m_IntValue )
			{
				return m_StringCacheInt;
			}
			else
			{
				m_StringCacheInt = std::to_string( m_IntValue );
				m_StringInt = m_IntValue;
				return m_StringCacheInt;
			}
		}
		case VT_ARRAY:
		{
			m_StringValue = "[";
			for ( size_t i = 0; i < m_ArrayValue.size(); ++i )
				m_StringValue += i > 0 ? ", " + m_ArrayValue[ i ].GetString() : m_ArrayValue[ i ].GetString();

			m_StringValue += "]";
			return m_StringValue;
		}
		case VT_UNINITIALIZED:
			throw Exception( "CValue is uninitialized" );
		default:
			throw Exception( "Invalid CValue type" );
		}
	}

	const std::vector< CValue >& CValue::GetArray() const
	{
		switch ( m_ValueType )
		{
		case VT_ARRAY:
			return m_ArrayValue;
		case VT_UNINITIALIZED:
			throw Exception( "CValue is uninitialized" );
		default:
			throw Exception( "Invalid CValue type (CValue is not an array)" );
		}
	}

	int CValue::GetInt() const
	{
		switch ( m_ValueType )
		{
		case VT_BOOLEAN:
		case VT_INTEGER:
			return m_IntValue;
		case VT_DOUBLE:
			return ( int ) m_DoubleValue;
		case VT_STRING:
			return m_StringValue.length() > 0 ? 1 : 0;
		case VT_ARRAY:
			return m_ArrayValue.size() > 0 ? 1 : 0;
		case VT_UNINITIALIZED:
			throw Exception( "CValue is uninitialized" );
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
		case VT_BOOLEAN:
		case VT_INTEGER:
			return ( double ) m_IntValue;
		case VT_STRING:
			return m_StringValue.length() > 0 ? 1.0 : 0.0;
		case VT_ARRAY:
			return m_ArrayValue.size() > 0 ? 1.0 : 0.0;
		case VT_UNINITIALIZED:
			throw Exception( "CValue is uninitialized" );
		default:
			throw Exception( "Invalid CValue type" );
		}
	}

	bool CValue::GetBool() const
	{
		REQUIRE_INITIALIZED();
		return !!GetInt();
	}

	ValueType CValue::GetType() const
	{
		return m_ValueType;
	}

	void CValue::ArrayPush( const CValue& value )
	{
		REQUIRE_TYPE( VT_ARRAY );
		m_ArrayValue.push_back( value );
	}

	void CValue::ArrayConcat( const CValue& array )
	{
		REQUIRE_TYPE( VT_ARRAY );
		std::copy( array.m_ArrayValue.begin(), array.m_ArrayValue.end(), std::back_inserter( m_ArrayValue ) );
	}

	void CValue::ArrayClear()
	{
		REQUIRE_TYPE( VT_ARRAY );
		m_ArrayValue.clear();
	}

	size_t CValue::ArraySize()
	{
		REQUIRE_TYPE( VT_ARRAY );
		return m_ArrayValue.size();
	}
}
