#include "UnitTests.h"
#include "Tests.h"

#include "Exception.h"
#include "Grammar.h"
#include "Parser.h"

void RunParserTests()
{
	UTEST_BEGIN( "Parser Tests" );

	AST::PushTrackAlloc();

	UTEST_CASE( "Generate symbols from a sentence" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "5 + 3* 1- 2;" ) );

		UTEST_ASSERT( syntaxTree.size() == 1 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
				)->Lhs()->Type() == AST::ExpressionType::ET_VALUE );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
				)->Rhs()->Type() == AST::ExpressionType::ET_COMPLEX );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >(
					static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
					)->Rhs()
				)->Symbol() == Grammar::Symbol::S_MUL );

		AST::FreeTree( syntaxTree );
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
			&& e.locations()[ 0 ].m_LineNr == 0
			&& e.locations()[ 0 ].m_ColNr == 8
			&& e.locations()[ 0 ].m_SrcToken == "="
			&& e.locations()[ 1 ].m_LineNr == 0
			&& e.locations()[ 1 ].m_ColNr == 17
			&& e.locations()[ 1 ].m_SrcToken == "="
			);

		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Decimal constants" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			1.1;										\
			1.5;										\
			5.5555;										\
			1.01;										\
			1.000101;									\
		" ) );

		UTEST_ASSERT( syntaxTree.size() == 5 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT( syntaxTree[ 1 ]->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT( syntaxTree[ 2 ]->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT( syntaxTree[ 3 ]->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT( syntaxTree[ 4 ]->Type() == AST::ExpressionType::ET_VALUE );

		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_DBLCNST );
		UTEST_ASSERT( syntaxTree[ 1 ]->Symbol() == Grammar::Symbol::S_DBLCNST );
		UTEST_ASSERT( syntaxTree[ 2 ]->Symbol() == Grammar::Symbol::S_DBLCNST );
		UTEST_ASSERT( syntaxTree[ 3 ]->Symbol() == Grammar::Symbol::S_DBLCNST );
		UTEST_ASSERT( syntaxTree[ 4 ]->Symbol() == Grammar::Symbol::S_DBLCNST );

		UTEST_ASSERT( static_cast< AST::CValueExpression* >( syntaxTree[ 0 ] )->Value().GetDouble() == 1.1 );
		UTEST_ASSERT( static_cast< AST::CValueExpression* >( syntaxTree[ 1 ] )->Value().GetDouble() == 1.5 );
		UTEST_ASSERT( static_cast< AST::CValueExpression* >( syntaxTree[ 2 ] )->Value().GetDouble() == 5.5555 );
		UTEST_ASSERT( static_cast< AST::CValueExpression* >( syntaxTree[ 3 ] )->Value().GetDouble() == 1.01 );
		UTEST_ASSERT( static_cast< AST::CValueExpression* >( syntaxTree[ 4 ] )->Value().GetDouble() == 1.000101 );

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

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
			static_cast< AST::IExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
				)->Type() == AST::ExpressionType::ET_VALUE );

		UTEST_ASSERT(
			static_cast< AST::IExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()
				)->Type() == AST::ExpressionType::ET_VALUE );

		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 6 ] )->Lhs()->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 6 ] )->Rhs()->Type() == AST::ExpressionType::ET_COMPLEX );

		UTEST_ASSERT(
			static_cast< AST::CValueExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 6 ] )->Lhs()
				)->Type() == AST::ExpressionType::ET_VALUE );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 6 ] )->Rhs()
				)->Symbol() == Grammar::Symbol::S_ADD );

		AST::FreeTree( syntaxTree );
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
			static_cast< AST::IExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 6 ] )->Lhs()
				)->Type() == AST::ExpressionType::ET_VALUE );

		UTEST_ASSERT(
			static_cast< AST::IExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 6 ] )->Rhs()
				)->Type() == AST::ExpressionType::ET_COMPLEX );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 6 ] )->Rhs()
				)->Symbol() == Grammar::Symbol::S_EQUALS );

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Logical operators (||, &&, !)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "a = 3 || b; a && b; !a;" ) );

		UTEST_ASSERT( syntaxTree.size() == 3 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Symbol() == Grammar::Symbol::S_LOGIC_OR );
		UTEST_ASSERT( syntaxTree[ 1 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 2 ]->Type() == AST::ExpressionType::ET_SIMPLE );
		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_LOGIC_OR );
		UTEST_ASSERT( syntaxTree[ 1 ]->Symbol() == Grammar::Symbol::S_LOGIC_AND );
		UTEST_ASSERT( syntaxTree[ 2 ]->Symbol() == Grammar::Symbol::S_LOGIC_NOT );

		UTEST_ASSERT(
			static_cast< AST::IExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
				)->Type() == AST::ExpressionType::ET_COMPLEX );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
				)->Rhs()->Type() == AST::ExpressionType::ET_VALUE );

		UTEST_ASSERT(
			static_cast< AST::IExpression* >(
				static_cast< AST::CSimpleExpression* >( syntaxTree[ 2 ] )->Expression()
				)->Type() == AST::ExpressionType::ET_VALUE );

		AST::FreeTree( syntaxTree );
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

		AST::FreeTree( syntaxTree );
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

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Dot operator (.)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "a.b = 4.2; a1.b2.c3.d4 || ++a1.b2; 4.5; 444.952;" ) );

		UTEST_ASSERT( syntaxTree.size() == 4 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_ASSIGN );
		UTEST_ASSERT( syntaxTree[ 1 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 1 ]->Symbol() == Grammar::Symbol::S_LOGIC_OR );
		UTEST_ASSERT( syntaxTree[ 2 ]->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT( syntaxTree[ 2 ]->Symbol() == Grammar::Symbol::S_DBLCNST );
		UTEST_ASSERT( syntaxTree[ 3 ]->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT( syntaxTree[ 3 ]->Symbol() == Grammar::Symbol::S_DBLCNST );

		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()->Symbol() == Grammar::Symbol::S_DOT );
		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
				)->Lhs()->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
				)->Lhs()->Symbol() == Grammar::Symbol::S_NAME );
		UTEST_ASSERT(
			static_cast< AST::CValueExpression* >(
				static_cast< AST::CComplexExpression* >(
					static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
					)->Lhs() )->Value().GetType() == Value::ValueType::VT_STRING );
		UTEST_ASSERT(
			static_cast< AST::CValueExpression* >(
				static_cast< AST::CComplexExpression* >(
					static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
					)->Lhs() )->Value().GetString() == "a" );
		UTEST_ASSERT(
			static_cast< AST::CValueExpression* >(
				static_cast< AST::CComplexExpression* >(
					static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
					)->Rhs() )->Value().GetString() == "b" );
		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()
				)->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()->Symbol() == Grammar::Symbol::S_DBLCNST );
		UTEST_ASSERT(
			static_cast< AST::CValueExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()
				)->Value().GetType() == Value::ValueType::VT_DOUBLE );
		UTEST_ASSERT(
			static_cast< AST::CValueExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()
				)->Value().GetDouble() == 4.2 );

		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Lhs()->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Lhs()->Symbol() == Grammar::Symbol::S_DOT );
		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Lhs()
				)->Rhs()->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT(
			static_cast< AST::CValueExpression* >(
				static_cast< AST::CComplexExpression* >(
					static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Lhs()
					)->Rhs() )->Value().GetType() == Value::ValueType::VT_STRING );
		UTEST_ASSERT(
			static_cast< AST::CValueExpression* >(
				static_cast< AST::CComplexExpression* >(
					static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Lhs()
					)->Rhs() )->Value().GetString() == "d4" );
		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Lhs()
				)->Lhs()->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Lhs()
				)->Lhs()->Symbol() == Grammar::Symbol::S_DOT );
		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >(
					static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Lhs()
					)->Lhs() )->Rhs()->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT(
			static_cast< AST::CValueExpression* >(
				static_cast< AST::CComplexExpression* >(
					static_cast< AST::CComplexExpression* >(
						static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Lhs()
						)->Lhs() )->Rhs() )->Value().GetString() == "c3" );

		UTEST_ASSERT( static_cast< AST::CValueExpression* >( syntaxTree[ 2 ] )->Value().GetType() == Value::ValueType::VT_DOUBLE );
		UTEST_ASSERT( static_cast< AST::CValueExpression* >( syntaxTree[ 2 ] )->Value().GetDouble() == 4.5 );
		UTEST_ASSERT( static_cast< AST::CValueExpression* >( syntaxTree[ 3 ] )->Value().GetType() == Value::ValueType::VT_DOUBLE );
		UTEST_ASSERT( static_cast< AST::CValueExpression* >( syntaxTree[ 3 ] )->Value().GetDouble() == 444.952 );

		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Rhs()->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Rhs()->Symbol() == Grammar::Symbol::S_INCREMENT );

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Other separators (, ;)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( ";;; ;;; ; ;;" ) );

		UTEST_ASSERT( syntaxTree.size() == 0 );

		AST::FreeTree( syntaxTree );
		syntaxTree = Parser::Parse( Lexer::Parse( "a, b, c + 1, c * 2 == 4;" ) );

		UTEST_ASSERT( syntaxTree.size() == 1 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_LIST );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List().size() == 4 );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 0 ]->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT(
			static_cast< AST::CValueExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 0 ]
				)->Value().GetType() == Value::ValueType::VT_STRING );
		UTEST_ASSERT(
			static_cast< AST::CValueExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 0 ]
				)->Value().GetString() == "a" );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 2 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 2 ]->Symbol() == Grammar::Symbol::S_ADD );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 3 ]->Symbol() == Grammar::Symbol::S_EQUALS );

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Array operator ([])" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "var a = []; [1, 2, 3, a + 2]; [1, [2, 3], 4]; [ [ [1, 2], [1, 2] ], [ [1, 2 ] ] ];" ) );

		UTEST_ASSERT( syntaxTree.size() == 4 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 1 ]->Type() == AST::ExpressionType::ET_LIST );
		UTEST_ASSERT( syntaxTree[ 2 ]->Type() == AST::ExpressionType::ET_LIST );
		UTEST_ASSERT( syntaxTree[ 3 ]->Type() == AST::ExpressionType::ET_LIST );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 1 ] )->List().size() == 4 );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 2 ] )->List().size() == 3 );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 3 ] )->List().size() == 2 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()
				)->List().size() == 0 );

		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 1 ] )->List()[ 0 ]->Symbol() == Grammar::Symbol::S_INTCNST );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 1 ] )->List()[ 1 ]->Symbol() == Grammar::Symbol::S_INTCNST );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 1 ] )->List()[ 2 ]->Symbol() == Grammar::Symbol::S_INTCNST );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 1 ] )->List()[ 3 ]->Symbol() == Grammar::Symbol::S_ADD );

		UTEST_ASSERT(
			static_cast< AST::CValueExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 1 ] )->List()[ 0 ]
				)->Value().GetInt() == 1 );

		UTEST_ASSERT(
			static_cast< AST::CValueExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 1 ] )->List()[ 1 ]
				)->Value().GetInt() == 2 );

		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 2 ] )->List()[ 0 ]->Symbol() == Grammar::Symbol::S_INTCNST );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 2 ] )->List()[ 1 ]->Symbol() == Grammar::Symbol::S_ARRAY );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 2 ] )->List()[ 2 ]->Symbol() == Grammar::Symbol::S_INTCNST );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 2 ] )->List()[ 1 ]
				)->List().size() == 2 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 2 ] )->List()[ 1 ]
				)->List()[ 0 ]->Symbol() == Grammar::Symbol::S_INTCNST );

		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 3 ] )->List()[ 0 ]->Symbol() == Grammar::Symbol::S_ARRAY );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 3 ] )->List()[ 0 ]
				)->List()[ 1 ]->Symbol() == Grammar::Symbol::S_ARRAY );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >(
					static_cast< AST::CListExpression* >( syntaxTree[ 3 ] )->List()[ 0 ]
					)->List()[ 1 ] )->List().size() == 2 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >(
					static_cast< AST::CListExpression* >( syntaxTree[ 3 ] )->List()[ 0 ]
					)->List()[ 1 ] )->List()[ 1 ]->Symbol() == Grammar::Symbol::S_INTCNST );

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Access operator ([])" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "a[ 1 ]; a[ b[ 1 + a[ 1 ] ] ];" ) );

		UTEST_ASSERT( syntaxTree.size() == 2 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 1 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()->Symbol() == Grammar::Symbol::S_NAME );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()->Symbol() == Grammar::Symbol::S_INTCNST );

		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Lhs()->Symbol() == Grammar::Symbol::S_NAME );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Rhs()->Symbol() == Grammar::Symbol::S_ACCESS );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Rhs()
				)->Rhs()->Symbol() == Grammar::Symbol::S_ADD );

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Parenthesis (())" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "a * (4 - 2) / 5; print(); print(x); print(x, y + 1, z);" ) );

		UTEST_ASSERT( syntaxTree.size() == 4 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 1 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 2 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 3 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Symbol() == Grammar::Symbol::S_DIV );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()->Symbol() == Grammar::Symbol::S_MUL );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >( 
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
				)->Lhs()->Symbol() == Grammar::Symbol::S_NAME );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
				)->Rhs()->Symbol() == Grammar::Symbol::S_SUB );

		UTEST_ASSERT( syntaxTree[ 1 ]->Symbol() == Grammar::Symbol::S_CALL );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Lhs()->Symbol() == Grammar::Symbol::S_NAME );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 2 ] )->Lhs()->Symbol() == Grammar::Symbol::S_NAME );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 3 ] )->Lhs()->Symbol() == Grammar::Symbol::S_NAME );

		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Rhs()->Symbol() == Grammar::Symbol::S_LIST );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 2 ] )->Rhs()->Symbol() == Grammar::Symbol::S_LIST );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 3 ] )->Rhs()->Symbol() == Grammar::Symbol::S_LIST );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Rhs()
				)->List().size() == 0 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 2 ] )->Rhs()
				)->List().size() == 1 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 3 ] )->Rhs()
				)->List().size() == 3 );

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Boolean operators (true, false)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "true; false;" ) );

		UTEST_ASSERT( syntaxTree.size() == 2 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT( syntaxTree[ 1 ]->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_TRUE );
		UTEST_ASSERT( syntaxTree[ 1 ]->Symbol() == Grammar::Symbol::S_FALSE );

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "If clause (if)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "if ( a > 1 ) a;" ) );

		UTEST_ASSERT( syntaxTree.size() == 1 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_IF );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()->Symbol() == Grammar::Symbol::S_MORETHAN );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()->Symbol() == Grammar::Symbol::S_SCOPE );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()->Type() == AST::ExpressionType::ET_LIST );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()
				)->List().size() == 1 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()
				)->List()[ 0 ]->Symbol() == Grammar::Symbol::S_NAME );

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "While clause (while)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "while ( a > 1 ) a;" ) );

		UTEST_ASSERT( syntaxTree.size() == 1 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_WHILE );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()->Symbol() == Grammar::Symbol::S_MORETHAN );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()->Symbol() == Grammar::Symbol::S_SCOPE );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()->Type() == AST::ExpressionType::ET_LIST );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()
				)->List().size() == 1 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()
				)->List()[ 0 ]->Symbol() == Grammar::Symbol::S_NAME );

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "For clause (for)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "for ( var i = 0; i < 10; ++i) { i; };" ) );

		UTEST_ASSERT( syntaxTree.size() == 1 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_LIST );
		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_FOR );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List().size() == 4 );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 0 ]->Symbol() == Grammar::Symbol::S_ASSIGN );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 1 ]->Symbol() == Grammar::Symbol::S_LESSTHAN );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 2 ]->Symbol() == Grammar::Symbol::S_INCREMENT );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 3 ]->Symbol() == Grammar::Symbol::S_SCOPE );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 3 ]
				)->List().size() == 1 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 3 ]
				)->List()[ 0 ]->Symbol() == Grammar::Symbol::S_NAME );

		AST::FreeTree( syntaxTree );
		syntaxTree = Parser::Parse( Lexer::Parse( "for(var i = 0, var a, x = []; i < 10 && a == true; ++i) { i; };" ) );

		UTEST_ASSERT( syntaxTree.size() == 1 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_LIST );
		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_FOR );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List().size() == 4 );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 0 ]->Symbol() == Grammar::Symbol::S_LIST );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 1 ]->Symbol() == Grammar::Symbol::S_LOGIC_AND );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 2 ]->Symbol() == Grammar::Symbol::S_INCREMENT );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 3 ]->Symbol() == Grammar::Symbol::S_SCOPE );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 3 ]
				)->List().size() == 1 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 3 ]
				)->List()[ 0 ]->Symbol() == Grammar::Symbol::S_NAME );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 0 ]
			)->List().size() == 3 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 0 ]
			)->List()[ 0 ]->Symbol() == Grammar::Symbol::S_ASSIGN );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 0 ]
			)->List()[ 1 ]->Symbol() == Grammar::Symbol::S_VAR );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 0 ]
			)->List()[ 2 ]->Symbol() == Grammar::Symbol::S_ASSIGN );

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Break operator (break)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "while ( a > 1 ) break;" ) );

		UTEST_ASSERT( syntaxTree.size() == 1 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_WHILE );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()->Symbol() == Grammar::Symbol::S_MORETHAN );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()->Symbol() == Grammar::Symbol::S_SCOPE );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()->Type() == AST::ExpressionType::ET_LIST );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()
				)->List().size() == 1 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()
				)->List()[ 0 ]->Symbol() == Grammar::Symbol::S_BREAK );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()
				)->List()[ 0 ]->Type() == AST::ExpressionType::ET_SIMPLE );

		UTEST_ASSERT(
			static_cast< AST::CSimpleExpression*>(
				static_cast< AST::CListExpression* >(
					static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()
					)->List()[ 0 ]
				)->Expression() == NULL );

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Return operator (return)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "return; return 10 + (2*2);" ) );

		UTEST_ASSERT( syntaxTree.size() == 2 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_SIMPLE );
		UTEST_ASSERT( syntaxTree[ 1 ]->Type() == AST::ExpressionType::ET_SIMPLE );
		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_RETURN );
		UTEST_ASSERT( syntaxTree[ 1 ]->Symbol() == Grammar::Symbol::S_RETURN );
		UTEST_ASSERT( static_cast< AST::CSimpleExpression* >( syntaxTree[ 0 ] )->Expression() == NULL );
		UTEST_ASSERT( static_cast< AST::CSimpleExpression* >( syntaxTree[ 1 ] )->Expression()->Symbol() == Grammar::Symbol::S_ADD );

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Function clause (function)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "function f() { a; } function f(a) { a; } function f(a, b, c) { a; }" ) );

		UTEST_ASSERT( syntaxTree.size() == 3 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 1 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 2 ]->Type() == AST::ExpressionType::ET_COMPLEX );

		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_FUNC );
		UTEST_ASSERT( syntaxTree[ 1 ]->Symbol() == Grammar::Symbol::S_FUNC );
		UTEST_ASSERT( syntaxTree[ 2 ]->Symbol() == Grammar::Symbol::S_FUNC );

		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()->Symbol() == Grammar::Symbol::S_FUNCDEF );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Lhs()->Symbol() == Grammar::Symbol::S_FUNCDEF );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 2 ] )->Lhs()->Symbol() == Grammar::Symbol::S_FUNCDEF );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
				)->Lhs()->Symbol() == Grammar::Symbol::S_NAME );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Lhs()
				)->Lhs()->Symbol() == Grammar::Symbol::S_NAME );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 2 ] )->Lhs()
				)->Lhs()->Symbol() == Grammar::Symbol::S_NAME );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
				)->Rhs()->Symbol() == Grammar::Symbol::S_LIST );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Lhs()
				)->Rhs()->Symbol() == Grammar::Symbol::S_LIST );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 2 ] )->Lhs()
				)->Rhs()->Symbol() == Grammar::Symbol::S_LIST );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CComplexExpression* >(
					static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
					)->Rhs()
				)->List().size() == 0 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CComplexExpression* >(
					static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Lhs()
					)->Rhs()
				)->List().size() == 1 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CComplexExpression* >(
					static_cast< AST::CComplexExpression* >( syntaxTree[ 2 ] )->Lhs()
					)->Rhs()
				)->List().size() == 3 );

		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()->Symbol() == Grammar::Symbol::S_FUNCBODY );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Rhs()->Symbol() == Grammar::Symbol::S_FUNCBODY );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 2 ] )->Rhs()->Symbol() == Grammar::Symbol::S_FUNCBODY );

		UTEST_ASSERT(
			static_cast< AST::CListExpression*>(
				static_cast< AST::CSimpleExpression* >(
					static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()
					)->Expression()
				)->List().size() == 1 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CSimpleExpression* >(
					static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Rhs()
					)->Expression()
				)->List().size() == 1 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CSimpleExpression* >(
					static_cast< AST::CComplexExpression* >( syntaxTree[ 1 ] )->Rhs()
					)->Expression()
				)->List().size() == 1 );

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Function prototypes (function)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "function ABC();" ) );

		UTEST_ASSERT( syntaxTree.size() == 1 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_COMPLEX );

		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_FUNC );

		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()->Symbol() == Grammar::Symbol::S_FUNCDEF );
		UTEST_ASSERT( static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()->Symbol() == Grammar::Symbol::S_FUNCBODY );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
				)->Lhs()->Symbol() == Grammar::Symbol::S_NAME );

		UTEST_ASSERT(
			static_cast< AST::CComplexExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Lhs()
				)->Rhs()->Symbol() == Grammar::Symbol::S_LIST );

		UTEST_ASSERT(
			static_cast< AST::CSimpleExpression* >(
				static_cast< AST::CComplexExpression* >( syntaxTree[ 0 ] )->Rhs()
				)->Expression() == NULL );

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Class clause 1" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			class Value 								\
			{											\
														\
			}											\
		" ) );

		UTEST_ASSERT( syntaxTree.size() == 1 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_LIST );
		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_CLASS );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List().size() == 3 );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 0 ]->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 1 ]->Type() == AST::ExpressionType::ET_LIST );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 2 ]->Type() == AST::ExpressionType::ET_LIST );

		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 0 ]->Symbol() == Grammar::Symbol::S_NAME );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 1 ]->Symbol() == Grammar::Symbol::S_LIST );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 2 ]->Symbol() == Grammar::Symbol::S_CLASSBODY );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )
			->List()[ 1 ] )->List().size() == 0 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )
			->List()[ 2 ] )->List().size() == 0 );

		AST::FreeTree( syntaxTree );
		syntaxTree = Parser::Parse( Lexer::Parse( "		\
			class Value : BaseClass						\
			{											\
														\
			}											\
		" ) );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )
			->List()[ 1 ] )->List().size() == 1 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )
			->List()[ 1 ] )->List()[ 0 ]->Symbol() == Grammar::Symbol::S_NAME );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )
			->List()[ 2 ] )->List().size() == 0 );

		AST::FreeTree( syntaxTree );
		syntaxTree = Parser::Parse( Lexer::Parse( "				\
			class Value : BaseClass1, BaseClass2, BaseClass3	\
			{													\
																\
			}													\
		" ) );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )
			->List()[ 1 ] )->List().size() == 3 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )
			->List()[ 1 ] )->List()[ 0 ]->Symbol() == Grammar::Symbol::S_NAME );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )
			->List()[ 1 ] )->List()[ 1 ]->Symbol() == Grammar::Symbol::S_NAME );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )
			->List()[ 1 ] )->List()[ 2 ]->Symbol() == Grammar::Symbol::S_NAME );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )
			->List()[ 2 ] )->List().size() == 0 );

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Class clause 2" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "\
			class Value 								\
			{											\
				function A() {}							\
				function B();							\
				var x;									\
			}											\
		" ) );

		UTEST_ASSERT( syntaxTree.size() == 1 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_LIST );
		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_CLASS );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List().size() == 3 );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 0 ]->Type() == AST::ExpressionType::ET_VALUE );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 1 ]->Type() == AST::ExpressionType::ET_LIST );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 2 ]->Type() == AST::ExpressionType::ET_LIST );

		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 0 ]->Symbol() == Grammar::Symbol::S_NAME );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 1 ]->Symbol() == Grammar::Symbol::S_LIST );
		UTEST_ASSERT( static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )->List()[ 2 ]->Symbol() == Grammar::Symbol::S_CLASSBODY );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )
			->List()[ 1 ] )->List().size() == 0 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )
			->List()[ 2 ] )->List().size() == 3 );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )
			->List()[ 2 ] )->List()[ 0 ]->Symbol() == Grammar::Symbol::S_FUNC );

		UTEST_ASSERT(
			static_cast< AST::CSimpleExpression*>(
				static_cast< AST::CComplexExpression* >(
					static_cast< AST::CListExpression* >(
						static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )
					->List()[ 2 ] )->List()[ 0 ] )->Rhs() )->Expression() != NULL );

		UTEST_ASSERT(
			static_cast< AST::CSimpleExpression*>(
				static_cast< AST::CComplexExpression* >(
					static_cast< AST::CListExpression* >(
						static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )
					->List()[ 2 ] )->List()[ 0 ] )->Rhs() )->Expression()->Symbol() == Grammar::Symbol::S_SCOPE );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )
			->List()[ 2 ] )->List()[ 1 ]->Symbol() == Grammar::Symbol::S_FUNC );

		UTEST_ASSERT(
			static_cast< AST::CSimpleExpression*>(
				static_cast< AST::CComplexExpression* >(
					static_cast< AST::CListExpression* >(
						static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )
					->List()[ 2 ] )->List()[ 1 ] )->Rhs() )->Expression() == NULL );

		UTEST_ASSERT(
			static_cast< AST::CListExpression* >(
				static_cast< AST::CListExpression* >( syntaxTree[ 0 ] )
			->List()[ 2 ] )->List()[ 2 ]->Symbol() == Grammar::Symbol::S_VAR );

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Defer operator (defer)" )
	{
		auto syntaxTree = Parser::Parse( Lexer::Parse( "print(x); defer print(x); defer { print(x); print(y); };" ) );

		UTEST_ASSERT( syntaxTree.size() == 3 );
		UTEST_ASSERT( syntaxTree[ 0 ]->Type() == AST::ExpressionType::ET_COMPLEX );
		UTEST_ASSERT( syntaxTree[ 1 ]->Type() == AST::ExpressionType::ET_SIMPLE );
		UTEST_ASSERT( syntaxTree[ 2 ]->Type() == AST::ExpressionType::ET_SIMPLE );
		UTEST_ASSERT( syntaxTree[ 0 ]->Symbol() == Grammar::Symbol::S_CALL );
		UTEST_ASSERT( syntaxTree[ 1 ]->Symbol() == Grammar::Symbol::S_DEFER );
		UTEST_ASSERT( syntaxTree[ 2 ]->Symbol() == Grammar::Symbol::S_DEFER );
		UTEST_ASSERT( static_cast< AST::CSimpleExpression* >( syntaxTree[ 1 ] )->Expression()->Symbol() == Grammar::Symbol::S_CALL );
		UTEST_ASSERT( static_cast< AST::CSimpleExpression* >( syntaxTree[ 2 ] )->Expression()->Symbol() == Grammar::Symbol::S_SCOPE );

		AST::FreeTree( syntaxTree );
		UTEST_CASE_CLOSED();
	}( );

	UTEST_CASE( "Parser memory management" )
	{
		auto leakedNodes = AST::AllocatedExpressions();
		UTEST_ASSERT( leakedNodes.size() == 0 );
		UTEST_CASE_CLOSED();
	}( );

	AST::PopAllocTracking();

	UTEST_END();
}
