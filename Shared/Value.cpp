#include "Value.h"

namespace Value
{
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
