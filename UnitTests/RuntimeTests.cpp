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

	UTEST_CASE( "Function calls & arguments" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			function a() { __setFlag(); }				\
			function b(x) { 2 * x; }					\
			function c(x, y, z) { x + y * z; }			\
			a();										\
			b(2);										\
			c(1, 2, 3);									\
		" ) );

		Runtime::CContext context;
		Runtime::CreateDefaultContext( true, true, &context );

		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( results.size() == 6 );
		UTEST_ASSERT( results[ 4 ].m_Value == Value::CValue( 4 ) );
		UTEST_ASSERT( results[ 5 ].m_Value == Value::CValue( 7 ) );

		context.Release();
		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Function scope access" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			var globalVar = 20;							\
			function a()								\
			{											\
				globalVar + 1;							\
			}											\
			a();										\
		" ) );

		Runtime::CContext context;
		Runtime::CreateDefaultContext( true, true, &context );

		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( results.size() == 3 );
		UTEST_ASSERT( results[ 2 ].m_Value == Value::CValue( 21 ) );

		context.Release();
		AST::FreeTree( syntaxTree );

		syntaxTree = Parser::Parse( Lexer::Parse( "		\
			function a(x)								\
			{											\
				var localVar = x + 1;					\
				localVar;								\
			}											\
			a(20);										\
		" ) );

		Runtime::CreateDefaultContext( true, true, &context );

		results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( results.size() == 2 );
		UTEST_ASSERT( results[ 1 ].m_Value == Value::CValue( 21 ) );

		context.Release();
		AST::FreeTree( syntaxTree );

		syntaxTree = Parser::Parse( Lexer::Parse( "		\
			function b() { localVar; }					\
			function a(x)								\
			{											\
				var localVar = x + 1;					\
				b();									\
			}											\
			a(20);										\
		" ) );

		Runtime::CreateDefaultContext( true, true, &context );

		UTEST_THROW_EXCEPTION(
			Runtime::Execute( syntaxTree, context ),
			const RuntimeException& e,
			true );

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
