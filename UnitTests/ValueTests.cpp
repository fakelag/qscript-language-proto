#include "Tests.h"
#include "UnitTests.h"
#include "Value.h"

void RunValueTests()
{
	UTEST_BEGIN( "Lexer Tests" );

	UTEST_CASE( "Creating a string value" )
	{
		auto value = new Value::CValue( "string value" );

		UTEST_ASSERT( value->GetType() == Value::VT_STRING );
		UTEST_ASSERT( value->GetString() == "string value" );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Switching value type on runtime" )
	{
		auto value = new Value::CValue( "string value" );

		UTEST_ASSERT( value->GetType() == Value::VT_STRING );
		UTEST_ASSERT( value->GetString() == "string value" );

		value->SetInt( 42 );
		UTEST_ASSERT( value->GetType() == Value::VT_INTEGER );
		UTEST_ASSERT( value->GetInt() == 42 );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_END();
}
