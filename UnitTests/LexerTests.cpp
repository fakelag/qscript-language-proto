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
		UTEST_ASSERT( symbols[ 0 ].m_Symbol			== Grammar::S_INTCNST );
		UTEST_ASSERT( symbols[ 0 ].m_Token			== "5" );
		UTEST_ASSERT( symbols[ 1 ].m_Symbol			== Grammar::S_ADD );
		UTEST_ASSERT( symbols[ 5 ].m_Symbol			== Grammar::S_SUB );
		UTEST_ASSERT( symbols[ 3 ].m_LBP			== Grammar::LBP_ARITHMETIC_2 );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Ignore tabs and newlines in the code" )
	{
		auto symbols = Lexer::Parse( "\t5 \n+ \t3* 1\n\n- 2; \n				\
				1 + 2;														\
				2 + 3;														\
				3 + 4;														\
			" );

		UTEST_ASSERT( symbols.size() == 20 );
		UTEST_ASSERT( symbols[ 0 ].m_Token				== "5" );
		UTEST_ASSERT( symbols[ 1 ].m_Symbol				== Grammar::S_ADD );
		UTEST_ASSERT( symbols[ 5 ].m_Symbol				== Grammar::S_SUB );
		UTEST_ASSERT( symbols[ 3 ].m_LBP				== Grammar::LBP_ARITHMETIC_2 );
		UTEST_ASSERT( symbols[ 12 ].m_Token				== "2" );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Multicharacter symbols" )
	{
		auto symbols = Lexer::Parse( "5 + 2 ** 3 * 2;" );

		UTEST_ASSERT( symbols.size() == 8 );
		UTEST_ASSERT( symbols[ 3 ].m_Symbol == Grammar::S_POW );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Debug info generation" )
	{
		auto symbols = Lexer::Parse( "5 \n+ 2 ** 3\n * 2+2\n*1;" );

		UTEST_ASSERT( symbols.size() == 12 );
		UTEST_ASSERT( symbols[ 0 ].m_Location.m_ColNr		== 0 );
		UTEST_ASSERT( symbols[ 0 ].m_Location.m_LineNr		== 0 );
		UTEST_ASSERT( symbols[ 1 ].m_Location.m_ColNr		== 0 );
		UTEST_ASSERT( symbols[ 1 ].m_Location.m_LineNr		== 1 );
		UTEST_ASSERT( symbols[ 2 ].m_Location.m_ColNr		== 2 );
		UTEST_ASSERT( symbols[ 2 ].m_Location.m_LineNr		== 1 );
		UTEST_ASSERT( symbols[ 3 ].m_Location.m_ColNr		== 4 );
		UTEST_ASSERT( symbols[ 3 ].m_Location.m_LineNr		== 1 );
		UTEST_ASSERT( symbols[ 6 ].m_Location.m_ColNr		== 3 );
		UTEST_ASSERT( symbols[ 6 ].m_Location.m_LineNr		== 2 );
		UTEST_ASSERT( symbols[ 7 ].m_Location.m_ColNr		== 4 ); // +
		UTEST_ASSERT( symbols[ 7 ].m_Location.m_LineNr		== 2 );
		UTEST_ASSERT( symbols[ 8 ].m_Location.m_ColNr		== 5 ); // 2
		UTEST_ASSERT( symbols[ 8 ].m_Location.m_LineNr		== 2 );
		UTEST_ASSERT( symbols[ 9 ].m_Location.m_ColNr		== 0 ); // *
		UTEST_ASSERT( symbols[ 9 ].m_Location.m_LineNr		== 3 );
		UTEST_ASSERT( symbols[ 10 ].m_Location.m_ColNr		== 1 ); // 1
		UTEST_ASSERT( symbols[ 10 ].m_Location.m_LineNr	== 3 );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Name and integer constants" )
	{
		auto symbols = Lexer::Parse( "abcdefg = 4 + abc;" );

		UTEST_ASSERT( symbols.size() == 6 );
		UTEST_ASSERT( symbols[ 0 ].m_Symbol			== Grammar::S_NAME );
		UTEST_ASSERT( symbols[ 1 ].m_Symbol			== Grammar::S_ASSIGN );
		UTEST_ASSERT( symbols[ 2 ].m_Symbol			== Grammar::S_INTCNST );
		UTEST_ASSERT( symbols[ 3 ].m_Symbol			== Grammar::S_ADD );
		UTEST_ASSERT( symbols[ 4 ].m_Symbol			== Grammar::S_NAME );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_END();
}
