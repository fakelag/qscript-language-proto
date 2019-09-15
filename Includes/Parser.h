#pragma once
#include <string>
#include <vector>

#include "Grammar.h"
#include "AST.h"

namespace Parser
{
	std::vector< Grammar::SymbolInfo_t > Parse( const std::string source );
	// std::string Stringify( const std::vector< Grammar::SymbolInfo_t >& symbols );
}
