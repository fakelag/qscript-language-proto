#pragma once

#include <string>

#if defined(_WIN32)
#define FORCEINLINE FORCEINLINE
#elif defined(_OSX)
#define FORCEINLINE inline
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

		FORCEINLINE void					SetString( const std::string& string )		{ m_StringValue = string; m_ValueType = VT_STRING; };
		FORCEINLINE void					SetInt( int integer )						{ m_IntValue = integer; m_ValueType = VT_INTEGER; };
		FORCEINLINE void					SetDouble( double decimal )					{ m_DoubleValue = decimal; m_ValueType = VT_DOUBLE; };
		FORCEINLINE void					SetBool( bool boolean )						{ SetInt( boolean ? 1 : 0 ); };

		// TODO: Make conversions in runtime from one type to another. E.g interger 10 to string "10".
		// Perhaps have a "Fast" version of the function and a converting one
		FORCEINLINE const std::string&	GetString()			const { return m_StringValue; };
		FORCEINLINE int					GetInt()			const { return m_IntValue; };
		FORCEINLINE double				GetDouble()			const { return m_DoubleValue; };
		FORCEINLINE bool				GetBool()			const { return !!GetInt(); };
		FORCEINLINE ValueType			GetType()			const { return m_ValueType; };

		FORCEINLINE CValue operator=(const CValue& other)
		{
			switch (other.m_ValueType)
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
					throw "Invalid operation";
			}
		}

	private:
		std::string			m_StringValue;
		int					m_IntValue;
		double				m_DoubleValue;
		enum ValueType		m_ValueType;
	};
}
