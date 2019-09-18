#include "Parser.h"

namespace Parser
{
	std::vector< AST::IExpression* > Parse( const std::vector< Lexer::LexerSymbol_t >& symbols )
	{
		std::vector< AST::IExpression* > expressions;

		return expressions;
	}

	std::string Stringify( const std::vector< AST::IExpression* >& expressions )
	{
		std::string output;

		for (auto expression : expressions)
			output += expression->ToString();

		return output;
	}
}
