#include <functional>
#include <algorithm>
#include "Parser.h"

namespace Parser
{
	struct ParserSymbol_t;

	// Functions used for binding expressions together in a tree like structure
	using LeftBindFn = std::function<AST::IExpression*( const ParserSymbol_t& symbol, AST::IExpression* left )>;
	using RightBindFn = std::function<AST::IExpression*( const ParserSymbol_t& symbol )>;

	/*
		A flat structure used to generate the AST via m_LeftBind and m_RightBind depending on the symbol and
		the state of the parser.
	*/
	struct ParserSymbol_t
	{
		ParserSymbol_t( Lexer::LexerSymbol_t lexerSymbol )
		{
			m_Locations				= lexerSymbol.m_Locations;
			m_LBP					= lexerSymbol.m_LBP;
			m_Symbol				= lexerSymbol.m_Symbol;
			m_Token					= lexerSymbol.m_Token;
			m_LeftBind				= NULL;
			m_RightBind				= NULL;
		}

		Grammar::Symbol				m_Symbol;		// Current symbol
		Grammar::SymbolLoc_t		m_Locations;	// Debugging locations
		int							m_LBP;			// Left binding power
		std::string					m_Token;		// Token (Only used to propagate to CValueExpression for names and constants)

		// Different symbols override their binding behavior with these binding lambdas
		// Left binding is used by infix/postfix operators
		// Right binding is used by prefix operators
		LeftBindFn					m_LeftBind;
		RightBindFn					m_RightBind;
	};

	class CParserState
	{
	public:
		CParserState()
		{
			m_CurrentSymbol = 0;
		}

		void AddSymbols( const std::vector< ParserSymbol_t >& symbols )
		{
			m_Symbols = symbols;
		}

		bool IsFinished()
		{
			return ( size_t ) m_CurrentSymbol >= m_Symbols.size();
		}

		const ParserSymbol_t& CurrentSymbol()
		{
			if ( IsFinished() )
				throw "Reading tokens past end of stream";

			return m_Symbols[ m_CurrentSymbol ];
		}

		// Increments the current symbol index & returns the previous symbol
		const ParserSymbol_t& NextSymbol()
		{
			return m_Symbols[ ++m_CurrentSymbol - 1];
		}

		void AddExpression( AST::IExpression* expression )
		{
			m_Expressions.push_back( expression );
		}

		const std::vector< AST::IExpression* >& Expressions()
		{
			return m_Expressions;
		}

	private:
		std::vector< AST::IExpression* > 			m_Expressions;
		std::vector< ParserSymbol_t >				m_Symbols;
		int 										m_CurrentSymbol;
	};

	std::vector< AST::IExpression* > Parse( const std::vector< Lexer::LexerSymbol_t >& symbols )
	{
		CParserState parserState;

		// Traverses the flat token array and gets called recursively by
		// m_LeftBind and m_RightBind to form a tree like structure
		auto nextExpression = [ &parserState ]( int rbp = 0 /* Right binding power */ ) -> AST::IExpression*
		{
			// Get the current symbol (and advance to the next one)
			auto curSymbol = parserState.NextSymbol();

			// Construct the left hand tree
			auto leftSide = curSymbol.m_RightBind( curSymbol );

			// Override -- if we only need the first right hand symbol, return it now
			if ( rbp == -1 )
				return leftSide;

			// Finished parsing while expecting more tokens,
			// this will happen with inputs like "var x = 4 +"
			// where the last operator is expecting a right hand argument
			if ( parserState.IsFinished() )
				throw "Expected end of expression";

			// Call left bind while the current right binding power is less than the
			// next symbols left binding power. E.g:
			// 4	+	2	*	2	+	1;
			//		│		│		└ binding power is 10
			//		│		│ 
			//		│		└─ binding power is 20
			//		└─ binding power is 10
			//
			// this will group (2*2) at the bottom of the tree
			// while (4+(2*2)) is the second outer node,
			// and (4+(2*2))+1) the most outer node
			while ( rbp < parserState.CurrentSymbol().m_LBP )
			{
				curSymbol = parserState.NextSymbol();
				leftSide = curSymbol.m_LeftBind( curSymbol, leftSide );
			}

			return leftSide;
		};

		std::vector< ParserSymbol_t > parserSymbols;

		// Map lexer symbols to parser symbols
		std::transform( symbols.begin(), symbols.end(), std::back_inserter( parserSymbols ),
			[ &nextExpression ]( const Lexer::LexerSymbol_t& lexerSymbol ) -> ParserSymbol_t {
			ParserSymbol_t symbol( lexerSymbol );

			switch ( lexerSymbol.m_Symbol )
			{
			case Grammar::Symbol::S_INTCNST:
			{
				symbol.m_RightBind = [ &nextExpression ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
				{
					Value::CValue intValue( std::stoi( symbol.m_Token ) );
					return new AST::CValueExpression( intValue, symbol.m_Symbol, symbol.m_Locations );
				};
				break;
			}
			case Grammar::Symbol::S_NAME:
			{
				symbol.m_RightBind = [ &nextExpression ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
				{
					Value::CValue intValue( symbol.m_Token );
					return new AST::CValueExpression( intValue, symbol.m_Symbol, symbol.m_Locations );
				};
				break;
			}
			case Grammar::Symbol::S_ADD:
			case Grammar::Symbol::S_SUB:
			{
				// Left binding function receives the left hand side expression already parsed for it
				// and binds it to the tree
				symbol.m_LeftBind = [ &nextExpression ]( const ParserSymbol_t& symbol, AST::IExpression* left ) -> AST::IExpression*
				{
					auto right = nextExpression( symbol.m_LBP );
					return new AST::CComplexExpression( left, right, symbol.m_Symbol, symbol.m_Locations );
				};

				// Right binding function only parses ahead of the current token. Usually to embed the next expression to
				// it's tree
				symbol.m_RightBind = [ &nextExpression ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
				{
					auto right = nextExpression( -1 );
					return new AST::CSimpleExpression( right, symbol.m_Symbol, symbol.m_Locations );
				};
				break;
			}
			case Grammar::Symbol::S_MUL:
			case Grammar::Symbol::S_DIV:
			case Grammar::Symbol::S_POW:
			case Grammar::Symbol::S_MOD:
			case Grammar::Symbol::S_ASSIGN:
			{
				symbol.m_LeftBind = [ &nextExpression ]( const ParserSymbol_t& symbol, AST::IExpression* left ) -> AST::IExpression*
				{
					auto right = nextExpression( symbol.m_LBP );
					return new AST::CComplexExpression( left, right, symbol.m_Symbol, symbol.m_Locations );
				};
				break;
			}
			case Grammar::Symbol::S_VAR:
			{
				symbol.m_RightBind = [ &nextExpression ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
				{
					auto nameExpr = nextExpression( symbol.m_LBP );
					return new AST::CSimpleExpression( nameExpr, symbol.m_Symbol, symbol.m_Locations );
				};
				break;
			}
			case Grammar::Symbol::S_SEMICOLON:
				break;
			default:
				throw "Unhandled symbol";
			}

			return symbol;
		} );

		// Load the symbols
		parserState.AddSymbols( parserSymbols );

		while ( !parserState.IsFinished() )
		{
			// Add all top level expressions to the Expressions list
			parserState.AddExpression( nextExpression() );
			parserState.NextSymbol();
		}

		return parserState.Expressions();
	}

	std::string Stringify( const std::vector< AST::IExpression* >& expressions )
	{
		std::string output;

		for ( auto expression : expressions )
			output += expression->ToString();

		return output;
	}
}
