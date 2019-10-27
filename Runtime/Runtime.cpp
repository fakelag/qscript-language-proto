#include "Runtime.h"
#include "Grammar.h"
#include <functional>
#include <algorithm>

#include "Core/RuntimeInternal.h"

namespace Runtime
{
	// Unknown location
	const static Grammar::SymbolLoc_t unknownLocation{ -1, -1, "[[unknown]]" };

	std::vector< IExec* > CreateExecutionObjects( const std::vector< AST::IExpression* >& expressions, std::vector< IExec* >* allocationList )
	{
		std::vector< IExec* > executors;

		// Convert AST expressions into executor objects
		std::function< IExec*( AST::IExpression* ) > convert;
		convert = [ &convert, &allocationList ]( AST::IExpression* expression ) -> IExec*
		{
			if ( expression == NULL )
				return NULL;

			// List all enabled operators with macros
			EXEC_COMPLEX( S_ADD );
			EXEC_COMPLEX( S_SUB );
			EXEC_COMPLEX( S_DIV );
			EXEC_COMPLEX( S_MUL );
			EXEC_COMPLEX( S_CALL );
			EXEC_COMPLEX( S_FUNC );
			EXEC_COMPLEX( S_FUNCDEF );
			EXEC_SIMPLE( S_SUB );
			EXEC_SIMPLE( S_ADD );
			EXEC_SIMPLE( S_FUNCBODY );
			EXEC_LIST( S_SCOPE );
			EXEC_LIST( S_LIST );
			EXEC_VALUE( S_DBLCNST );
			EXEC_VALUE( S_INTCNST );
			EXEC_VALUE( S_NAME );

			throw Exception( "Unknown expression: " + std::to_string( expression->Symbol() ) );
		};

		std::transform( expressions.begin(), expressions.end(), std::back_inserter( executors ), [ &convert ]( AST::IExpression* expr ) {
			return convert( expr );
		} );

		return executors;
	}

	void FreeExecutionObjects( std::vector< IExec* >* allocationList )
	{
		// Release all resources allocated for execution
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
		// Reset the debugging flag. This flag is used by tests
		// to verify that certain code paths were properly triggered
		ctx->m_Flag = 0;

		if ( enableDebugging )
		{
			// Link debugging functions
			static auto __setFlag = RuntimeInternal::CExec_Internal___setFlag();

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

		// Allocate executors
		std::vector< IExec* > executors = CreateExecutionObjects( expressions, &allocationList );

		try
		{
			if ( context.m_Repl )
			{
				// For REPL mode, execute all the expressions
				// and return a statement for each
				for ( auto node : executors )
					statementList.push_back( node->Execute( context ) );
			}
			else
			{
				// Execute top-level code, such as class and function declarations
				for ( auto node : executors )
					node->Execute( context );

				// find "main" function
				auto main = context.m_Scopes[ 0 ].m_Functions.find( "main" );

				if ( main == context.m_Scopes[ 0 ].m_Functions.end() )
					throw Exception( "Entrypoint not found" );

				// Execute code from the program's main entrypoint
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

		// Pop global scope
		context.PopScope();

		// Free all resources
		FreeExecutionObjects( &allocationList );
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
			// Insert new variable
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