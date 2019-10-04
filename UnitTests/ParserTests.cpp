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
			&& e.locations()[0].m_SrcToken == "="
			&& e.locations()[1].m_LineNr == 0
			&& e.locations()[1].m_ColNr == 17
			&& e.locations()[1].m_SrcToken == "="
		);

		UTEST_CASE_CLOSED();
	}();

	UTEST_CASE( "Assign operators (=, +=, -=, *=, /=, %=)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "a += 5; a -= 5; a *= 1; a /= 1; a %= 1; a = 4;a += 4 * 1 + 2;" ) );

		UTEST_ASSERT( syntaxTree.size() == 7 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 1 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 2 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 3 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 4 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 5 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 6 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_ASSIGN_ADD );
		UTEST_ASSERT( syntaxTree[ 2 ]->Symbol() == Grammar::Symbol::S_ASSIGN_MUL );
		UTEST_ASSERT( syntaxTree[ 1 ]->Symbol() == Grammar::Symbol::S_ASSIGN_SUB );
		UTEST_ASSERT( syntaxTree[ 3 ]->Symbol() == Grammar::Symbol::S_ASSIGN_DIV );
		UTEST_ASSERT( syntaxTree[ 4 ]->Symbol() == Grammar::Symbol::S_ASSIGN_MOD );
		UTEST_ASSERT( syntaxTree[ 5 ]->Symbol() == Grammar::Symbol::S_ASSIGN );
		UTEST_ASSERT( syntaxTree[ 6 ]->Symbol() == Grammar::Symbol::S_ASSIGN_ADD );

		UTEST_ASSERT(
			static_cast<AST::IExpression*>(
				static_cast<AST::CComplexExpression*>( syntaxTree[ 0 ] )->Lhs()
			)->Type() == AST::ExpressionType::ET_VALUE );

		UTEST_ASSERT(
			static_cast<AST::IExpression*>(
				static_cast<AST::CComplexExpression*>( syntaxTree[ 0 ] )->Rhs()
			)->Type() == AST::ExpressionType::ET_VALUE );

		UTEST_ASSERT(static_cast<AST::CComplexExpression*>( syntaxTree[ 6 ] )->Lhs()->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT(static_cast<AST::CComplexExpression*>( syntaxTree[ 6 ] )->Rhs()->Type() == AST::ExpressionType::ET_COMPLEX );

		UTEST_ASSERT(
			static_cast<AST::CValueExpression*>(
				static_cast<AST::CComplexExpression*>( syntaxTree[ 6 ] )->Lhs()
			)->Type() == AST::ExpressionType::ET_VALUE );

		UTEST_ASSERT(
			static_cast<AST::CComplexExpression*>(
				static_cast<AST::CComplexExpression*>( syntaxTree[ 6 ] )->Rhs()
			)->Symbol() == Grammar::Symbol::S_ADD);

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Equality operators (==, !=, >=, <=, >, <)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "a == 5; a != 5; a >= 5; a <= 5; a < 5; a > 5; a = 4 == 4;" ) );

		UTEST_ASSERT( syntaxTree.size() == 7 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 1 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 2 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 3 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 4 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 5 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 6 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_EQUALS );
		UTEST_ASSERT( syntaxTree[ 1 ]->Symbol() == Grammar::Symbol::S_EQUALS_NOT );
		UTEST_ASSERT( syntaxTree[ 2 ]->Symbol() == Grammar::Symbol::S_MORE_OR_EQUALS );
		UTEST_ASSERT( syntaxTree[ 3 ]->Symbol() == Grammar::Symbol::S_LESS_OR_EQUALS );
		UTEST_ASSERT( syntaxTree[ 4 ]->Symbol() == Grammar::Symbol::S_LESSTHAN );
		UTEST_ASSERT( syntaxTree[ 5 ]->Symbol() == Grammar::Symbol::S_MORETHAN );
		UTEST_ASSERT( syntaxTree[ 6 ]->Symbol() == Grammar::Symbol::S_ASSIGN );

		UTEST_ASSERT(
			static_cast<AST::IExpression*>(
				static_cast<AST::CComplexExpression*>( syntaxTree[ 6 ] )->Lhs()
			)->Type() == AST::ExpressionType::ET_VALUE );

		UTEST_ASSERT(
			static_cast<AST::IExpression*>(
				static_cast<AST::CComplexExpression*>( syntaxTree[ 6 ] )->Rhs()
			)->Type() == AST::ExpressionType::ET_COMPLEX );

		UTEST_ASSERT(
			static_cast<AST::CComplexExpression*>(
				static_cast<AST::CComplexExpression*>( syntaxTree[ 6 ] )->Rhs()
			)->Symbol() == Grammar::Symbol::S_EQUALS );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Logical operators (||, &&, !)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "a = 3 || b; a && b; !a;" ) );

		UTEST_ASSERT( syntaxTree.size() == 3 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( static_cast<AST::CComplexExpression*>( syntaxTree[ 0 ] )->Symbol() == Grammar::Symbol::S_LOGIC_OR );
		UTEST_ASSERT( syntaxTree[ 1 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 2 ]->Type() == AST::ExpressionType::ET_SIMPLE );
		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_LOGIC_OR );
		UTEST_ASSERT( syntaxTree[ 1 ]->Symbol() == Grammar::Symbol::S_LOGIC_AND );
		UTEST_ASSERT( syntaxTree[ 2 ]->Symbol() == Grammar::Symbol::S_LOGIC_NOT );

		UTEST_ASSERT(
			static_cast<AST::IExpression*>(
				static_cast<AST::CComplexExpression*>( syntaxTree[ 0 ] )->Lhs()
			)->Type() == AST::ExpressionType::ET_COMPLEX );

		UTEST_ASSERT(
			static_cast<AST::CComplexExpression*>(
				static_cast<AST::CComplexExpression*>( syntaxTree[ 0 ] )->Lhs()
			)->Rhs()->Type() == AST::ExpressionType::ET_VALUE );

		UTEST_ASSERT(
			static_cast<AST::IExpression*>(
				static_cast<AST::CSimpleExpression*>( syntaxTree[ 2 ] )->Expression()
			)->Type() == AST::ExpressionType::ET_VALUE );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Incremment/Decrement operators (++, --)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "a++ || --b; ++a; a--;" ) );

		UTEST_ASSERT( syntaxTree.size() == 3 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_LOGIC_OR );

		UTEST_ASSERT( syntaxTree[ 1 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 2 ]->Type() == AST::ExpressionType::ET_COMPLEX );

		UTEST_ASSERT( syntaxTree[ 1 ]->Symbol() == Grammar::Symbol::S_INCREMENT );
		UTEST_ASSERT( syntaxTree[ 2 ]->Symbol() == Grammar::Symbol::S_DECREMENT );

		UTEST_ASSERT(
			static_cast< AST::IExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
				)->Type() == AST::ExpressionType::ET_COMPLEX );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
				)->Symbol() == Grammar::Symbol::S_INCREMENT );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()
				)->Symbol() == Grammar::Symbol::S_DECREMENT );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
				)->Lhs()->Symbol() == Grammar::Symbol::S_NAME );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
				)->Rhs() == NULL );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()
				)->Rhs()->Symbol() == Grammar::Symbol::S_NAME );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()
				)->Lhs() == NULL );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Scopes (brackets) ({})" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "{ a = 0; a++; } {} { a; }" ) );

		UTEST_ASSERT( syntaxTree.size() == 3 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_LIST );
		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_SCOPE );
		UTEST_ASSERT( syntaxTree[ 1 ]->Type() == AST::ExpressionType::ET_LIST );
		UTEST_ASSERT( syntaxTree[ 2 ]->Type() == AST::ExpressionType::ET_LIST );

		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List().size() == 2 );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 0 ]->Symbol() == Grammar::Symbol::S_ASSIGN );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 1 ]->Symbol() == Grammar::Symbol::S_INCREMENT );

		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 1 ] )->List().size() == 0 );

		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 2 ] )->List().size() == 1 );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 2 ] )->List()[ 0 ]->Symbol() == Grammar::Symbol::S_NAME );

		UTEST_CASE_CLOSED();
	}( );

	UTEST_END();
}
