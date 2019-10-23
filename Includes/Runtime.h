#pragma once
#include "AST.h"
#include "Value.h"
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
		struct Scope_t
		{
			Scope_t( bool isGlobal )
			{
				m_IsGlobal = isGlobal;
			}

			std::map< std::string, IExec* >				m_Functions;
			std::map< std::string, Value::CValue >		m_Variables;
			bool										m_IsGlobal;
		};

		void					PushScope( bool isGlobal );
		void					PushFunction( const std::string& name, IExec* body, const Grammar::SymbolLoc_t* where );
		void					PushVariable( const std::string& name, const Value::CValue& value, const Grammar::SymbolLoc_t* where );
		const Scope_t&			GetCurrentScope() const;

		void					PopScope();

		IExec*					FindFunction( const std::string& name );
		const Value::CValue*	FindVariable( const std::string& name );


		bool					m_Repl;
		std::vector< Scope_t >	m_Scopes;

#ifdef RTI_DEBUG_ENABLED
			int 				m_Flag;
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
