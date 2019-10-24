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

	UTEST_CASE( "Value subtraction (operator-)" )
	{
		Value::CValue rhs( 2 );
		Value::CValue lhs( 5 );

		UTEST_ASSERT( ( lhs - rhs ).GetType() == Value::ValueType::VT_INTEGER );
		UTEST_ASSERT( ( lhs - rhs ).GetInt() == 3 );

		rhs.SetInt( 5 );
		UTEST_ASSERT( ( lhs - rhs ).GetType() == Value::ValueType::VT_INTEGER );
		UTEST_ASSERT( ( lhs - rhs ).GetInt() == 0 );

		rhs.SetInt( -10 );
		UTEST_ASSERT( ( lhs - rhs ).GetType() == Value::ValueType::VT_INTEGER );
		UTEST_ASSERT( ( lhs - rhs ).GetInt() == 15 );

		rhs.SetDouble( 4.2 );
		UTEST_ASSERT( ( lhs - rhs ).GetType() == Value::ValueType::VT_DOUBLE );
		UTEST_ASSERT( ( lhs - rhs ).GetDouble() == 5.0 - 4.2 );

		rhs.SetInt( 2 );
		lhs.SetDouble( 3.2 );
		UTEST_ASSERT( ( lhs - rhs ).GetType() == Value::ValueType::VT_DOUBLE );
		UTEST_ASSERT( ( lhs - rhs ).GetDouble() == 3.2 - 2 );

		lhs.SetString( "Ping" );
		UTEST_THROW( lhs - rhs );

		lhs.SetArray( { Value::CValue( 1 ) } );
		UTEST_THROW( lhs - rhs );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Value subtraction (operator- prefix)" )
	{
		Value::CValue value( 2 );

		UTEST_ASSERT( ( -value ) == Value::CValue( -2 ) );
		UTEST_ASSERT( ( -( -value ) ) == Value::CValue( 2 ) );

		value.SetDouble( 5.0 );
		UTEST_ASSERT( ( -value ).GetType() == Value::ValueType::VT_DOUBLE );
		UTEST_ASSERT( ( -value ).GetDouble() == -5.0 );

		value.SetString( "Ping" );
		UTEST_THROW( -value );

		value.SetArray( { Value::CValue( 1 ) } );
		UTEST_THROW( -value );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Value division (operator/)" )
	{
		Value::CValue rhs( 2 );
		Value::CValue lhs( 10 );

		UTEST_ASSERT( ( lhs / rhs ).GetType() == Value::ValueType::VT_INTEGER );
		UTEST_ASSERT( ( lhs / rhs ).GetInt() == 5 );

		rhs.SetInt( 5 );
		UTEST_ASSERT( ( lhs / rhs ).GetType() == Value::ValueType::VT_INTEGER );
		UTEST_ASSERT( ( lhs / rhs ).GetInt() == 2 );

		rhs.SetInt( -10 );
		UTEST_ASSERT( ( lhs / rhs ).GetType() == Value::ValueType::VT_INTEGER );
		UTEST_ASSERT( ( lhs / rhs ).GetInt() == 10 / -10 );

		rhs.SetDouble( 3.0 );
		UTEST_ASSERT( ( lhs / rhs ).GetType() == Value::ValueType::VT_DOUBLE );
		UTEST_ASSERT( ( lhs / rhs ).GetDouble() == 10 / 3.0 );

		rhs.SetDouble( 4.2 );
		UTEST_ASSERT( ( lhs / rhs ).GetType() == Value::ValueType::VT_DOUBLE );
		UTEST_ASSERT( ( lhs / rhs ).GetDouble() == 10.0 / 4.2 );

		rhs.SetInt( 2 );
		lhs.SetDouble( 3.2 );
		UTEST_ASSERT( ( lhs / rhs ).GetType() == Value::ValueType::VT_DOUBLE );
		UTEST_ASSERT( ( lhs / rhs ).GetDouble() == 3.2 / 2.0 );

		lhs.SetString( "Ping" );
		UTEST_THROW( lhs / rhs );

		lhs.SetArray( { Value::CValue( 1 ) } );
		UTEST_THROW( lhs / rhs );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Value multiplication (operator*)" )
	{
		Value::CValue rhs( 2 );
		Value::CValue lhs( 10 );

		UTEST_ASSERT( ( lhs * rhs ).GetType() == Value::ValueType::VT_INTEGER );
		UTEST_ASSERT( ( lhs * rhs ).GetInt() == 20 );

		rhs.SetInt( 5 );
		UTEST_ASSERT( ( lhs * rhs ).GetType() == Value::ValueType::VT_INTEGER );
		UTEST_ASSERT( ( lhs * rhs ).GetInt() == 50 );

		rhs.SetInt( -10 );
		UTEST_ASSERT( ( lhs * rhs ).GetType() == Value::ValueType::VT_INTEGER );
		UTEST_ASSERT( ( lhs * rhs ).GetInt() == -100 );

		rhs.SetDouble( 3.0 );
		UTEST_ASSERT( ( lhs * rhs ).GetType() == Value::ValueType::VT_DOUBLE );
		UTEST_ASSERT( ( lhs * rhs ).GetDouble() == 10 * 3.0 );

		rhs.SetDouble( 4.2 );
		UTEST_ASSERT( ( lhs * rhs ).GetType() == Value::ValueType::VT_DOUBLE );
		UTEST_ASSERT( ( lhs * rhs ).GetDouble() == 10.0 * 4.2 );

		rhs.SetInt( 2 );
		lhs.SetDouble( 3.2 );
		UTEST_ASSERT( ( lhs * rhs ).GetType() == Value::ValueType::VT_DOUBLE );
		UTEST_ASSERT( ( lhs * rhs ).GetDouble() == 3.2 * 2.0 );

		lhs.SetString( "Ping" );
		UTEST_THROW( lhs * rhs );

		lhs.SetArray( { Value::CValue( 1 ) } );
		UTEST_THROW( lhs * rhs );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Array values" )
	{
		Value::CValue val1( 5 );
		Value::CValue val2( 10 );

		std::vector< Value::CValue > valueList = { val1, val2 };
		Value::CValue valArray( valueList );

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

		valueList = { Value::CValue( 20 ), Value::CValue( "a string" ) };
		Value::CValue valArray2( valueList );
		valArray.ArrayConcat( valArray2 );

		UTEST_ASSERT( valArray.ArraySize() == 5 );
		UTEST_ASSERT( valArray[ 0 ].GetInt() == 10 );
		UTEST_ASSERT( valArray[ 1 ].GetInt() == 10 );
		UTEST_ASSERT( valArray[ 2 ].GetInt() == 20 );
		UTEST_ASSERT( valArray[ 3 ].GetInt() == 20 );
		UTEST_ASSERT( valArray[ 4 ].GetType() == Value::ValueType::VT_STRING );
		UTEST_ASSERT( valArray[ 4 ].GetString() == "a string" );
		UTEST_THROW( valArray[ 5 ] );

		valueList = { val1, val2 };
		valArray.ArrayPush( valueList );
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

	UTEST_CASE( "Value equality (operator==, operator!=)" )
	{
		Value::CValue int1( 5 );
		Value::CValue int2( 5 );
		Value::CValue int3( 10 );

		Value::CValue string1( "string" );
		Value::CValue string2( "string" );
		Value::CValue string3( "not string" );

		Value::CValue double1( 5.0 );
		Value::CValue double2( 5.0 );
		Value::CValue double3( 10.0 );

		Value::CValue array1( (std::vector<Value::CValue>){1, 2} );
		Value::CValue array2( (std::vector<Value::CValue>){1, 2} );

		Value::CValue undefined1;
		Value::CValue undefined2;

		UTEST_ASSERT( ( int1 == int2 ) == true );
		UTEST_ASSERT( ( int1 != int2 ) == false );
		UTEST_ASSERT( ( int1 == undefined1 ) == false );

		UTEST_ASSERT( ( int1 == int3 ) == false );
		UTEST_ASSERT( ( int1 != int3 ) == true );

		UTEST_ASSERT( ( string1 == string2 ) == true );
		UTEST_ASSERT( ( string1 != string2 ) == false );
		UTEST_ASSERT( ( string1 == undefined1 ) == false );

		UTEST_ASSERT( ( string1 == string3 ) == false );
		UTEST_ASSERT( ( string1 != string3 ) == true );

		UTEST_ASSERT( ( double1 == double2 ) == true );
		UTEST_ASSERT( ( double1 != double2 ) == false );
		UTEST_ASSERT( ( double1 == undefined1 ) == false );

		UTEST_ASSERT( ( double1 == double3 ) == false );
		UTEST_ASSERT( ( double1 != double3 ) == true );

		// Arrays don't have equality
		UTEST_ASSERT( ( array1 == array2 ) == false );
		UTEST_ASSERT( ( array1 != array2 ) == true );

		UTEST_ASSERT( ( undefined1 == undefined2 ) == true );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_END();
}
