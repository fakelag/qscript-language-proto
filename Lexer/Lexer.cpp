#pragma once
#include <algorithm>
#include "Lexer.h"

namespace Lexer
{
	std::vector< Grammar::SymbolInfo_t > Parse( const std::string source )
	{
		std::vector< Grammar::SymbolInfo_t > symbols;
		std::vector< Grammar::SymbolInfo_t > commonSymbols;
		std::string stringBuffer = "";

		// Map common symbols to a vector
		for ( auto it = Grammar::LanguageSymbols.cbegin(); it != Grammar::LanguageSymbols.cend(); ++it )
			commonSymbols.push_back( it->second );

		// Sort the common symbols to have longest character words at the top
		std::sort( commonSymbols.begin(), commonSymbols.end(), []( Grammar::SymbolInfo_t a, Grammar::SymbolInfo_t b )
		{
			return a.m_Token.length() > b.m_Token.length();
		} );

		// Get the longest searchable token length
		int longestToken = commonSymbols.size() > 0 ? commonSymbols[ 0 ].m_Token.length() : 0;

		// Iterate through source and look for common symbols
		for ( auto srcIt = source.cbegin(); srcIt != source.cend(); ++srcIt )
		{
			auto findSymbol = [ &commonSymbols, &symbols, &stringBuffer ]( std::string pattern ) -> bool {
				// Iterate all the commons
				for ( auto symIt = commonSymbols.cbegin(); symIt != commonSymbols.cend(); ++symIt )
				{
					if ( symIt->m_Token == pattern.substr( 0, symIt->m_Token.length() ) )
					{
						if ( stringBuffer.length() > 0 )
						{
							// Push the last accumulated string as SG_NAME and flush stringBuffer
							symbols.push_back( { stringBuffer, Grammar::LBP_NAME, Grammar::SG_NAME } );
							stringBuffer = "";
						}

						// Push the found common symbol
						symbols.push_back( *symIt );
						return true;
					}
				}

				return false;
			};

			if ( findSymbol( source.substr( std::distance( source.cbegin(), srcIt ), longestToken ) ) )
				continue;

			switch ( *srcIt )
			{
			case ' ':
				if ( stringBuffer.length() > 0 )
				{
					// Push the last accumulated string as SG_NAME and flush stringBuffer
					symbols.push_back( { stringBuffer, Grammar::LBP_NAME, Grammar::SG_NAME } );
					stringBuffer = "";
				}

				break;
			default:
				// Add the character to the symbol buffer
				stringBuffer += *srcIt;
				break;
			}
		}

		return symbols;
	}
}
