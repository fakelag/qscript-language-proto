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

	UTEST_CASE( "Value addition (operator+)" )
	{
		Value::CValue rhs( 2 );
		Value::CValue lhs( 5 );

		UTEST_ASSERT( ( lhs + rhs ).GetType() == Value::ValueType::VT_INTEGER );
		UTEST_ASSERT( ( lhs + rhs ).GetInt() == 7 );

		rhs.SetInt( 5 );
		UTEST_ASSERT( ( lhs + rhs ).GetType() == Value::ValueType::VT_INTEGER );
		UTEST_ASSERT( ( lhs + rhs ).GetInt() == 10 );

		rhs.SetInt( -10 );
		UTEST_ASSERT( ( lhs + rhs ).GetType() == Value::ValueType::VT_INTEGER );
		UTEST_ASSERT( ( lhs + rhs ).GetInt() == -5 );

		rhs.SetDouble( 4.2 );
		UTEST_ASSERT( ( lhs + rhs ).GetType() == Value::ValueType::VT_DOUBLE );
		UTEST_ASSERT( ( lhs + rhs ).GetDouble() == 9.2 );

		rhs.SetInt( 2 );
		lhs.SetDouble( 3.2 );
		UTEST_ASSERT( ( lhs + rhs ).GetType() == Value::ValueType::VT_DOUBLE );
		UTEST_ASSERT( ( lhs + rhs ).GetDouble() == 5.2 );

		lhs.SetString( "Ping" );
		UTEST_ASSERT( ( lhs + rhs ).GetType() == Value::ValueType::VT_STRING );
		UTEST_ASSERT( ( lhs + rhs ).GetString() == "Ping2" );

		lhs.SetInt( 2 );
		rhs.SetString( "Pong" );
		UTEST_ASSERT( ( lhs + rhs ).GetType() == Value::ValueType::VT_STRING );
		UTEST_ASSERT( ( lhs + rhs ).GetString() == "2Pong" );

		lhs.SetString( "Ping" );
		rhs.SetString( "Pong" );
		UTEST_ASSERT( ( lhs + rhs ).GetType() == Value::ValueType::VT_STRING );
		UTEST_ASSERT( ( lhs + rhs ).GetString() == "PingPong" );

		lhs.SetArray( { Value::CValue( 1 ) } );
		UTEST_THROW( lhs + rhs );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Array values" )
	{
		Value::CValue val1( 5 );
		Value::CValue val2( 10 );

		Value::CValue valArray( { val1, val2 } );

		UTEST_ASSERT( valArray.ArraySize() == 2 );
		UTEST_ASSERT( valArray[ 0 ].GetType() == Value::ValueType::VT_INTEGER );
		UTEST_ASSERT( valArray[ 0 ].GetInt() == 5 );
		
		valArray[ 0 ].SetInt( 10 );
		UTEST_ASSERT( valArray[ 0 ].GetInt() == 10 );
		UTEST_ASSERT( val1.GetInt() == 5 );

		UTEST_ASSERT( valArray.GetString() == "[10, 10]" );

		valArray.ArrayPush( Value::CValue( 20 ) );
		UTEST_ASSERT( valArray[ 0 ].GetInt() == 10 );
		UTEST_ASSERT( valArray[ 1 ].GetInt() == 10 );
		UTEST_ASSERT( valArray[ 2 ].GetInt() == 20 );
		UTEST_THROW( valArray[ 3 ] );

		Value::CValue valArray2( { Value::CValue( 20 ), Value::CValue( "a string" ) } );
		valArray.ArrayConcat( valArray2 );

		UTEST_ASSERT( valArray.ArraySize() == 5 );
		UTEST_ASSERT( valArray[ 0 ].GetInt() == 10 );
		UTEST_ASSERT( valArray[ 1 ].GetInt() == 10 );
		UTEST_ASSERT( valArray[ 2 ].GetInt() == 20 );
		UTEST_ASSERT( valArray[ 3 ].GetInt() == 20 );
		UTEST_ASSERT( valArray[ 4 ].GetType() == Value::ValueType::VT_STRING );
		UTEST_ASSERT( valArray[ 4 ].GetString() == "a string" );
		UTEST_THROW( valArray[ 5 ] );

		valArray.ArrayPush( Value::CValue( { val1, val2 } ) );
		UTEST_ASSERT( valArray.ArraySize() == 6 );
		UTEST_ASSERT( valArray[ 5 ].GetType() == Value::ValueType::VT_ARRAY );
		UTEST_ASSERT( valArray[ 5 ][ 0 ].GetType() == Value::ValueType::VT_INTEGER );
		UTEST_ASSERT( valArray[ 5 ][ 0 ].GetInt() == 5 );


		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Initialized/Uninitialized values" )
	{
		Value::CValue init1( 5 );
		Value::CValue uninit1;

		UTEST_ASSERT( init1.IsInitialized() == true );
		UTEST_ASSERT( uninit1.IsInitialized() == false );

		UTEST_THROW( uninit1.GetInt() );

		uninit1 = init1;
		UTEST_ASSERT( uninit1.IsInitialized() == true );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_END();
}
