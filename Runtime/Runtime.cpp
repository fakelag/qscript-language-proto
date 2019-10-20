#include "Runtime.h"
#include "Grammar.h"
#include <functional>
#include <algorithm>

#include "RuntimeInternal.h"

#define EXEC_COMPLEX( symbol ) case Grammar::Symbol::##symbol: {\
	auto complex = new RuntimeInternal::CExec_##symbol( \
		convert( static_cast< AST::CComplexExpression* > ( expression )->Lhs() ), \
		convert( static_cast< AST::CComplexExpression* > ( expression )->Rhs() ), \
		expression->Location() ); \
	allocationList->push_back( complex ); \
	return complex; \
}

#define EXEC_VALUE( symbol ) case Grammar::Symbol::##symbol: { \
	auto value = new RuntimeInternal::CExec_##symbol( static_cast< AST::CValueExpression* > ( expression )->Value(), expression->Location() ); \
	allocationList->push_back( value ); \
	return value; \
}

namespace Runtime
{
	std::vector< IExec* > CreateExecutionObjects( const std::vector< AST::IExpression* >& expressions, std::vector< IExec* >* allocationList )
	{
		std::vector< IExec* > executors;

		std::function< IExec*( AST::IExpression* ) > convert;
		convert = [ &convert, &allocationList ]( AST::IExpression* expression ) -> IExec*
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

	void FreeExecutionObjects( std::vector< IExec* >* allocationList )
	{
		for ( size_t i = 0; i < allocationList->size(); ++i )
			delete ( *allocationList )[ i ];

		allocationList->clear();
	}

	CContext CreateDefaultContext( bool isRepl )
	{
		auto ctx = CContext();

		ctx.m_Repl = isRepl;

		return ctx;
	}

	std::vector< Statement_t > Execute( const std::vector< AST::IExpression* >& expressions, CContext& context )
	{
		std::vector< IExec* > allocationList;
		std::vector< Statement_t > statementList;

		std::vector< IExec* > executors = CreateExecutionObjects( expressions, &allocationList );

		try
		{
			if ( context.m_Repl )
			{
				for ( auto node : executors )
					statementList.push_back( node->Execute( context ) );
			}
			else
			{
				for ( auto node : executors )
					node->Execute( context );

				// TODO: execute main() from context
			}
		}
		catch ( const RuntimeException& exception )
		{
			FreeExecutionObjects( &allocationList );
			throw exception;
		}
		catch ( const Exception& exception )
		{
			FreeExecutionObjects( &allocationList );
			throw exception;
		}

		return statementList;
	}
}