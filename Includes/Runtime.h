#pragma once
#include "AST.h"
#include "Value.h"
#include <unordered_map>
#include <vector>

namespace Runtime
{
	class IExec;

	struct Statement_t
	{
		Value::CValue m_Value;
	};

	class CContext
	{
	public:
		enum ContextFlags
		{
			// Don't resolve names to variables. Return the name itself instead
			CF_NORESOLVE = ( 1<<0 ),
		};

		enum ScopeType
		{
			ST_GLOBAL = 0,
			ST_NORMAL,
			ST_ARGS,
			ST_LOOP,
		};

		struct Function_t
		{
			std::vector< std::string >	m_Args;
			IExec* 						m_Body;
		};

		struct Scope_t
		{
			Scope_t( ScopeType scopeType )
			{
				m_ScopeType = scopeType;
				m_IsBreaking = false;
			}

			std::unordered_map< std::string, Function_t >			m_Functions;
			std::unordered_map< std::string, Value::CValue >		m_Variables;
			ScopeType												m_ScopeType;
			bool 													m_IsBreaking;
		};

		void					PushScope( ScopeType type );
		void					PushFunction( const std::string& name, IExec* body, const std::vector< std::string >& args, const Grammar::SymbolLoc_t* where );
		void					PushVariable( const std::string& name, const Value::CValue& value, const Grammar::SymbolLoc_t* where );
		void 					BreakScope( bool isReturn, const Grammar::SymbolLoc_t* where );
		const Scope_t&			GetCurrentScope() const;

		void					AddObject( IExec* exec );
		void					Release();
		void					PopScope();

		unsigned long			AddFlag( unsigned long flag );
		unsigned long			RemoveFlag( unsigned long flag );

		Function_t*				FindFunction( const std::string& name );
		Value::CValue*			FindVariable( const std::string& name );

		bool					m_Repl;
		unsigned long			m_ExecFlags;
		std::vector< Scope_t* >	m_Scopes;
		std::vector< IExec* >	m_AllocationList;

#ifdef RTI_DEBUG_ENABLED
		int 					m_Flag;
#endif
	};

	class IExec
	{
	public:
		virtual ~IExec() {}
		virtual Statement_t Execute( CContext& context ) = 0;
	};

	void CreateDefaultContext( bool isRepl, bool enableDebugging, CContext* context );
	std::vector< Statement_t > Execute( const std::vector< AST::IExpression* >& expressions, CContext& context );
}
