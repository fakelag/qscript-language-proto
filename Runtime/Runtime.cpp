#include "Runtime.h"
#include "Grammar.h"
#include <functional>
#include <algorithm>

#include "RuntimeInternal.h"

#define EXEC_COMPLEX( symbol ) case Grammar::Symbol::symbol: {\
	auto complex = new RuntimeInternal::CExec_##symbol( \
		convert( static_cast< AST::CComplexExpression* > ( expression )->Lhs() ), \
		convert( static_cast< AST::CComplexExpression* > ( expression )->Rhs() ), \
		expression->Location() ); \
	allocationList->push_back( complex ); \
	return complex; \
}

#define EXEC_SIMPLE( symbol ) case Grammar::Symbol::symbol: {\
	auto simple = new RuntimeInternal::CExec_##symbol( \
		convert( static_cast< AST::CSimpleExpression* > ( expression )->Expression() ), \
		expression->Location() ); \
	allocationList->push_back( simple ); \
	return simple; \
}

#define EXEC_LIST( symbol ) case Grammar::Symbol::symbol: {\
	std::vector< IExec* > objects; \
	auto astList = static_cast< AST::CListExpression* > ( expression )->List(); \
	std::transform( astList.begin(), astList.end(), std::back_inserter( objects ), convert ); \
	auto list = new RuntimeInternal::CExec_##symbol( \
		objects, \
		expression->Location() ); \
	allocationList->push_back( list ); \
	return list; \
}

#define EXEC_VALUE( symbol ) case Grammar::Symbol::symbol: { \
	auto value = new RuntimeInternal::CExec_##symbol( static_cast< AST::CValueExpression* > ( expression )->Value(), expression->Location() ); \
	allocationList->push_back( value ); \
	return value; \
}

namespace Runtime
{
	// Unknown location
	const static Grammar::SymbolLoc_t unknownLocation{ -1, -1, "[[unknown]]" };

	std::vector< IExec* > CreateExecutionObjects( const std::vector< AST::IExpression* >& expressions, std::vector< IExec* >* allocationList )
	{
		std::vector< IExec* > executors;

		std::function< IExec*( AST::IExpression* ) > convert;
		convert = [ &convert, &allocationList ]( AST::IExpression* expression ) -> IExec*
		{
			if ( expression == NULL )
				return NULL;

			switch ( expression->Symbol() )
			{
				EXEC_COMPLEX( S_ADD );
				EXEC_COMPLEX( S_CALL );
				EXEC_COMPLEX( S_FUNC );
				EXEC_COMPLEX( S_FUNCDEF );
				EXEC_SIMPLE( S_FUNCBODY );
				EXEC_LIST( S_SCOPE );
				EXEC_LIST( S_LIST );
				EXEC_VALUE( S_INTCNST );
				EXEC_VALUE( S_NAME );
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

	void CreateDefaultContext( bool isRepl, bool enableDebugging, CContext* ctx )
	{
		// Read-Eval-Print-Loop ?
		ctx->m_Repl = isRepl;

		// Push global scope into the stack
		ctx->PushScope( true );

#ifdef RTI_DEBUG_ENABLED
		ctx->m_Flag = 0;

		if ( enableDebugging )
		{
			// Link debugging functions
			static auto __setFlag = RuntimeInternal::CExec_internal___setFlag();

			ctx->PushFunction( "__setFlag", &__setFlag, NULL );
		}
#else
		if ( enableDebugging )
			throw Exception( "Unable to create a debuggable context. Debugging features are disabled." );
#endif
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

				auto main = context.m_Scopes[ 0 ].m_Functions.find( "main" );

				if ( main == context.m_Scopes[ 0 ].m_Functions.end() )
					throw Exception( "Entrypoint not found" );

				statementList.push_back( main->second->Execute( context ) );
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

		context.PopScope();
		return statementList;
	}

	void CContext::PushScope( bool isGlobal )
	{
		m_Scopes.push_back( Scope_t( isGlobal ) );
	}

	void CContext::PopScope()
	{
		m_Scopes.pop_back();
	}

	void CContext::PushFunction( const std::string& name, IExec* body, const Grammar::SymbolLoc_t* where )
	{
		if ( m_Scopes.size() <= 0 )
			throw RuntimeException( where ? *where : unknownLocation, "No available scope found" );

		auto function = m_Scopes[ m_Scopes.size() - 1 ].m_Functions.find( name );
		if ( function == m_Scopes[ m_Scopes.size() - 1 ].m_Functions.end() )
		{
			// Insert new function
			m_Scopes[ m_Scopes.size() - 1 ].m_Functions.insert( { name, body } );
		}
		else
		{
			if ( function->second == NULL )
			{
				// Link function to its body
				function->second = body;
			}
			else
			{
				// Function is already defined
				throw RuntimeException( where ? *where : unknownLocation, "Function " + name + " is already defined" );
			}
		}
	}

	void CContext::PushVariable( const std::string& name, const Value::CValue& value, const Grammar::SymbolLoc_t* where )
	{
		if ( m_Scopes.size() <= 0 )
			throw RuntimeException( where ? *where : unknownLocation, "No available scope found" );

		auto variable = m_Scopes[ m_Scopes.size() - 1 ].m_Variables.find( name );

		if ( variable == m_Scopes[ m_Scopes.size() - 1 ].m_Variables.end() )
		{
			m_Scopes[ m_Scopes.size() - 1 ].m_Variables.insert( { name, value } );
		}
		else
		{
			throw RuntimeException( where ? *where : unknownLocation, "Variable " + name + " already exists" );
		}
	}

	const CContext::Scope_t& CContext::GetCurrentScope() const
	{
		if ( m_Scopes.size() <= 0 )
			throw Exception( "GetCurrentScope - No available scope found" );

		return m_Scopes[ m_Scopes.size() - 1 ];
	}

	IExec* CContext::FindFunction( const std::string& name )
	{
		for ( size_t i = m_Scopes.size() - 1; i >= 0; --i )
		{
			auto function = m_Scopes[ i ].m_Functions.find( name );
			if ( function != m_Scopes[ i ].m_Functions.end() )
				return function->second;
		}

		return NULL;
	}

	const Value::CValue* CContext::FindVariable( const std::string& name )
	{
		for ( size_t i = m_Scopes.size() - 1; i >= 0; --i )
		{
			auto function = m_Scopes[ i ].m_Variables.find( name );
			if ( function != m_Scopes[ i ].m_Variables.end() )
				return &function->second;
		}

		return NULL;
	}
}