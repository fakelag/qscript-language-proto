#pragma once

#include <string>
#include <vector>
#include "Value.h"
#include "Lexer.h"

namespace AST
{
	enum ExpressionType
	{
		ET_SIMPLE = 0,
		ET_VALUE,
		ET_LIST,
		ET_COMPLEX,
	};

	class IExpression
	{
	public:
		virtual ~IExpression() {};
		virtual ExpressionType Type() = 0;
		virtual std::string ToString(int indent = 1) = 0;
	};

	class CComplexExpression : public IExpression
	{
	public:
		CComplexExpression( IExpression* lhs, IExpression* rhs, const std::string& token, const Lexer::DebugInfo_t& dbg );
		std::string ToString(int indent);
		ExpressionType Type() { return ET_COMPLEX; };
	private:
		IExpression*			m_LHS;
		IExpression*			m_RHS;
		Lexer::DebugInfo_t		m_DebugInfo;
		std::string				m_Token;
	};

	class CSimpleExpression : public IExpression
	{
	public:
		CSimpleExpression( IExpression* expression, const std::string& token, const Lexer::DebugInfo_t& dbg );
		std::string ToString(int indent);
		ExpressionType Type() { return ET_SIMPLE; };
	private:
		IExpression*			m_Expression;
		Lexer::DebugInfo_t		m_DebugInfo;
		std::string				m_Token;
	};

	class CValueExpression : public IExpression
	{
	public:
		CValueExpression( const Value::CValue& value, const std::string& token, const Lexer::DebugInfo_t& dbg );
		std::string ToString(int indent);
		ExpressionType Type() { return ET_VALUE; };
	private:
		Value::CValue			m_Value;
		Lexer::DebugInfo_t		m_DebugInfo;
		std::string				m_Token;
	};

	class CListExpression : public IExpression
	{
	public:
		CListExpression( const std::vector< IExpression* >& list, const std::string& token, const Lexer::DebugInfo_t& dbg );
		std::string ToString(int indent);
		ExpressionType Type() { return ET_LIST; };
	private:
		std::vector< IExpression* >	m_List;
		Lexer::DebugInfo_t			m_DebugInfo;
		std::string					m_Token;
	};
}
