#pragma once

#include <string>

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
		CValue( const std::string& string );
		CValue( const char* string );
		CValue( int integer );
		CValue( double decimal );
		CValue( bool boolean );

		__forceinline void					SetString( const std::string& string )		{ m_StringValue = string; m_ValueType = VT_STRING; };
		__forceinline void					SetInt( int integer )						{ m_IntValue = integer; m_ValueType = VT_INTEGER; };
		__forceinline void					SetDouble( double decimal )					{ m_DoubleValue = decimal; m_ValueType = VT_DOUBLE; };
		__forceinline void					SetBool( bool boolean )						{ SetInt( boolean ? 1 : 0 ); };

		// TODO: Make conversions in runtime from one type to another. E.g interger 10 to string "10".
		// Perhaps have a "Fast" version of the function and a converting one
		__forceinline const std::string&	GetString()			const { return m_StringValue; };
		__forceinline int					GetInt()			const { return m_IntValue; };
		__forceinline double				GetDouble()			const { return m_DoubleValue; };
		__forceinline bool					GetBool()			const { return !!GetInt(); };
		__forceinline ValueType				GetType()			const { return m_ValueType; };

	private:
		std::string		m_StringValue;
		int				m_IntValue;
		double			m_DoubleValue;
		ValueType		m_ValueType;
	};
}
