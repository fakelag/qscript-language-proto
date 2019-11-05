#include "Runtime.h"
#include "Grammar.h"
#include <functional>
#include <algorithm>

#include "Core/RuntimeInternal.h"

namespace Runtime
{
	// Unknown location
	const static Grammar::SymbolLoc_t unknownLocation{ -1, -1, "[[unknown]]" };

	std::vector< IExec* > CreateExecutionObjects( const std::vector< AST::IExpression* >& expressions, CContext* context )
	{
		std::vector< IExec* > executors;

		// Convert AST expressions into executor objects
		std::function< IExec*( AST::IExpression* ) > convert;
		convert = [ &convert, &context ]( AST::IExpression* expression ) -> IExec*
		{
			if ( expression == NULL )
				return NULL;

			// List all enabled operators with macros
			EXEC_COMPLEX( S_ADD );
			EXEC_COMPLEX( S_SUB );
			EXEC_COMPLEX( S_DIV );
			EXEC_COMPLEX( S_MUL );
			EXEC_COMPLEX( S_POW );
			EXEC_COMPLEX( S_MOD );
			EXEC_COMPLEX( S_CALL );
			EXEC_COMPLEX( S_FUNC );
			EXEC_COMPLEX( S_FUNCDEF );
			EXEC_COMPLEX( S_ASSIGN );
			EXEC_COMPLEX( S_EQUALS );
			EXEC_COMPLEX( S_EQUALS_NOT );
			EXEC_COMPLEX( S_WHILE );
			EXEC_COMPLEX( S_IF );
			EXEC_COMPLEX( S_MORETHAN );
			EXEC_COMPLEX( S_LESSTHAN );
			EXEC_COMPLEX( S_LESS_OR_EQUALS );
			EXEC_COMPLEX( S_MORE_OR_EQUALS );
			EXEC_COMPLEX( S_ASSIGN_ADD );
			EXEC_COMPLEX( S_ASSIGN_SUB );
			EXEC_COMPLEX( S_ASSIGN_MUL );
			EXEC_COMPLEX( S_ASSIGN_DIV );
			EXEC_COMPLEX( S_ASSIGN_MOD );
			EXEC_COMPLEX( S_LOGIC_OR );
			EXEC_COMPLEX( S_LOGIC_AND );
			EXEC_COMPLEX( S_INCREMENT );
			EXEC_COMPLEX( S_DECREMENT );
			EXEC_SIMPLE( S_SUB );
			EXEC_SIMPLE( S_ADD );
			EXEC_SIMPLE( S_FUNCBODY );
			EXEC_SIMPLE( S_VAR );
			EXEC_SIMPLE( S_LOGIC_NOT );
			EXEC_LIST( S_SCOPE );
			EXEC_LIST( S_LIST );
			EXEC_LIST( S_FOR );
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

	void CreateDefaultContext( bool isRepl, bool enableDebugging, CContext* ctx )
	{
		// Read-Eval-Print-Loop ?
		ctx->m_Repl = isRepl;
		ctx->m_ExecFlags = 0;

		// Push global scope into the stack
		ctx->PushScope( true, false );

#ifdef RTI_DEBUG_ENABLED
		// Reset the debugging flag. This flag is used by tests
		// to verify that certain code paths were properly triggered
		ctx->m_Flag = 0;

		if ( enableDebugging )
		{
			// Link debugging functions
			static auto __setFlag = RuntimeInternal::CExec_Internal___setFlag();

			ctx->PushFunction( "__setFlag", &__setFlag, {}, NULL );
		}
#else
		if ( enableDebugging )
			throw Exception( "Unable to create a debuggable context. Debugging features are disabled." );
#endif
	}

	std::vector< Statement_t > Execute( const std::vector< AST::IExpression* >& expressions, CContext& context )
	{
		std::vector< Statement_t > statementList;

		// Allocate executors
		std::vector< IExec* > executors = CreateExecutionObjects( expressions, &context );

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
				statementList.push_back( main->second.m_Body->Execute( context ) );
			}
		}
		catch ( const RuntimeException& exception )
		{
			if ( !context.m_Repl )
				context.Release();

			throw exception;
		}
		catch ( const Exception& exception )
		{
			if ( !context.m_Repl )
				context.Release();

			throw exception;
		}

		// Release the context
		if ( !context.m_Repl )
			context.Release();

		return statementList;
	}

	void CContext::PushScope( bool isGlobal, bool isArgsScope )
	{
		m_Scopes.push_back( Scope_t( isGlobal, isArgsScope ) );
	}

	void CContext::PopScope()
	{
		m_Scopes.pop_back();
	}

	void CContext::PushFunction( const std::string& name, IExec* body, const std::vector< std::string >& args, const Grammar::SymbolLoc_t* where )
	{
		if ( m_Scopes.size() <= 0 )
			throw RuntimeException( where ? *where : unknownLocation, "No available scope found" );

		auto function = m_Scopes[ m_Scopes.size() - 1 ].m_Functions.find( name );
		if ( function == m_Scopes[ m_Scopes.size() - 1 ].m_Functions.end() )
		{
			// Insert new function
			auto funcDef = Function_t{ args, body };
			m_Scopes[ m_Scopes.size() - 1 ].m_Functions.insert({ name, funcDef });
		}
		else
		{
			if ( function->second.m_Body == NULL )
			{
				// Link function to its body
				function->second.m_Body = body;
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

	Runtime::CContext::Function_t* CContext::FindFunction( const std::string& name )
	{
		for ( int i = ( int ) m_Scopes.size() - 1; i >= 0; --i )
		{
			auto function = m_Scopes[ i ].m_Functions.find( name );
			if ( function != m_Scopes[ i ].m_Functions.end() )
				return &function->second; // references to std::unordered_map elements should be safe

			if ( m_Scopes[ i ].m_IsArgsScope )
			{
				// This is the last subscope to check, now consult the global scope
				function = m_Scopes[ 0 ].m_Functions.find( name );
				if ( function != m_Scopes[ 0 ].m_Functions.end() )
					return &function->second;

				break;
			}
		}

		return NULL;
	}

	Value::CValue* CContext::FindVariable( const std::string& name )
	{
		for ( int i = ( int ) m_Scopes.size() - 1; i >= 0; --i )
		{
			auto variable = m_Scopes[ i ].m_Variables.find( name );
			if ( variable != m_Scopes[ i ].m_Variables.end() )
				return &variable->second; // references to std::unordered_map elements should be safe

			if ( m_Scopes[ i ].m_IsArgsScope )
			{
				// This is the last subscope to check, now consult the global scope
				variable = m_Scopes[ 0 ].m_Variables.find( name );
				if ( variable != m_Scopes[ 0 ].m_Variables.end() )
					return &variable->second;

				break;
			}
		}

		return NULL;
	}

	void CContext::AddObject( IExec* exec )
	{
		m_AllocationList.push_back( exec );
	}

	unsigned long CContext::AddFlag( unsigned long flag )
	{
		return ( m_ExecFlags |= flag );
	}

	unsigned long CContext::RemoveFlag( unsigned long flag )
	{
		return ( m_ExecFlags &= ~flag );
	}

	void CContext::Release()
	{
		// Release all resources allocated for execution
		for ( int i = ( int ) m_AllocationList.size() - 1; i >= 0; --i )
			delete m_AllocationList[ i ];

		m_AllocationList.clear();

		// Pop global scope
		PopScope();
	}
}
