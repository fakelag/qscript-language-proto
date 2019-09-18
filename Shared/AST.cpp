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

	std::string CComplexExpression::ToString(int indent)
	{
		std::string output = std::string(indent - 1 < 0 ? indent : indent - 1, '\t') + "{\n";
		output += std::string(indent, '\t') + "type: COMPLEX\n";
		output += std::string(indent, '\t') + "token: " + m_Token + "\n";
		output += std::string(indent, '\t') + "lhs: \n" + m_LHS->ToString(indent + 1);
		output += std::string(indent, '\t') + "rhs: \n" + m_RHS->ToString(indent + 1);
		output += std::string(indent - 1 < 0 ? indent : indent - 1, '\t') + "}\n";
		return output;
	}

	CSimpleExpression::CSimpleExpression( IExpression* expression, const std::string& token, const Lexer::DebugInfo_t& dbg )
	{
		m_Expression = expression;
		m_DebugInfo = dbg;
		m_Token = token;
	}

	std::string CSimpleExpression::ToString(int indent)
	{
		std::string output = std::string(indent - 1 < 0 ? indent : indent - 1, '\t') + "{\n";
		output += std::string(indent, '\t') + "type: SIMPLE\n";
		output += std::string(indent, '\t') + "token: " + m_Token + "\n";
		output += std::string(indent, '\t') + "expression: \n" + m_Expression->ToString(indent + 1);
		output += std::string(indent - 1 < 0 ? indent : indent - 1, '\t') + "}\n";
		return output;
	}

	CValueExpression::CValueExpression( const Value::CValue& value, const std::string& token, const Lexer::DebugInfo_t& dbg )
	{
		m_Value = Value::CValue( value );
		m_DebugInfo = dbg;
		m_Token = token;
	}

	std::string CValueExpression::ToString(int indent)
	{
		static const char* s_ValueTypes[] = {
			"STRING",
			"INT",
			"DOUBLE"
		};

		std::string output = std::string(indent - 1 < 0 ? indent : indent - 1, '\t') + "{\n";
		output += std::string(indent, '\t') + "type: VALUE\n";
		output += std::string(indent, '\t') + "value: " + m_Value.GetString() + "\n";
		output += std::string(indent, '\t') + "valueType: " + std::string(s_ValueTypes[m_Value.GetType()]) + "\n";
		output += std::string(indent - 1 < 0 ? indent : indent - 1, '\t') + "}\n";
		return output;
	}

	CListExpression::CListExpression( const std::vector< IExpression* >& list, const std::string& token, const Lexer::DebugInfo_t& dbg )
	{
		m_List = list;
		m_DebugInfo = dbg;
		m_Token = token;
	}

	std::string CListExpression::ToString(int indent)
	{
		std::string output = std::string(indent - 1 < 0 ? indent : indent - 1, '\t') + "{\n";
		output += std::string(indent, '\t') + "type: LIST\n";
		output += std::string(indent, '\t') + "length: " + std::to_string(m_List.size()) + "\n";
		output += std::string(indent, '\t') + "items: [\n";

		for (auto expression : m_List)
			output += expression->ToString(indent + 1);

		output += std::string(indent - 1 < 0 ? indent : indent - 1, '\t') + "]\n" + "}\n";
		return output;
	}
}
