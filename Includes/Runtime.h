#pragma once
#include "AST.h"
#include "Value.h"
#include <vector>

namespace Runtime
{
	class IExec;

	struct Statement_t
	{	
		Value::CValue			m_Value;
	};

	class CContext
	{
	public:
		struct Scope_t
		{
			std::map< std::string, IExec* >				m_Functions;
			std::map< std::string, Value::CValue >		m_Variables;
		};

		bool					m_Repl;
		std::vector< Scope_t >	m_Scopes;
	};

	class IExec
	{
	public:
		virtual ~IExec() {}
		virtual Statement_t Execute( CContext& context ) = 0;
	};

	CContext CreateDefaultContext( bool isRepl );
	std::vector< Statement_t > Execute( const std::vector< AST::IExpression* >& expressions, CContext& context );
}
