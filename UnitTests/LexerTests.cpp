#include "UnitTests.h"
#include "Tests.h"

#include "Lexer.h"

void RunLexerTests()
{
	UTEST_BEGIN( "Lexer Tests" );

	UTEST_CASE( "Generate symbols from a sentence" )
	{
		auto symbols = Lexer::Parse( "5 + 3* 1- 2;" );

		UTEST_ASSERT( symbols.size() == 8 );
		UTEST_ASSERT( symbols[ 0 ].m_Token == "5" );
		UTEST_ASSERT( symbols[ 1 ].m_Token == "+" );
		UTEST_ASSERT( symbols[ 5 ].m_Token == "-" );
		UTEST_ASSERT( symbols[ 3 ].m_LBP == Grammar::LBP_ARITHMETIC_2 );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_END();
}
