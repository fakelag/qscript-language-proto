#include "AST.h"

#include "Grammar.h"
#include "Parser.h"
#include "Value.h"

namespace AST
{
	CComplexExpression::CComplexExpression( IExpression* lhs, IExpression* rhs, const std::string& token, const Lexer::DebugInfo_t& dbg )
	{
		m_LHS = lhs;
		m_RHS = rhs;
		m_DebugInfo = dbg;
		m_Token = token;
	}

	CSimpleExpression::CSimpleExpression( IExpression* expression, const std::string& token, const Lexer::DebugInfo_t& dbg )
	{
		m_Expression = expression;
		m_DebugInfo = dbg;
		m_Token = token;
	}

	CValueExpression::CValueExpression( CValue* value, const std::string& token, const Lexer::DebugInfo_t& dbg )
	{
		m_Value = value;
		m_DebugInfo = dbg;
		m_Token = token;
	}
}
