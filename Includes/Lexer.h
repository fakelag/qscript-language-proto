#pragma once
#include <string>
#include <vector>

#include "Grammar.h"

namespace Lexer
{
	struct LexerSymbol_t
	{
		Grammar::Symbol				m_Symbol;
		Grammar::SymbolLoc_t		m_Locations;
		int							m_LBP;
		std::string					m_Token;
	};

	std::vector< LexerSymbol_t > Parse( const std::string source );
	std::string Stringify( const std::vector< LexerSymbol_t >& symbols );
}
