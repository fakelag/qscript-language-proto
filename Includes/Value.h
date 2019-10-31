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

#include <cmath>

namespace Value
{
	enum ValueType
	{
		VT_UNINITIALIZED = -1,
		VT_STRING = 0,
		VT_INTEGER,
		VT_DOUBLE,
		VT_ARRAY,
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
		CValue( const std::vector< CValue >& values );

		FORCEINLINE void					SetString( const std::string& string ) { m_StringValue = string; m_ValueType = VT_STRING; };
		FORCEINLINE void					SetInt( int integer ) { m_IntValue = integer; m_ValueType = VT_INTEGER; };
		FORCEINLINE void					SetDouble( double decimal ) { m_DoubleValue = decimal; m_ValueType = VT_DOUBLE; };
		FORCEINLINE void					SetBool( bool boolean ) { SetInt( boolean ? 1 : 0 ); };
		FORCEINLINE void					SetArray( const std::vector< CValue >& values ) { m_ArrayValue = values; m_ValueType = VT_ARRAY; };

		const std::string&				GetString();		// GetString uses internal optimization
		const std::vector< CValue >&	GetArray()			const;
		int								GetInt()			const;
		double							GetDouble()			const;
		bool							GetBool()			const;
		ValueType						GetType()			const;
		FORCEINLINE bool				IsInitialized()		const { return m_ValueType != VT_UNINITIALIZED; }

		// Uninitialize
		void Uninitialize();

		// Array functions
		void							ArrayPush( const CValue& value );
		void							ArrayConcat( const CValue& array );
		void							ArrayClear();
		size_t							ArraySize();

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
			case VT_ARRAY:
				SetArray( other.m_ArrayValue );
				return *this;
			case VT_UNINITIALIZED:
				Uninitialize();
				return *this;
			default:
				throw Exception( "Invalid CValue type" );
			}
		}

		FORCEINLINE bool operator==( const CValue& other ) const
		{
			if ( other.m_ValueType != m_ValueType )
				return false;

			switch ( m_ValueType )
			{
				case VT_UNINITIALIZED: return true;
				case VT_ARRAY: return false;
				case VT_INTEGER: return m_IntValue == other.m_IntValue;
				case VT_DOUBLE: return m_DoubleValue == other.m_DoubleValue;
				case VT_STRING: return m_StringValue == other.m_StringValue;
				default: throw Exception( "Invalid CValue type" );
			}
		}

		FORCEINLINE bool operator!=( const CValue& other ) const
		{
			return !operator==( other );
		}

		FORCEINLINE CValue operator!() const
		{
			return !GetInt();
		}

		FORCEINLINE CValue operator+( const CValue& other ) const
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
				case VT_ARRAY: throw Exception( "Invalid CValue type" );
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
				case VT_ARRAY: throw Exception( "Invalid CValue type" );
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
				case VT_ARRAY: throw Exception( "Invalid CValue type" );
				default: throw Exception( "Invalid CValue type" );
				}
			}
			case VT_ARRAY:
			{
				throw Exception( "Invalid CValue type" );
			}
			default:
				throw Exception( "Invalid CValue type" );
			}
		}

		FORCEINLINE CValue operator-( const CValue& other ) const
		{
			switch ( m_ValueType )
			{
			case VT_STRING:
			{
				throw Exception( "Invalid CValue type" );
			}
			case VT_INTEGER:
			{
				switch ( other.m_ValueType )
				{
				case VT_STRING: throw Exception( "Invalid CValue type" );
				case VT_INTEGER: return CValue( m_IntValue - other.m_IntValue );
				case VT_DOUBLE: return CValue( ( double ) m_IntValue - other.m_DoubleValue );
				case VT_ARRAY: throw Exception( "Invalid CValue type" );
				default: throw Exception( "Invalid CValue type" );
				}
			}
			case VT_DOUBLE:
			{
				switch ( other.m_ValueType )
				{
				case VT_STRING: throw Exception( "Invalid CValue type" );
				case VT_INTEGER: return CValue( m_DoubleValue - ( double ) other.m_IntValue );
				case VT_DOUBLE: return CValue( m_DoubleValue - other.m_DoubleValue );
				case VT_ARRAY: throw Exception( "Invalid CValue type" );
				default: throw Exception( "Invalid CValue type" );
				}
			}
			case VT_ARRAY:
			{
				throw Exception( "Invalid operation on an array" );
			}
			default:
				throw Exception( "Invalid CValue type" );
			}
		}

		FORCEINLINE CValue operator-() const
		{
			switch ( m_ValueType )
			{
			case VT_STRING:
			{
				throw Exception( "Invalid CValue type" );
			}
			case VT_INTEGER:
			{
				return CValue( -m_IntValue );
			}
			case VT_DOUBLE:
			{
				return CValue( -m_DoubleValue );
			}
			case VT_ARRAY:
			{
				throw Exception( "Invalid operation on an array" );
			}
			default:
				throw Exception( "Invalid CValue type" );
			}
		}

		FORCEINLINE CValue operator/( const CValue& other ) const
		{
			switch ( m_ValueType )
			{
			case VT_STRING:
			{
				throw Exception( "Invalid CValue type" );
			}
			case VT_INTEGER:
			{
				switch ( other.m_ValueType )
				{
				case VT_STRING: throw Exception( "Invalid CValue type" );
				case VT_INTEGER: return CValue( m_IntValue / other.m_IntValue );
				case VT_DOUBLE: return CValue( ( double ) m_IntValue / other.m_DoubleValue );
				case VT_ARRAY: throw Exception( "Invalid CValue type" );
				default: throw Exception( "Invalid CValue type" );
				}
			}
			case VT_DOUBLE:
			{
				switch ( other.m_ValueType )
				{
				case VT_STRING: throw Exception( "Invalid CValue type" );
				case VT_INTEGER: return CValue( m_DoubleValue / ( double ) other.m_IntValue );
				case VT_DOUBLE: return CValue( m_DoubleValue / other.m_DoubleValue );
				case VT_ARRAY: throw Exception( "Invalid CValue type" );
				default: throw Exception( "Invalid CValue type" );
				}
			}
			case VT_ARRAY:
			{
				throw Exception( "Invalid operation on an array" );
			}
			default:
				throw Exception( "Invalid CValue type" );
			}
		}

		FORCEINLINE CValue operator*( const CValue& other ) const
		{
			switch ( m_ValueType )
			{
			case VT_STRING:
			{
				throw Exception( "Invalid CValue type" );
			}
			case VT_INTEGER:
			{
				switch ( other.m_ValueType )
				{
				case VT_STRING: throw Exception( "Invalid CValue type" );
				case VT_INTEGER: return CValue( m_IntValue * other.m_IntValue );
				case VT_DOUBLE: return CValue( ( double ) m_IntValue * other.m_DoubleValue );
				case VT_ARRAY: throw Exception( "Invalid CValue type" );
				default: throw Exception( "Invalid CValue type" );
				}
			}
			case VT_DOUBLE:
			{
				switch ( other.m_ValueType )
				{
				case VT_STRING: throw Exception( "Invalid CValue type" );
				case VT_INTEGER: return CValue( m_DoubleValue * ( double ) other.m_IntValue );
				case VT_DOUBLE: return CValue( m_DoubleValue * other.m_DoubleValue );
				case VT_ARRAY: throw Exception( "Invalid CValue type" );
				default: throw Exception( "Invalid CValue type" );
				}
			}
			case VT_ARRAY:
			{
				throw Exception( "Invalid operation on an array" );
			}
			default:
				throw Exception( "Invalid CValue type" );
			}
		}

		FORCEINLINE CValue operator%( const CValue& other ) const
		{
			switch ( m_ValueType )
			{
			case VT_STRING:
				throw Exception( "Invalid CValue type" );
			case VT_INTEGER:
			{
				switch ( other.m_ValueType )
				{
				case VT_STRING: throw Exception( "Invalid CValue type" );
				case VT_INTEGER: return CValue( m_IntValue % other.m_IntValue );
				case VT_DOUBLE: return CValue( std::fmodf( ( double ) m_IntValue, other.m_DoubleValue ) );
				case VT_ARRAY: throw Exception( "Invalid CValue type" );
				default: throw Exception( "Invalid CValue type" );
				}
			}
			case VT_DOUBLE:
			{
				switch ( other.m_ValueType )
				{
				case VT_STRING: throw Exception( "Invalid CValue type" );
				case VT_INTEGER: return CValue( std::fmodf( m_DoubleValue, ( double ) other.m_IntValue ) );
				case VT_DOUBLE: return CValue( std::fmodf( m_DoubleValue, other.m_DoubleValue ) );
				case VT_ARRAY: throw Exception( "Invalid CValue type" );
				default: throw Exception( "Invalid CValue type" );
				}
			}
			case VT_ARRAY:
			{
				throw Exception( "Invalid operation on an array" );
			}
			default:
				throw Exception( "Invalid CValue type" );
			}
		}

		FORCEINLINE CValue& operator[]( int index )
		{
			if ( m_ValueType != VT_ARRAY )
				throw Exception( "CValue is not an array" );

			if ( m_ArrayValue.size() <= ( size_t ) index )
				throw Exception( "CValue array index out of bounds" );

			return m_ArrayValue[ index ];
		}

	private:
		std::string				m_StringValue;
		int						m_IntValue;
		double					m_DoubleValue;
		std::vector< CValue >	m_ArrayValue;
		enum ValueType			m_ValueType;

		// Stringcaching
		void 					InitStringCache();
		std::string 			m_StringCacheDouble;
		std::string 			m_StringCacheInt;
		double					m_StringDouble;
		int						m_StringInt;
	};
}
