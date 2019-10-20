#pragma once

#include <string>
#include "Exception.h"

#if defined(_WIN32)
#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif
#elif defined(_OSX)
#ifndef FORCEINLINE
#define FORCEINLINE inline
#endif
#endif

namespace Value
{
	enum ValueType
	{
		VT_STRING = 0,
		VT_INTEGER,
		VT_DOUBLE,
	};

	class CValue
	{
	public:
		CValue();
		CValue( const CValue& value );
		CValue( const std::string& string );
		CValue( const char* string );
		CValue( int integer );
		CValue( double decimal );
		CValue( bool boolean );

		FORCEINLINE void					SetString( const std::string& string ) { m_StringValue = string; m_ValueType = VT_STRING; };
		FORCEINLINE void					SetInt( int integer ) { m_IntValue = integer; m_ValueType = VT_INTEGER; };
		FORCEINLINE void					SetDouble( double decimal ) { m_DoubleValue = decimal; m_ValueType = VT_DOUBLE; };
		FORCEINLINE void					SetBool( bool boolean ) { SetInt( boolean ? 1 : 0 ); };

		const std::string&		GetString();		// GetString uses internal optimization
		int						GetInt()			const;
		double					GetDouble()			const;
		bool					GetBool()			const;
		ValueType				GetType()			const;

		FORCEINLINE CValue operator=( const CValue& other )
		{
			switch ( other.m_ValueType )
			{
			case VT_STRING:
				SetString( other.m_StringValue );
				return *this;
			case VT_INTEGER:
				SetInt( other.m_IntValue );
				return *this;
			case VT_DOUBLE:
				SetDouble( other.m_DoubleValue );
				return *this;
			default:
				throw Exception( "Invalid CValue type" );
			}
		}

		FORCEINLINE CValue operator+( const CValue& other )
		{
			switch ( m_ValueType )
			{
			case VT_STRING:
			{
				switch ( other.m_ValueType )
				{
				case VT_STRING: return CValue( m_StringValue + other.m_StringValue );
				case VT_INTEGER: return CValue( m_StringValue + std::to_string( other.m_IntValue ) );
				case VT_DOUBLE: return CValue( m_StringValue + std::to_string( other.m_DoubleValue ) );
				default: throw Exception( "Invalid CValue type" );
				}
			}
			case VT_INTEGER:
			{
				switch ( other.m_ValueType )
				{
				case VT_STRING: return CValue( std::to_string( m_IntValue ) + other.m_StringValue );
				case VT_INTEGER: return CValue( m_IntValue + other.m_IntValue );
				case VT_DOUBLE: return CValue( ( double ) m_IntValue + other.m_DoubleValue );
				default: throw Exception( "Invalid CValue type" );
				}
			}
			case VT_DOUBLE:
			{
				switch ( other.m_ValueType )
				{
				case VT_STRING: return CValue( std::to_string( m_DoubleValue ) + other.m_StringValue );
				case VT_INTEGER: return CValue( m_DoubleValue + ( double ) other.m_IntValue );
				case VT_DOUBLE: return CValue( m_DoubleValue + other.m_DoubleValue );
				default: throw Exception( "Invalid CValue type" );
				}
			}
			default:
				throw Exception( "Invalid CValue type" );
			}
		}

	private:
		std::string			m_StringValue;
		int					m_IntValue;
		double				m_DoubleValue;
		enum ValueType		m_ValueType;

		// Stringcaching
		double				m_StringDouble;
		int					m_StringInt;
	};
}
