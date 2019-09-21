#include <exception>
#include <algorithm>
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

		// Get the longest searchable token length
		int longestToken = commonSymbols.size() > 0 ? commonSymbols[ 0 ].m_Info.m_Token.length() : 0;

		auto pushSymbol = [ &currentLineNr, &currentColNr, &stringBuffer, &symbols ]() -> void {
			if ( stringBuffer.length() == 0 )
				return;

			Grammar::Symbol symbolType = Grammar::S_NAME;
			try
			{
				std::stoi( stringBuffer );
				symbolType = Grammar::S_INTCNST;
			}
			catch ( std::exception e )
			{
				// Conversion failed, it's probably a name
			}

			// Push the last accumulated string as SG_NAME and flush stringBuffer
			symbols.push_back( LexerSymbol_t {
				symbolType,
				Grammar::SymbolLoc_t { currentLineNr, currentColNr },
				Grammar::LBP_NAME,
				stringBuffer,
				} );

			currentColNr += stringBuffer.length();
			stringBuffer = "";
		};

		// Iterate through source and look for common symbols
		for ( auto srcIt = source.cbegin(); srcIt != source.cend(); ++srcIt )
		{
			auto findSymbol = [ &commonSymbols, &symbols, &currentLineNr, &currentColNr, &pushSymbol ]( std::string pattern ) -> int {
				// Iterate all the commons
				for ( auto symIt = commonSymbols.cbegin(); symIt != commonSymbols.cend(); ++symIt )
				{
					if ( symIt->m_Info.m_Token == pattern.substr( 0, symIt->m_Info.m_Token.length() ) )
					{
						pushSymbol();

						// Push the found common symbol
						symbols.push_back( LexerSymbol_t {
							symIt->m_Symbol,
							Grammar::SymbolLoc_t { currentLineNr, currentColNr },
							symIt->m_Info.m_LBP,
							symIt->m_Info.m_Token,
						} );

						currentColNr += symIt->m_Info.m_Token.length();

						return symIt->m_Info.m_Token.length();
					}
				}

				return 0;
			};

			int nCommonLength = findSymbol( source.substr( std::distance( source.cbegin(), srcIt ), longestToken ) );

			if ( nCommonLength > 0 )
			{
				// Increment the iterator by the amount of characters in the symbol
				srcIt += nCommonLength - 1;
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
			std::snprintf( szFormat, sizeof( szFormat ), "%s | lbp: %i\n", symbol.m_Token.c_str(), symbol.m_LBP );
			output += szFormat;
		}

		return output;
	}
}
