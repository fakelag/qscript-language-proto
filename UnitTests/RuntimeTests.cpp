#include "UnitTests.h"
#include "Tests.h"

#include "Lexer.h"
#include "Parser.h"
#include "Runtime.h"

void RunRuntimeTests()
{
	UTEST_BEGIN( "Runtime Tests" );

	AST::PushTrackAlloc();

	UTEST_CASE( "Simple math" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			1 + 1; 										\
			1 + (2 + 2) + 5;							\
		" ) );

		Runtime::CContext context;
		Runtime::CreateDefaultContext( true, true, &context );

		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( results.size() == 2 );
		UTEST_ASSERT( results[ 0 ].m_Value == Value::CValue( 2 ) );
		UTEST_ASSERT( results[ 1 ].m_Value == Value::CValue( 10 ) );

		context.Release();
		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Debugging flags" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			__setFlag();								\
			__setFlag();								\
		" ) );

		Runtime::CContext context;
		Runtime::CreateDefaultContext( true, true, &context );

		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( results.size() == 2 );
		UTEST_ASSERT( context.m_Flag == 2 );
		UTEST_ASSERT( results[ 0 ].m_Value == Value::CValue( 1 ) );
		UTEST_ASSERT( results[ 1 ].m_Value == Value::CValue( 2 ) );

		context.Release();
		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Basic arithmetics (Order of operations)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			(18 - 3) * 10 + 2;																					\
			6 * 6 + 7 * 5;																						\
			146 + 7 * 14 - 31;																					\
			4 * 2 + -(40 / 20) * 3;																				\
			-66 - 30 / 10 * (4 / 2) + 5;																		\
			-(3 * ((2 + -4) - 4) / -1 * (((-(3 - 2) + ((2) -2) - 4) * 2)) * 2 + -(6 / (1 + 1 ) - 2 * 2)); 		\
		" ) );

		Runtime::CContext context;
		Runtime::CreateDefaultContext( true, true, &context );

		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( results.size() == 6 );
		UTEST_ASSERT( results[ 0 ].m_Value == Value::CValue( (18 - 3) * 10 + 2 ) );
		UTEST_ASSERT( results[ 1 ].m_Value == Value::CValue( 6 * 6 + 7 * 5 ) );
		UTEST_ASSERT( results[ 2 ].m_Value == Value::CValue( 146 + 7 * 14 - 31 ) );
		UTEST_ASSERT( results[ 3 ].m_Value == Value::CValue( 4 * 2 + -(40 / 20) * 3 ) );
		UTEST_ASSERT( results[ 4 ].m_Value == Value::CValue( -66 - 30 / 10 * (4 / 2) + 5 ) );
		UTEST_ASSERT( results[ 5 ].m_Value == Value::CValue( -(3 * ((2 + -4) - 4) / -1 * (((-(3 - 2) + ((2) -2) - 4) * 2)) * 2 + -(6 / (1 + 1 ) - 2 * 2)) ) );

		context.Release();
		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Variables and assignment (var, =)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			var a = 4;									\
			var b = a * 2;								\
			var c = a * a;								\
			a = (18 - 3) * 10 + 2.5;					\
			a;											\
			b;											\
			c;											\
		" ) );

		Runtime::CContext context;
		Runtime::CreateDefaultContext( true, true, &context );

		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( results.size() == 7 );
		UTEST_ASSERT( results[ 0 ].m_Value == Value::CValue( 4 ) );
		UTEST_ASSERT( results[ 1 ].m_Value == Value::CValue( 8 ) );
		UTEST_ASSERT( results[ 2 ].m_Value == Value::CValue( 16 ) );
		UTEST_ASSERT( results[ 3 ].m_Value == Value::CValue( 152.5 ) );
		UTEST_ASSERT( results[ 4 ].m_Value == Value::CValue( 152.5 ) );
		UTEST_ASSERT( results[ 5 ].m_Value == Value::CValue( 8 ) );
		UTEST_ASSERT( results[ 6 ].m_Value == Value::CValue( 16 ) );

		context.Release();
		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Parser/AST memory management" )
	{
		auto leakedNodes = AST::AllocatedExpressions();
		UTEST_ASSERT( leakedNodes.size() == 0 );
		UTEST_CASE_CLOSED();
	}( );

	AST::PopAllocTracking();
	UTEST_END();
}
