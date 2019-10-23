#include "UnitTests.h"
#include "Tests.h"

#include "Lexer.h"
#include "Parser.h"
#include "Runtime.h"

void RunRuntimeTests()
{
	UTEST_BEGIN( "Parser Tests" );

	AST::PushTrackAlloc();

	UTEST_CASE( "Simple math" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			1 + 1; 										\
			1 + (2 + 2) + 5;							\
		" ) );

		auto context = Runtime::CreateDefaultContext( true );
		auto results = Runtime::Execute( syntaxTree, context );

		UTEST_ASSERT( results.size() == 2 );
		UTEST_ASSERT( results[ 0 ].m_Value == Value::CValue( 2 ) );
		UTEST_ASSERT( results[ 1 ].m_Value == Value::CValue( 10 ) );

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
