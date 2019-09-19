#include "AST.h"

#include "Grammar.h"
#include "Parser.h"
#include "Value.h"

namespace AST
{
	CComplexExpression::CComplexExpression( IExpression* lhs, IExpression* rhs, Grammar::Symbol symbol, const Lexer::DebugInfo_t& dbg )
	{
		m_LHS = lhs;
		m_RHS = rhs;
		m_DebugInfo = dbg;
		m_Symbol = symbol;
	}

	std::string CComplexExpression::ToString(int indent)
	{
		std::string token = "UNKNOWN";
		auto commonSymbol = Grammar::LanguageSymbols.find(m_Symbol);

		if (commonSymbol != Grammar::LanguageSymbols.end())
			token = commonSymbol->second.m_Token;

		std::string output = std::string(indent - 1 < 0 ? indent : indent - 1, '\t') + "{\n";
		output += std::string(indent, '\t') + "type: COMPLEX\n";
		output += std::string(indent, '\t') + "token: " + token + "\n";
		output += std::string(indent, '\t') + "lhs: \n" + m_LHS->ToString(indent + 1);
		output += std::string(indent, '\t') + "rhs: \n" + m_RHS->ToString(indent + 1);
		output += std::string(indent - 1 < 0 ? indent : indent - 1, '\t') + "}\n";
		return output;
	}

	CSimpleExpression::CSimpleExpression( IExpression* expression, Grammar::Symbol symbol, const Lexer::DebugInfo_t& dbg )
	{
		m_Expression = expression;
		m_DebugInfo = dbg;
		m_Symbol = symbol;
	}

	std::string CSimpleExpression::ToString(int indent)
	{
		std::string token = "UNKNOWN";
		auto commonSymbol = Grammar::LanguageSymbols.find(m_Symbol);

		if (commonSymbol != Grammar::LanguageSymbols.end())
			token = commonSymbol->second.m_Token;

		std::string output = std::string(indent - 1 < 0 ? indent : indent - 1, '\t') + "{\n";
		output += std::string(indent, '\t') + "type: SIMPLE\n";
		output += std::string(indent, '\t') + "token: " + token + "\n";
		output += std::string(indent, '\t') + "expression: \n" + m_Expression->ToString(indent + 1);
		output += std::string(indent - 1 < 0 ? indent : indent - 1, '\t') + "}\n";
		return output;
	}

	CValueExpression::CValueExpression( const Value::CValue& value, Grammar::Symbol symbol, const Lexer::DebugInfo_t& dbg )
	{
		m_Value = Value::CValue( value );
		m_DebugInfo = dbg;
		m_Symbol = symbol;
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

	CListExpression::CListExpression( const std::vector< IExpression* >& list, Grammar::Symbol symbol, const Lexer::DebugInfo_t& dbg )
	{
		m_List = list;
		m_DebugInfo = dbg;
		m_Symbol = symbol;
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
