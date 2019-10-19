#include "Runtime.h"
#include "Grammar.h"
#include <functional>
#include <algorithm>

#include "RuntimeInternal.h"

#define EXEC_COMPLEX( symbol ) case Grammar::Symbol::##symbol: \
return new RuntimeInternal::CExec_##symbol( \
	convert( static_cast< AST::CComplexExpression* > ( expression )->Lhs() ), \
	convert( static_cast< AST::CComplexExpression* > ( expression )->Rhs() ), \
	static_cast< AST::CComplexExpression* > ( expression ) )

#define EXEC_VALUE( symbol ) case Grammar::Symbol::##symbol: \
return new RuntimeInternal::CExec_##symbol( static_cast< AST::CValueExpression* > ( expression ) )

namespace Runtime
{
	std::vector< IExec* > CreateExecutionObjects( const std::vector< AST::IExpression* >& expressions )
	{
		std::vector< IExec* > executors;

		std::function< IExec*( AST::IExpression* ) > convert;
		convert = [ &convert ]( AST::IExpression* expression ) -> IExec*
		{
			switch ( expression->Symbol() )
			{
				EXEC_COMPLEX( S_ADD );
				EXEC_VALUE( S_INTCNST );
			default:
				throw Exception( "Unknown expression: " + std::to_string( expression->Symbol() ) );
			}
		};

		std::transform( expressions.begin(), expressions.end(), std::back_inserter( executors ), [ &convert ]( AST::IExpression* expr ) {
			return convert( expr );
		} );

		return executors;
	}

	// TODO:
	//void FreeExecutionObjects( std::vector< IExec* >& executionObjects )
	//{
	//	for ( auto object : executionObjects )
	//		
	//}

	Context_t CreateDefaultContext()
	{
		auto ctx = Context_t();
		return ctx;
	}

	std::vector< Statement_t > Execute( const std::vector< AST::IExpression* >& expressions, Context_t& context )
	{
		std::vector< Statement_t > statementList;
		std::vector< IExec* > executors = CreateExecutionObjects( expressions );

		for ( auto node : executors )
			statementList.push_back( node->Execute( context ) );

		return statementList;
	}
}