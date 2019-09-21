#include <functional>
#include <algorithm>
#include "Parser.h"

namespace Parser
{
	struct ParserSymbol_t;

	using LeftBindFn = std::function<AST::IExpression*( const ParserSymbol_t& symbol, AST::IExpression* left )>;
	using RightBindFn = std::function<AST::IExpression*( const ParserSymbol_t& symbol )>;

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

		Grammar::Symbol				m_Symbol;
		Grammar::SymbolLoc_t		m_Locations;
		int							m_LBP;
		std::string					m_Token;
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
			return m_CurrentSymbol >= m_Symbols.size() - 1;
		}

		const ParserSymbol_t& CurrentSymbol()
		{
			if (IsFinished())
				throw "Reading tokens past end of stream";

			return m_Symbols[m_CurrentSymbol];
		}

		// Increments the current symbol index
		// & returns the previous symbol
		const ParserSymbol_t& NextSymbol()
		{
			if (IsFinished())
				throw "Reading tokens past end of stream";

			return m_Symbols[++m_CurrentSymbol - 1];
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

		auto nextExpression = [&parserState](int rbp = 0) -> AST::IExpression*
		{
			auto curSymbol = parserState.NextSymbol();
			auto leftSide = curSymbol.m_RightBind(curSymbol);

			if (rbp == -1)
				return leftSide;

			while (rbp < curSymbol.m_LexerSymbol.m_SymbolInfo.m_LBP)
			{
				auto nextSymbol = parserState.NextSymbol();
				leftSide = nextSymbol.m_RightBind(nextSymbol);
			}

			return leftSide;
		};

		std::vector< ParserSymbol_t > parserSymbols;
		std::transform(symbols.begin(), symbols.end(), parserSymbols.begin(),
			[&nextExpression]( const Lexer::LexerSymbol_t& lexerSymbol ) -> ParserSymbol_t {
				ParserSymbol_t symbol;
				symbol.m_LexerSymbol = lexerSymbol;
				symbol.m_LeftBind = NULL;
				symbol.m_RightBind = NULL;

				switch (lexerSymbol.m_Symbol)
				{
					case Grammar::Symbol::S_ADD:
					{
						symbol.m_LeftBind = [&nextExpression](const ParserSymbol_t& symbol, AST::IExpression* left) -> AST::IExpression*
						{
							auto right = nextExpression(symbol.m_LexerSymbol.m_SymbolInfo.m_LBP);
							auto assignExpr = new AST::CComplexExpression( left, right, symbol.m_LexerSymbol.m_Symbol, symbol.m_LexerSymbol.m_DebugInfo );
							return assignExpr;
						};
						break;
					}
					default:
						throw "Unhandled symbol";
				}

				return symbol;
			});

		while (!parserState.IsFinished())
		{
			auto currentSymbol = parserState.NextSymbol();
		}

		return parserState.Expressions();
	}

	std::string Stringify( const std::vector< AST::IExpression* >& expressions )
	{
		std::string output;

		for (auto expression : expressions)
			output += expression->ToString();

		return output;
	}
}
