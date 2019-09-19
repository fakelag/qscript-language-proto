#pragma once
#include <string>
#include <vector>

#include "Grammar.h"

namespace Lexer
{
	struct DebugInfo_t
	{
		int						m_LineNr;
		int						m_ColNr;
	};

	struct LexerSymbol_t
	{
		Grammar::Symbol			m_Symbol;
		Grammar::SymbolInfo_t	m_SymbolInfo;
		DebugInfo_t				m_DebugInfo;
	};

	std::vector< LexerSymbol_t > Parse( const std::string source );
	std::string Stringify( const std::vector< LexerSymbol_t >& symbols );
}
