#include "Tests.h"
#include "UnitTests.h"
#include "Value.h"

void RunValueTests()
{
	UTEST_BEGIN( "CValue Tests" );

	UTEST_CASE( "Creating a string value" )
	{
		Value::CValue value( "string value" );

		UTEST_ASSERT( value.GetType() == Value::VT_STRING );
		UTEST_ASSERT( value.GetString() == "string value" );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Switching value type on runtime" )
	{
		Value::CValue value( "string value" );

		UTEST_ASSERT( value.GetType() == Value::VT_STRING );
		UTEST_ASSERT( value.GetString() == "string value" );

		value.SetInt( 42 );
		UTEST_ASSERT( value.GetType() == Value::VT_INTEGER );
		UTEST_ASSERT( value.GetInt() == 42 );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Runtime primitive->string conversions" )
	{
		Value::CValue value( 2.5 );

		UTEST_ASSERT( value.GetType() == Value::VT_DOUBLE );
		UTEST_ASSERT( value.GetDouble() == 2.5 );

		// getstring uses internal cache/optimizations, call it a few times
		UTEST_ASSERT( value.GetString() == "2.500000" );
		UTEST_ASSERT( value.GetString() == "2.500000" );
		UTEST_ASSERT( value.GetString() == "2.500000" );

		value.SetDouble( 2.1 );
		UTEST_ASSERT( value.GetString() == "2.100000" );
		UTEST_ASSERT( value.GetString() == "2.100000" );
		UTEST_ASSERT( value.GetString() == "2.100000" );

		value.SetString( "abc" );
		UTEST_ASSERT( value.GetString() == "abc" );
		UTEST_ASSERT( value.GetString() == "abc" );
		UTEST_ASSERT( value.GetString() == "abc" );

		value.SetBool( true );
		UTEST_ASSERT( value.GetString() == "1" );
		UTEST_ASSERT( value.GetString() == "1" );
		UTEST_ASSERT( value.GetString() == "1" );

		value.SetBool( false );
		UTEST_ASSERT( value.GetString() == "0" );
		UTEST_ASSERT( value.GetString() == "0" );
		UTEST_ASSERT( value.GetString() == "0" );

		value.SetInt( 42 );
		UTEST_ASSERT( value.GetString() == "42" );
		UTEST_ASSERT( value.GetString() == "42" );
		UTEST_ASSERT( value.GetString() == "42" );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Runtime string->primitive (use string length)" )
	{
		Value::CValue value( "b1g string" );

		UTEST_ASSERT( value.GetType() == Value::VT_STRING );
		UTEST_ASSERT( value.GetString() == "b1g string" );

		UTEST_ASSERT( value.GetBool() == true );
		UTEST_ASSERT( value.GetInt() == 1 );
		UTEST_ASSERT( value.GetDouble() == 1.0 );

		value.SetString( "" ); // 0 length

		UTEST_ASSERT( value.GetBool() == false );
		UTEST_ASSERT( value.GetInt() == 0 );
		UTEST_ASSERT( value.GetDouble() == 0.0 );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_END();
}
