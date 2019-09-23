#include "UnitTests.h"
#include "Tests.h"

#include "Exception.h"
#include "Grammar.h"
#include "Parser.h"

void RunParserTests()
{
	UTEST_BEGIN( "Parser Tests" );

	UTEST_CASE( "Generate symbols from a sentence" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "5 + 3* 1- 2;" ) );

		UTEST_ASSERT( syntaxTree.size() == 1 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( static_cast<AST::CComplexExpression*>( syntaxTree[ 0 ] )->Lhs()->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( static_cast<AST::CComplexExpression*>( syntaxTree[ 0 ] )->Rhs()->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT(
			static_cast<AST::CComplexExpression*>(
				static_cast<AST::CComplexExpression*>( syntaxTree[ 0 ] )->Lhs()
			)->Lhs()->Type() == AST::ExpressionType::ET_VALUE );

		UTEST_ASSERT(
			static_cast<AST::CComplexExpression*>(
				static_cast<AST::CComplexExpression*>( syntaxTree[ 0 ] )->Lhs()
			)->Rhs()->Type() == AST::ExpressionType::ET_COMPLEX );

		UTEST_ASSERT(
			static_cast<AST::CComplexExpression*>(
				static_cast<AST::CComplexExpression*>(
					static_cast<AST::CComplexExpression*>( syntaxTree[ 0 ] )->Lhs()
				)->Rhs()
			)->Symbol() == Grammar::Symbol::S_MUL );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Throw on parsing errors" )
	{
		UTEST_THROW(
			auto syntaxTree = Parser::Parse( Lexer::Parse( "5 + 3* * 1- 2;" ) );
		);

		UTEST_THROW(
			auto syntaxTree = Parser::Parse( Lexer::Parse( "5 + 3 * 1- 2" ) );
		);

		UTEST_THROW_EXCEPTION(
			auto syntaxTree = Parser::Parse( Lexer::Parse( "var a = = 1;" ) );
			, const ParseException& e, e.errors().size() == 1
		);

		UTEST_THROW_EXCEPTION(
			auto syntaxTree = Parser::Parse( Lexer::Parse( "var a = = 1; a = = 2;" ) );
			, const ParseException& e,
			e.errors().size() == 2
			&& e.locations()[0].m_LineNr == 0
			&& e.locations()[0].m_ColNr == 8
			&& e.locations()[0].m_Length == 1
			&& e.locations()[1].m_LineNr == 0
			&& e.locations()[1].m_ColNr == 17
			&& e.locations()[1].m_Length == 1
		);

		UTEST_CASE_CLOSED();
	}();

	UTEST_END();
}
