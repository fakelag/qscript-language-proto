#pragma once
#include <string>
#include <vector>

#include "Grammar.h"
#include "Lexer.h"
#include "AST.h"

namespace Parser
{
	std::vector< AST::IExpression* > Parse( const std::vector< Lexer::LexerSymbol_t >& symbols );
	std::string Stringify( const std::vector< AST::IExpression* >& expressions );
}
