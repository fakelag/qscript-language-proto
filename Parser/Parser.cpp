#include <functional>
#include <algorithm>
#include "Grammar.h"
#include "Parser.h"
#include "Exception.h"

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
			m_Location				= lexerSymbol.m_Location;
			m_LBP					= lexerSymbol.m_LBP;
			m_Symbol				= lexerSymbol.m_Symbol;
			m_Token					= lexerSymbol.m_Token;
			m_LeftBind				= NULL;
			m_RightBind				= NULL;
		}

		Grammar::Symbol				m_Symbol;		// Current symbol
		Grammar::SymbolLoc_t		m_Location;		// Debugging locations
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
			m_HasErrors = false;
		}

		void AddSymbols( const std::vector< ParserSymbol_t >& symbols )
		{
			m_Symbols = symbols;
		}

		bool IsFinished() const
		{
			return ( size_t ) m_CurrentSymbol >= m_Symbols.size();
		}

		const ParserSymbol_t& CurrentSymbol() const
		{
			if ( IsFinished() )
				throw Exception( "Reading tokens past end of stream" );

			return m_Symbols[ m_CurrentSymbol ];
		}

		const ParserSymbol_t* PreviousSymbol( int offset ) const
		{
			if ( m_CurrentSymbol - offset < 0 )
				return NULL;

			if ( m_CurrentSymbol - offset >= ( int ) m_Symbols.size() )
				throw Exception( "Reading tokens past end of stream" );

			return &m_Symbols[ m_CurrentSymbol - offset ];
		}

		// Increments the current symbol index & returns the previous symbol
		const ParserSymbol_t& NextSymbol()
		{
			return m_Symbols[ ++m_CurrentSymbol - 1 ];
		}

		void AddError( const Grammar::SymbolLoc_t& location, const std::string& exception )
		{
			m_HasErrors = true;
			m_Exception.AddError( location, exception );
		}

		void AddErrorAndResync( const Grammar::SymbolLoc_t& location, const std::string& exception )
		{
			m_HasErrors = true;
			m_Exception.AddError( location, exception );

			// Continue parsing from a stable spot.
			// This is so we can catch multiple mistakes
			// in the code at the same time
			while ( !IsFinished() )
			{
				switch ( m_Symbols[ m_CurrentSymbol ].m_Symbol )
				{
				case Grammar::Symbol::S_SEMICOLON:
				case Grammar::Symbol::S_BRACKET_CLOSE:
					++m_CurrentSymbol;
					break;
				default:
					++m_CurrentSymbol;
					continue;
				}

				break;
			}
		}

		bool HasErrors() const
		{
			return m_HasErrors;
		}

		void AddExpression( AST::IExpression* expression )
		{
			if ( expression )
				m_Expressions.push_back( expression );
		}

		const ParseException& GetException() const
		{
			return m_Exception;
		}

		const std::vector< AST::IExpression* >& GetExpressions() const
		{
			return m_Expressions;
		}

	private:
		std::vector< AST::IExpression* > 			m_Expressions;
		std::vector< ParserSymbol_t >				m_Symbols;
		int 										m_CurrentSymbol;

		bool										m_HasErrors;
		ParseException								m_Exception;
	};

	std::vector< AST::IExpression* > Parse( const std::vector< Lexer::LexerSymbol_t >& symbols, bool autoFreeMemory )
	{
		// Start tracking node allocations
		if ( autoFreeMemory )
			AST::PushTrackAlloc();

		// Create parser state object
		CParserState parserState;

		// Traverses the flat token array and gets called recursively by
		// m_LeftBind and m_RightBind to form a tree like structure
		auto nextExpression = [ &parserState ]( int rbp = 0 /* Right binding power */ ) -> AST::IExpression*
		{
			// Skip surplus semicolons
			if ( parserState.CurrentSymbol().m_Symbol == Grammar::Symbol::S_SEMICOLON )
				return NULL;

			// Get the current symbol (and advance to the next one)
			auto curSymbol = parserState.NextSymbol();

			if ( curSymbol.m_RightBind == NULL )
				throw ParseException( curSymbol.m_Location, "Expected an expression" );

			// Construct the left hand tree
			auto leftHand = curSymbol.m_RightBind( curSymbol );

			// Override -- if we only need the first right hand symbol, return it now
			if ( rbp == -1 )
				return leftHand;

			// Finished parsing while expecting more tokens,
			// this will happen with inputs like "var x = 4 +"
			// where the last operator is expecting a right hand argument
			if ( parserState.IsFinished() )
				throw ParseException( curSymbol.m_Location, "Expected an end of expression" );

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

				if ( curSymbol.m_LeftBind == NULL )
					throw ParseException( curSymbol.m_Location, "Expected an expression" );

				leftHand = curSymbol.m_LeftBind( curSymbol, leftHand );
			}

			return leftHand;
		};

		std::vector< ParserSymbol_t > parserSymbols;

		// Map lexer symbols to parser symbols
		std::transform( symbols.begin(), symbols.end(), std::back_inserter( parserSymbols ),
			[ &nextExpression, &parserState ]( const Lexer::LexerSymbol_t& lexerSymbol ) -> ParserSymbol_t
		{
			ParserSymbol_t symbol( lexerSymbol );

			try
			{
				switch ( lexerSymbol.m_Symbol )
				{
				case Grammar::Symbol::S_DBLCNST:
				{
					symbol.m_RightBind = [ &nextExpression ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
					{
						Value::CValue dblValue( std::stod( symbol.m_Token ) );
						return new AST::CValueExpression( dblValue, symbol.m_Symbol, symbol.m_Location );
					};
					break;
				}
				case Grammar::Symbol::S_INTCNST:
				{
					symbol.m_RightBind = [ &nextExpression ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
					{
						Value::CValue intValue( std::stoi( symbol.m_Token ) );
						return new AST::CValueExpression( intValue, symbol.m_Symbol, symbol.m_Location );
					};
					break;
				}
				case Grammar::Symbol::S_STRCNST:
				case Grammar::Symbol::S_NAME:
				{
					symbol.m_RightBind = [ &nextExpression ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
					{
						Value::CValue strValue( symbol.m_Token );
						return new AST::CValueExpression( strValue, symbol.m_Symbol, symbol.m_Location );
					};
					break;
				}
				case Grammar::Symbol::S_LOGIC_NOT:
				{
					symbol.m_RightBind = [ &nextExpression ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
					{
						auto right = nextExpression( symbol.m_LBP );
						return new AST::CSimpleExpression( right, symbol.m_Symbol, symbol.m_Location );
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
						return new AST::CComplexExpression( left, right, symbol.m_Symbol, symbol.m_Location );
					};

					// Right binding function only parses ahead of the current token. Usually to embed the next expression to
					// it's tree
					symbol.m_RightBind = [ &nextExpression ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
					{
						auto right = nextExpression( -1 );
						return new AST::CSimpleExpression( right, symbol.m_Symbol, symbol.m_Location );
					};
					break;
				}
				case Grammar::Symbol::S_INCREMENT:
				case Grammar::Symbol::S_DECREMENT:
				{
					symbol.m_LeftBind = [ &nextExpression ]( const ParserSymbol_t& symbol, AST::IExpression* left ) -> AST::IExpression*
					{
						if ( !AST::IsVariable( left ) )
							throw ParseException( left->Location(), "Expected a variable, got: \"" + left->Location().m_SrcToken + "\"" );

						return new AST::CComplexExpression( left, NULL, symbol.m_Symbol, symbol.m_Location );
					};

					symbol.m_RightBind = [ &nextExpression ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
					{
						auto right = nextExpression( symbol.m_LBP );

						if ( !AST::IsVariable( right ) )
							throw ParseException( right->Location(), "Expected a variable, got: \"" + right->Location().m_SrcToken + "\"" );

						return new AST::CComplexExpression( NULL, right, symbol.m_Symbol, symbol.m_Location );
					};
					break;
				}
				case Grammar::Symbol::S_ASSIGN:
				case Grammar::Symbol::S_ASSIGN_ADD:
				case Grammar::Symbol::S_ASSIGN_DIV:
				case Grammar::Symbol::S_ASSIGN_SUB:
				case Grammar::Symbol::S_ASSIGN_MUL:
				case Grammar::Symbol::S_ASSIGN_MOD:
				{
					symbol.m_LeftBind = [ &nextExpression ]( const ParserSymbol_t& symbol, AST::IExpression* left ) -> AST::IExpression*
					{
						auto right = nextExpression( symbol.m_LBP );

						if ( symbol.m_Symbol == Grammar::Symbol::S_ASSIGN )
						{
							if ( !AST::IsVariable( left ) && left->Symbol() != Grammar::Symbol::S_VAR )
								throw ParseException( left->Location(), "Expected a variable, got: \"" + left->Location().m_SrcToken + "\"" );
						}
						else
						{
							if ( !AST::IsVariable( left ) )
								throw ParseException( left->Location(), "Expected a variable, got: \"" + left->Location().m_SrcToken + "\"" );
						}

						return new AST::CComplexExpression( left, right, symbol.m_Symbol, symbol.m_Location );
					};
					break;
				}
				case Grammar::Symbol::S_MUL:
				case Grammar::Symbol::S_DIV:
				case Grammar::Symbol::S_POW:
				case Grammar::Symbol::S_MOD:
				case Grammar::Symbol::S_LOGIC_AND:
				case Grammar::Symbol::S_LOGIC_OR:
				case Grammar::Symbol::S_EQUALS:
				case Grammar::Symbol::S_EQUALS_NOT:
				case Grammar::Symbol::S_MORE_OR_EQUALS:
				case Grammar::Symbol::S_LESS_OR_EQUALS:
				case Grammar::Symbol::S_LESSTHAN:
				case Grammar::Symbol::S_MORETHAN:
				{
					symbol.m_LeftBind = [ &nextExpression ]( const ParserSymbol_t& symbol, AST::IExpression* left ) -> AST::IExpression*
					{
						auto right = nextExpression( symbol.m_LBP );
						return new AST::CComplexExpression( left, right, symbol.m_Symbol, symbol.m_Location );
					};
					break;
				}
				case Grammar::Symbol::S_DOT:
				{
					symbol.m_LeftBind = [ &nextExpression ]( const ParserSymbol_t& symbol, AST::IExpression* left ) -> AST::IExpression*
					{
						auto right = nextExpression( symbol.m_LBP );
						return new AST::CComplexExpression( left, right, symbol.m_Symbol, symbol.m_Location );
					};
					break;
				}
				case Grammar::Symbol::S_RETURN:
				case Grammar::Symbol::S_BREAK:
				{
					symbol.m_RightBind = [ &nextExpression ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
					{
						return new AST::CSimpleExpression( nextExpression(), symbol.m_Symbol, symbol.m_Location );
					};
					break;
				}
				case Grammar::Symbol::S_TRUE:
				case Grammar::Symbol::S_FALSE:
				{
					symbol.m_RightBind = [ &nextExpression ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
					{
						Value::CValue boolValue( symbol.m_Symbol == Grammar::Symbol::S_TRUE ? true : false );
						return new AST::CValueExpression( boolValue, symbol.m_Symbol, symbol.m_Location );
					};
					break;
				}
				case Grammar::Symbol::S_DEFER:
				{
					symbol.m_RightBind = [ &nextExpression ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
					{
						auto right = nextExpression();
						return new AST::CSimpleExpression( right, symbol.m_Symbol, symbol.m_Location );
					};
					break;
				}
				case Grammar::Symbol::S_WHILE:
				case Grammar::Symbol::S_IF:
				{
					symbol.m_RightBind = [ &nextExpression ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
					{
						auto condition = nextExpression();
						auto body = nextExpression();

						// Append a scope for single line bodies
						if ( body->Symbol() != Grammar::Symbol::S_SCOPE )
							body = new AST::CListExpression( { body }, Grammar::Symbol::S_SCOPE, symbol.m_Location );

						return new AST::CComplexExpression( condition, body, symbol.m_Symbol, symbol.m_Location );
					};
					break;
				}
				case Grammar::Symbol::S_FOR:
				{
					symbol.m_RightBind = [ &nextExpression, &parserState ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
					{
						auto head = nextExpression();

						if ( head->Type() != AST::ExpressionType::ET_LIST )
							throw ParseException( head->Location(), "Expected a for loop declaration, got: \"" + head->Location().m_SrcToken + "\"" );

						auto body = nextExpression();

						// Append a scope for single line bodies
						if ( body->Symbol() != Grammar::Symbol::S_SCOPE )
							body = new AST::CListExpression( { body }, Grammar::Symbol::S_SCOPE, symbol.m_Location );

						std::vector< AST::IExpression* > forLoop = static_cast< AST::CListExpression* >( head )->List();

						forLoop.push_back( body );

						if ( forLoop.size() != 4 )
							throw ParseException( head->Location(), "Invalid for loop declaration" );

						// remove the head node, and append it's content to the for node
						delete head;

						return new AST::CListExpression( forLoop, Grammar::Symbol::S_FOR, symbol.m_Location );
					};
					break;
				}
				case Grammar::Symbol::S_FUNC:
				{
					symbol.m_RightBind = [ &nextExpression, &parserState ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
					{
						auto name = nextExpression( -1 );

						if ( !AST::IsNameConstant( name ) )
							throw ParseException( name->Location(), "Expected a function name, got: \"" + name->Location().m_SrcToken + "\"" );

						auto args = nextExpression();

						// Function takes no arguments (empty parenthesis)
						if ( args == NULL )
							args = new AST::CListExpression( {}, Grammar::Symbol::S_LIST, symbol.m_Location );

						// Force args into a list, if it isn't already
						if ( args->Type() != AST::ExpressionType::ET_LIST )
							args = new AST::CListExpression( { args }, Grammar::Symbol::S_LIST, symbol.m_Location );

						// The body of a function may also be NULL
						auto body = nextExpression();

						// Append a scope for single line bodies
						if ( body && body->Symbol() != Grammar::Symbol::S_SCOPE )
							body = new AST::CListExpression( { body }, Grammar::Symbol::S_SCOPE, symbol.m_Location );

						auto funcExpr = new AST::CComplexExpression( name, args, Grammar::Symbol::S_FUNCDEF, name->Location() );
						auto bodyExpr = new AST::CSimpleExpression( body, Grammar::Symbol::S_FUNCBODY, body ? body->Location() : name->Location() );
						return new AST::CComplexExpression( funcExpr, bodyExpr, Grammar::Symbol::S_FUNC, symbol.m_Location );
					};
					break;
				}
				case Grammar::Symbol::S_CLASS:
				{
					symbol.m_RightBind = [ &nextExpression ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
					{
						AST::IExpression* name = nextExpression();
						AST::IExpression* parent = NULL;
						AST::IExpression* body = NULL;

						if ( !AST::IsNameConstant( name ) )
							throw ParseException( name->Location(), "Expected a class name, got: \"" + name->Location().m_SrcToken + "\"" );

						auto next = nextExpression();
						switch ( next->Symbol() )
						{
						case Grammar::Symbol::S_NAME:
						{
							// Convert the single name into a list, and then handle it as a list of names
							next = new AST::CListExpression({ next }, Grammar::Symbol::S_LIST, next->Location() );

							/* Fall through! Treat the expression as a list */
						}
						case Grammar::Symbol::S_LIST:
						{
							// There is an inheritance chain, add it
							parent = next;

							// Parse the class body
							auto bodyExpr = nextExpression();

							if ( bodyExpr->Symbol() != Grammar::Symbol::S_SCOPE )
								throw ParseException( bodyExpr->Location(), "Expected a class definition, got: \"" + bodyExpr->Location().m_SrcToken + "\"" );

							// Replace S_SCOPE with S_CLASSBODY
							auto exprList = static_cast< AST::CListExpression* >( bodyExpr )->List();

							// Create body expression
							body = new AST::CListExpression( exprList, Grammar::Symbol::S_CLASSBODY, bodyExpr->Location() );

							// Remove scope expression
							delete bodyExpr;
							break;
						}
						case Grammar::Symbol::S_SCOPE:
						{
							// Replace S_SCOPE with S_CLASSBODY
							auto exprList = static_cast< AST::CListExpression* >( next )->List();

							// Create body expression
							body = new AST::CListExpression( exprList, Grammar::Symbol::S_CLASSBODY, next->Location() );

							// Create an empty inheritence chain
							parent = new AST::CListExpression({}, Grammar::Symbol::S_LIST, symbol.m_Location );

							// Remove scope expression
							delete next;
							break;
						}
						default:
							throw ParseException( next->Location(), "Expected a class definition, got: \"" + next->Location().m_SrcToken + "\"" );
						}

						return new AST::CListExpression( { name, parent, body }, symbol.m_Symbol, symbol.m_Location );
					};
					break;
				}
				case Grammar::Symbol::S_VAR:
				{
					symbol.m_RightBind = [ &nextExpression ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
					{
						auto right = nextExpression( symbol.m_LBP );

						if ( !AST::IsNameConstant( right ) )
							throw ParseException( right->Location(), "Expected a variable name, got: \"" + right->Location().m_SrcToken + "\"" );

						return new AST::CSimpleExpression( right, symbol.m_Symbol, symbol.m_Location );
					};
					break;
				}
				case Grammar::Symbol::S_COLON:
				{
					symbol.m_RightBind = [ &nextExpression ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
					{
						return nextExpression();
					};
					break;
				}
				case Grammar::Symbol::S_COMMA:
				{
					symbol.m_LeftBind = [ &nextExpression, &parserState ]( const ParserSymbol_t& symbol, AST::IExpression* left ) -> AST::IExpression*
					{
						std::vector< AST::IExpression* > expressionList ={ left };

						while ( true )
						{
							expressionList.push_back( nextExpression( symbol.m_LBP ) );

							if ( parserState.CurrentSymbol().m_Symbol != Grammar::Symbol::S_COMMA )
								break;

							parserState.NextSymbol();
						}

						return new AST::CListExpression( expressionList, Grammar::Symbol::S_LIST, symbol.m_Location );
					};
					break;
				}
				case Grammar::Symbol::S_BRACKET_OPEN:
				{
					symbol.m_RightBind = [ &nextExpression, &parserState ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
					{
						std::vector< AST::IExpression* > expressionList;

						while ( parserState.CurrentSymbol().m_Symbol != Grammar::Symbol::S_BRACKET_CLOSE )
						{
							auto expression = nextExpression();

							// Only add executable expressions to scopes
							if ( expression )
								expressionList.push_back( expression );

							parserState.NextSymbol();
						}

						return new AST::CListExpression( expressionList, Grammar::Symbol::S_SCOPE, symbol.m_Location );
					};
					break;
				}
				case Grammar::Symbol::S_SBRACKET_OPEN:
				{
					symbol.m_LeftBind = [ &nextExpression, &parserState ]( const ParserSymbol_t& symbol, AST::IExpression* left ) -> AST::IExpression*
					{
						auto right = nextExpression();

						if ( parserState.NextSymbol().m_Symbol != Grammar::Symbol::S_SBRACKET_CLOSE )
						{
							auto curSymbol = parserState.CurrentSymbol();
							throw ParseException( curSymbol.m_Location, "Expected end of an access expression, got: \"" + curSymbol.m_Token + "\"" );
						}

						return new AST::CComplexExpression( left, right, Grammar::Symbol::S_ACCESS, symbol.m_Location );
					};

					symbol.m_RightBind = [ &nextExpression, &parserState ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
					{
						if ( parserState.CurrentSymbol().m_Symbol == Grammar::Symbol::S_SBRACKET_CLOSE )
						{
							// Empty list
							parserState.NextSymbol();
							return new AST::CListExpression( {}, Grammar::Symbol::S_ARRAY, symbol.m_Location );
						}
						else
						{
							auto right = nextExpression();

							if ( parserState.NextSymbol().m_Symbol != Grammar::Symbol::S_SBRACKET_CLOSE )
							{
								auto curSymbol = parserState.CurrentSymbol();
								throw ParseException( curSymbol.m_Location, "Expected end of an array expression, got: \"" + curSymbol.m_Token + "\"" );
							}

							if ( right->Type() == AST::ExpressionType::ET_LIST )
							{
								// Convert the list into an array
								std::vector< AST::IExpression* > list = static_cast< AST::CListExpression* >( right )->List();

								// remove only the list node (while keeping it's children intact)
								delete right;

								return new AST::CListExpression( list, Grammar::Symbol::S_ARRAY, symbol.m_Location );
							}
							else
							{
								return new AST::CListExpression( { right }, Grammar::Symbol::S_ARRAY, symbol.m_Location );
							}
						}
					};
					break;
				}
				case Grammar::Symbol::S_PARENT_OPEN:
				{
					symbol.m_LeftBind = [ &nextExpression, &parserState ]( const ParserSymbol_t& symbol, AST::IExpression* left ) -> AST::IExpression*
					{
						if ( parserState.CurrentSymbol().m_Symbol == Grammar::Symbol::S_PARENT_CLOSE )
						{
							parserState.NextSymbol(); // skip ')'
							auto emptyArgs = new AST::CListExpression( {}, Grammar::Symbol::S_LIST, symbol.m_Location );
							return new AST::CComplexExpression( left, emptyArgs, Grammar::Symbol::S_CALL, symbol.m_Location );
						}
						else
						{
							auto right = nextExpression();

							if ( parserState.CurrentSymbol().m_Symbol != Grammar::Symbol::S_PARENT_CLOSE )
								throw ParseException( parserState.CurrentSymbol().m_Location, "Expected a closing parenthesis, got: \"" + parserState.CurrentSymbol().m_Token + "\"" );

							parserState.NextSymbol(); // skip ')'

							if ( right->Symbol() == Grammar::Symbol::S_LIST )
							{
								return new AST::CComplexExpression( left, right, Grammar::Symbol::S_CALL, symbol.m_Location );
							}
							else
							{
								auto args = new AST::CListExpression( { right }, Grammar::Symbol::S_LIST, symbol.m_Location );
								return new AST::CComplexExpression( left, args, Grammar::Symbol::S_CALL, symbol.m_Location );
							}
						}
					};

					symbol.m_RightBind = [ &nextExpression, &parserState ]( const ParserSymbol_t& symbol ) -> AST::IExpression*
					{
						auto previousSymbol = parserState.PreviousSymbol( 2 );

						if ( previousSymbol && previousSymbol->m_Symbol == Grammar::Symbol::S_FOR )
						{
							if ( parserState.CurrentSymbol().m_Symbol == Grammar::Symbol::S_PARENT_CLOSE )
								throw ParseException( parserState.CurrentSymbol().m_Location, "Expected a for loop, got: \"" + parserState.CurrentSymbol().m_Token + "\"" );

							std::vector< AST::IExpression* > expressionList ={};
							while ( true )
							{
								if ( parserState.CurrentSymbol().m_Symbol == Grammar::Symbol::S_SEMICOLON )
								{
									expressionList.push_back( NULL );

									parserState.NextSymbol();
									if ( parserState.CurrentSymbol().m_Symbol == Grammar::Symbol::S_PARENT_CLOSE )
									{
										expressionList.push_back( NULL );
										parserState.NextSymbol();
										break;
									}

									continue;
								}

								expressionList.push_back( nextExpression() );

								if ( parserState.NextSymbol().m_Symbol == Grammar::Symbol::S_PARENT_CLOSE )
									break;

								if ( parserState.CurrentSymbol().m_Symbol == Grammar::Symbol::S_PARENT_CLOSE )
								{
									expressionList.push_back( NULL );
									parserState.NextSymbol();
									break;
								}
							}

							return new AST::CListExpression( expressionList, Grammar::Symbol::S_LIST, symbol.m_Location );
						}
						else
						{
							if ( parserState.CurrentSymbol().m_Symbol == Grammar::Symbol::S_PARENT_CLOSE )
							{
								parserState.NextSymbol();
								return NULL;
							}

							auto right = nextExpression();

							if ( parserState.NextSymbol().m_Symbol != Grammar::Symbol::S_PARENT_CLOSE )
							{
								auto curSymbol = parserState.CurrentSymbol();
								throw ParseException( right->Location(), "Expected an end of expression, got: \"" + curSymbol.m_Token + "\"" );
							}

							return right;
						}
					};
					break;
				}
				case Grammar::Symbol::S_SEMICOLON:
				case Grammar::Symbol::S_PARENT_CLOSE:
				case Grammar::Symbol::S_SBRACKET_CLOSE:
				case Grammar::Symbol::S_BRACKET_CLOSE:
					break;
				default:
					throw ParseException( lexerSymbol.m_Location, "Unknown symbol: " + std::to_string( lexerSymbol.m_Symbol ) );
				}
			}
			catch ( const ParseException& exception )
			{
				// Add to the error list
				parserState.AddError( exception.locations()[ 0 ], exception.errors()[ 0 ] );
			}

			return symbol;
		} );

		// Load the symbols
		parserState.AddSymbols( parserSymbols );

		while ( !parserState.IsFinished() )
		{
			try
			{
				// Add all top level expressions to the Expressions list
				parserState.AddExpression( nextExpression() );
				parserState.NextSymbol();
			}
			catch ( const ParseException& exception )
			{
				// Propagate the error and relocate
				parserState.AddErrorAndResync( exception.locations()[ 0 ], exception.errors()[ 0 ] );
			}
		}

		std::vector< AST::IExpression* > allocatedExpressions;

		if ( autoFreeMemory )
		{
			// Fetch everything allocated
			allocatedExpressions = AST::AllocatedExpressions();

			// Stop tracking
			AST::PopAllocTracking();
		}

		// Propagate all the accumulated errors to the caller, if any
		if ( parserState.HasErrors() )
		{
			if ( autoFreeMemory )
			{
				// Free all allocated expressions
				for ( auto expr : allocatedExpressions )
					delete expr;
			}

			ParseException exception = parserState.GetException();
			throw exception;
		}

		return parserState.GetExpressions();
	}

	std::string Stringify( const std::vector< AST::IExpression* >& expressions )
	{
		std::string output;

		for ( auto expression : expressions )
			output += expression->ToString();

		return output;
	}
}
