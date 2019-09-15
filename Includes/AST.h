#pragma once

#include <string>
#include "Lexer.h"

namespace AST
{
	class CValue;

	enum ExpressionType
	{
		ET_SIMPLE = 0,
		ET_VALUE,
		ET_COMPLEX,
	};

	class IExpression
	{
	public:
		virtual ~IExpression() {};
		virtual ExpressionType Type() = 0;
	};

	class CComplexExpression : IExpression
	{
	public:
		CComplexExpression( IExpression* lhs, IExpression* rhs, const std::string& token, const Lexer::DebugInfo_t& dbg );
		ExpressionType Type() { return ET_COMPLEX; };
	private:
		IExpression*			m_LHS;
		IExpression*			m_RHS;
		Lexer::DebugInfo_t		m_DebugInfo;
		std::string				m_Token;
	};

	class CSimpleExpression : IExpression
	{
	public:
		CSimpleExpression( IExpression* expression, const std::string& token, const Lexer::DebugInfo_t& dbg );
		ExpressionType Type() { return ET_SIMPLE; };
	private:
		IExpression*			m_Expression;
		Lexer::DebugInfo_t		m_DebugInfo;
		std::string				m_Token;
	};

	class CValueExpression : IExpression
	{
	public:
		CValueExpression( CValue* value, const std::string& token, const Lexer::DebugInfo_t& dbg );
		ExpressionType Type() { return ET_VALUE; };
	private:
		CValue*					m_Value;
		Lexer::DebugInfo_t		m_DebugInfo;
		std::string				m_Token;
	};
}
