#include <exception>
#include <algorithm>
#include <regex>
#include "Lexer.h"

namespace Lexer
{
	struct CommonSymbol_t
	{
		Grammar::SymbolInfo_t 	m_Info;
		Grammar::Symbol 		m_Symbol;
	};

	std::vector< LexerSymbol_t > Parse( const std::string source )
	{
		std::vector< LexerSymbol_t > symbols;
		std::vector< CommonSymbol_t > commonSymbols;
		std::string stringBuffer = "";
		int currentLineNr = 0;
		int currentColNr = 0;

		// Map common symbols to a vector
		for ( auto it = Grammar::LanguageSymbols.cbegin(); it != Grammar::LanguageSymbols.cend(); ++it )
			commonSymbols.push_back( { it->second, it->first } );

		// Sort the common symbols to have longest character words at the top
		std::sort( commonSymbols.begin(), commonSymbols.end(), []( CommonSymbol_t a, CommonSymbol_t b ) -> bool
		{
			return a.m_Info.m_Token.length() > b.m_Info.m_Token.length();
		} );

		// Is this string an integer ?
		auto isInteger = []( const std::string token ) -> bool {
			std::regex re("[-+]?([0-9]*[0-9]+|[0-9]+)");
			return std::regex_match( token, re );
		};

		// Is it a decimal ?
		auto isDecimal = []( const std::string token ) -> bool {
			std::regex re("[-+]?\\d+\\.\\d*");
			return std::regex_match( token, re );
		};

		// Get the longest searchable token length
		int longestToken = commonSymbols.size() > 0 ? commonSymbols[ 0 ].m_Info.m_Token.length() : 0;

		auto pushSymbol = [ &currentLineNr, &currentColNr, &stringBuffer, &symbols, &isInteger, &isDecimal ]() -> void {
			if ( stringBuffer.length() == 0 )
				return;

			Grammar::Symbol symbolType = Grammar::S_NAME;

			if ( isDecimal( stringBuffer ) )
				symbolType = Grammar::Symbol::S_DBLCNST;
			else if ( isInteger( stringBuffer ) )
				symbolType = Grammar::Symbol::S_INTCNST;

			// Push the last accumulated string as SG_NAME and flush stringBuffer
			symbols.push_back( LexerSymbol_t {
				symbolType,
				Grammar::SymbolLoc_t { currentLineNr, currentColNr, stringBuffer },
				Grammar::LBP_NONE,
				stringBuffer,
				} );

			currentColNr += stringBuffer.length();
			stringBuffer = "";
		};

		// Iterate through source and look for common symbols
		for ( auto srcIt = source.cbegin(); srcIt != source.cend(); ++srcIt )
		{
			auto findSymbol = [ &commonSymbols, &symbols, &currentLineNr,
				&currentColNr, &pushSymbol, &stringBuffer, &isInteger ]( std::string pattern ) -> int {
				// Iterate all the commons
				for ( auto symIt = commonSymbols.cbegin(); symIt != commonSymbols.cend(); ++symIt )
				{
					if ( symIt->m_Info.m_Token == pattern.substr( 0, symIt->m_Info.m_Token.length() ) )
					{
						if ( symIt->m_Info.m_Token == "." && isInteger( stringBuffer ) )
							return 0;

						pushSymbol();

						// Push the found common symbol
						symbols.push_back( LexerSymbol_t {
							symIt->m_Symbol,
							Grammar::SymbolLoc_t { currentLineNr, currentColNr, symIt->m_Info.m_Token },
							symIt->m_Info.m_LBP,
							symIt->m_Info.m_Token,
						} );

						currentColNr += symIt->m_Info.m_Token.length();

						return symIt->m_Info.m_Token.length();
					}
				}

				return 0;
			};

			int commonLength = findSymbol( source.substr( std::distance( source.cbegin(), srcIt ), longestToken ) );

			if ( commonLength > 0 )
			{
				// Increment the iterator by the amount of characters in the symbol
				srcIt += commonLength - 1;
				continue;
			}

			switch ( *srcIt )
			{
			case '\n':
			case '\t':
			case ' ':
			{
				pushSymbol();
				if ( *srcIt != '\n' )
				{
					++currentColNr;
				}
				else
				{
					currentColNr = 0;
					++currentLineNr;
				}

				break;
			}
			default:
			{
				// Add the character to the symbol buffer
				stringBuffer += *srcIt;
				break;
			}
			}
		}

		pushSymbol();

		return symbols;
	}

	std::string Stringify( const std::vector< LexerSymbol_t >& symbols )
	{
		std::string output;

		char szFormat[ 256 ];
		for ( auto symbol : symbols )
		{
			std::snprintf( szFormat, sizeof( szFormat ), "%s | sym: %i\n", symbol.m_Token.c_str(), symbol.m_Symbol );
			output += szFormat;
		}

		return output;
	}
}
