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

		bool parsingString = false;
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

		auto pushSymbol = [ &currentLineNr, &currentColNr, &stringBuffer, &symbols, &isInteger, &isDecimal ]( bool isStrCnst = false ) -> void {
			if ( stringBuffer.length() == 0 )
				return;

			Grammar::Symbol symbolType = Grammar::Symbol::S_NAME;

			if ( isStrCnst )
			{
				// It is a string constant, strip the quotation marks
				stringBuffer = stringBuffer.substr( 1, stringBuffer.length() - 1 );
				symbolType = Grammar::Symbol::S_STRCNST;
			}
			else
			{
				if ( isDecimal( stringBuffer ) )
					symbolType = Grammar::Symbol::S_DBLCNST;
				else if ( isInteger( stringBuffer ) )
					symbolType = Grammar::Symbol::S_INTCNST;
			}

			// Push the last accumulated string as SG_NAME and flush stringBuffer
			symbols.push_back( LexerSymbol_t {
				symbolType,
				Grammar::SymbolLoc_t { currentLineNr, currentColNr, stringBuffer },
				Grammar::LeftBindingPower::LBP_NONE,
				stringBuffer,
				} );

			currentColNr += stringBuffer.length();
			stringBuffer = "";
		};

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
					symbols.push_back( LexerSymbol_t{
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

		// Iterate through source and look for common symbols
		for ( auto srcIt = source.cbegin(); srcIt != source.cend(); ++srcIt )
		{
			if ( *srcIt == '"' )
			{
				// Switch string parsing state
				parsingString = !parsingString;

				if ( !parsingString )
				{
					// Push the new string constant to the stack
					pushSymbol( true );
					continue;
				}
			}

			if ( !parsingString )
			{
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
			else
			{
				// Append the character to the string
				stringBuffer += *srcIt;
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
