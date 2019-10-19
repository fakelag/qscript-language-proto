#pragma once
#include "AST.h"
#include "Value.h"
#include <vector>

namespace Runtime
{
	struct Statement_t
	{
		Value::CValue m_Value;
	};

	struct Context_t
	{

	};

	class IExec
	{
	public:
		virtual ~IExec() {}
		virtual Statement_t Execute( Context_t& context ) = 0;
	};

	Context_t CreateDefaultContext();
	std::vector< Statement_t > Execute( const std::vector< AST::IExpression* >& expressions, Context_t& context );
}
