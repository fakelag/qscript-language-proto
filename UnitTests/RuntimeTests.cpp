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

	UTEST_CASE( "Modulo operator (%)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			5 % 2; 										\
			1 % 1;										\
			2 % 4;										\
			5.2 % 2.4; 									\
			1.5 % 1.5;									\
			100.30 % 4.7;								\
		" ) );

		Runtime::CContext context;
		Runtime::CreateDefaultContext( true, true, &context );

		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( results.size() == 6 );
		UTEST_ASSERT( results[ 0 ].m_Value == Value::CValue( 5 % 2 ) );
		UTEST_ASSERT( results[ 1 ].m_Value == Value::CValue( 1 % 1 ) );
		UTEST_ASSERT( results[ 2 ].m_Value == Value::CValue( 2 % 4 ) );
		UTEST_ASSERT( results[ 3 ].m_Value == Value::CValue( std::fmodf( 5.2, 2.4 ) ) );
		UTEST_ASSERT( results[ 4 ].m_Value == Value::CValue( std::fmodf( 1.5, 1.5 ) ) );
		UTEST_ASSERT( results[ 5 ].m_Value == Value::CValue( std::fmodf( 100.30, 4.7 ) ) );

		context.Release();
		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Power operator (**)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			5 ** 2; 									\
			1 ** 1;										\
			2 ** 4;										\
			5.2 ** 2.4; 								\
			1.5 ** 1.5;									\
			100.30 ** 4.7;								\
		" ) );

		Runtime::CContext context;
		Runtime::CreateDefaultContext( true, true, &context );

		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( results.size() == 6 );
		UTEST_ASSERT( results[ 0 ].m_Value == Value::CValue( std::pow( 5, 2 ) ) );
		UTEST_ASSERT( results[ 1 ].m_Value == Value::CValue( std::pow( 1, 1 ) ) );
		UTEST_ASSERT( results[ 2 ].m_Value == Value::CValue( std::pow( 2, 4 ) ) );
		UTEST_ASSERT( results[ 3 ].m_Value == Value::CValue( std::pow( 5.2, 2.4 ) ) );
		UTEST_ASSERT( results[ 4 ].m_Value == Value::CValue( std::pow( 1.5, 1.5 ) ) );
		UTEST_ASSERT( results[ 5 ].m_Value == Value::CValue( std::pow( 100.30, 4.7 ) ) );

		context.Release();
		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Equality operator (==, !=)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			var a = 10 == 10;							\
			var b = 11 == 10;							\
			a;											\
			b;											\
		" ) );

		Runtime::CContext context;
		Runtime::CreateDefaultContext( true, true, &context );

		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( results.size() == 4 );
		UTEST_ASSERT( results[ 2 ].m_Value == Value::CValue( true ) );
		UTEST_ASSERT( results[ 3 ].m_Value == Value::CValue( false ) );

		context.Release();
		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "If operator (if)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			var a = 10;									\
			var b = 5;									\
			if (a == 10) {								\
				__setFlag();							\
			}											\
			if (a != 10) {								\
				__setFlag();							\
			}											\
		" ) );

		Runtime::CContext context;
		Runtime::CreateDefaultContext( true, true, &context );

		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( context.m_Flag == 1 );

		context.Release();
		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "While operator (while)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			var a = 10;									\
			var b = 0;									\
			while (a > 5)								\
			{											\
				__setFlag();							\
				b = b + 2;								\
				a = a - 1;								\
			}											\
			b;											\
		" ) );

		Runtime::CContext context;
		Runtime::CreateDefaultContext( true, true, &context );

		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( context.m_Flag == 5 );
		UTEST_ASSERT( results[ 3 ].m_Value == Value::CValue( 10 ) );

		context.Release();
		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "For operator (for)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			var a = 4;									\
			for (var i = 0; i < 20 * 20; i = i + 1)		\
			{											\
				__setFlag();							\
				a = i + a;								\
			}											\
			a;											\
		" ) );

		Runtime::CContext context;
		Runtime::CreateDefaultContext( true, true, &context );

		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( context.m_Flag == 400 );
		UTEST_ASSERT( results[ 2 ].m_Value.GetInt() == 79804 );

		context.Release();
		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Assign operators (+=, -=, /=, *=, %=)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			var a = 6;									\
			var b = 6;									\
			var c = 6;									\
			var d = 6;									\
			var e = 6;									\
			a += 2;										\
			b -= 2;										\
			c /= 2;										\
			d *= 2.7;									\
			e %= 2;										\
			a;											\
			b;											\
			c;											\
			d;											\
			e;											\
		" ) );

		Runtime::CContext context;
		Runtime::CreateDefaultContext( true, true, &context );

		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( results.size() == 15 );
		UTEST_ASSERT( results[ 5 ].m_Value.GetInt() == 8 );
		UTEST_ASSERT( results[ 6 ].m_Value.GetInt() == 4 );
		UTEST_ASSERT( results[ 7 ].m_Value.GetInt() == 3 );
		UTEST_ASSERT( results[ 8 ].m_Value.GetDouble() == 6 * 2.7 );
		UTEST_ASSERT( results[ 9 ].m_Value.GetInt() == 0 );
		UTEST_ASSERT( results[ 10 ].m_Value.GetInt() == 8 );
		UTEST_ASSERT( results[ 11 ].m_Value.GetInt() == 4 );
		UTEST_ASSERT( results[ 12 ].m_Value.GetInt() == 3 );
		UTEST_ASSERT( results[ 13 ].m_Value.GetDouble() == 6 * 2.7 );
		UTEST_ASSERT( results[ 14 ].m_Value.GetInt() == 0 );

		context.Release();
		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Logical operators (||, &&, !)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			var a = 1;									\
			var b = 2;									\
			var c = 3;									\
			var d = a || b;								\
			var e = a && b;								\
			var f = !a;									\
			if ( a == b || b == c ) __setFlag();		\
			if ( a == b || b == b ) __setFlag();		\
			if ( a == a || b == c ) __setFlag();		\
			if ( a == b && b == c ) __setFlag();		\
			if ( a == b && b == b ) __setFlag();		\
			if ( a == b && b == c ) __setFlag();		\
			if ( a == a && b == b ) __setFlag();		\
			if ( d == a ) __setFlag();					\
			if ( f ) __setFlag();						\
			if ( !!e ) __setFlag();						\
		" ) );

		Runtime::CContext context;
		Runtime::CreateDefaultContext( true, true, &context );

		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( context.m_Flag == 5 );

		context.Release();
		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Increment/Decrement operators (++, --)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			var a = 1;									\
			var b = 1;									\
			var c = 1.5;								\
			++a;										\
			a++;										\
			--b;										\
			b--;										\
			--c;										\
			a;											\
			b;											\
			c;											\
		" ) );

		Runtime::CContext context;
		Runtime::CreateDefaultContext( true, true, &context );

		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( results[ 3 ].m_Value == Value::CValue( 2 ) );
		UTEST_ASSERT( results[ 4 ].m_Value == Value::CValue( 2 ) );
		UTEST_ASSERT( results[ 5 ].m_Value == Value::CValue( 0 ) );
		UTEST_ASSERT( results[ 6 ].m_Value == Value::CValue( 0 ) );
		UTEST_ASSERT( results[ 7 ].m_Value == Value::CValue( 0.5 ) );
		UTEST_ASSERT( results[ 8 ].m_Value == Value::CValue( 3 ) );
		UTEST_ASSERT( results[ 9 ].m_Value == Value::CValue( -1 ) );
		UTEST_ASSERT( results[ 10 ].m_Value == Value::CValue( 0.5 ) );

		context.Release();
		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Return operator (return)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			function a(x) {								\
				if (x <= 1)	{							\
					{									\
						return 1;						\
					}									\
				}										\
				return x * x;							\
			}											\
			function b() {								\
				return;									\
				__setFlag();							\
			}											\
			a(0);										\
			a(5);										\
			b();										\
		" ) );

		Runtime::CContext context;
		Runtime::CreateDefaultContext( true, true, &context );

		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( results.size() == 5 );
		UTEST_ASSERT( context.m_Flag == 0 );
		UTEST_ASSERT( results[ 2 ].m_Value == Value::CValue( 1 ) );
		UTEST_ASSERT( results[ 3 ].m_Value == Value::CValue( 25 ) );
		UTEST_ASSERT( results[ 4 ].m_Value.IsInitialized() == false );

		context.Release();
		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Break operator (break)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			var i = 0;									\
			var j = 0;									\
			for (; i < 20; ++i) {						\
				if (i == 15)							\
					break;								\
				__setFlag();							\
			}											\
			while (j < 100 * 100) {						\
				if (j >= 5) { break; } ++j;				\
				__setFlag();							\
			}											\
			var x = while (1) {							\
				__setFlag();							\
				break j;								\
			}											\
			i;											\
			j;											\
			x;											\
		" ) );

		Runtime::CContext context;
		Runtime::CreateDefaultContext( true, true, &context );

		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( results.size() == 8 );
		UTEST_ASSERT( context.m_Flag == 21 );
		UTEST_ASSERT( results[ 5 ].m_Value.GetInt() == 15 );
		UTEST_ASSERT( results[ 6 ].m_Value.GetInt() == 5 );
		UTEST_ASSERT( results[ 7 ].m_Value.GetInt() == 5 );

		context.Release();
		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "True/False constants (true, false)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			var a = true;								\
			var b = false;								\
			var c = false || true;						\
			var d = false && true;						\
			if (a) __setFlag();							\
			if (!b) __setFlag();						\
			if (c) __setFlag();							\
			if (!d) __setFlag();						\
			if (true) __setFlag();						\
			if (b == false) __setFlag();				\
		" ) );

		Runtime::CContext context;
		Runtime::CreateDefaultContext( true, true, &context );

		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( context.m_Flag == 6 );

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
