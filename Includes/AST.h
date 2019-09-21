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
		virtual std::string ToString( int indent = 1 ) = 0;
	};

	class CComplexExpression : public IExpression
	{
	public:
		CComplexExpression( IExpression* lhs, IExpression* rhs, Grammar::Symbol symbol, const Grammar::SymbolLoc_t& loc );
		std::string ToString( int indent );
		ExpressionType Type() { return ET_COMPLEX; };
	private:
		IExpression*			m_LHS;
		IExpression*			m_RHS;
		Grammar::SymbolLoc_t	m_Loc;
		Grammar::Symbol			m_Symbol;
	};

	class CSimpleExpression : public IExpression
	{
	public:
		CSimpleExpression( IExpression* expression, Grammar::Symbol symbol, const Grammar::SymbolLoc_t& loc );
		std::string ToString( int indent );
		ExpressionType Type() { return ET_SIMPLE; };
	private:
		IExpression*			m_Expression;
		Grammar::SymbolLoc_t	m_Loc;
		Grammar::Symbol			m_Symbol;
	};

	class CValueExpression : public IExpression
	{
	public:
		CValueExpression( const Value::CValue& value, Grammar::Symbol symbol, const Grammar::SymbolLoc_t& loc );
		std::string ToString( int indent );
		ExpressionType Type() { return ET_VALUE; };
	private:
		Value::CValue			m_Value;
		Grammar::SymbolLoc_t	m_Loc;
		Grammar::Symbol			m_Symbol;
	};

	class CListExpression : public IExpression
	{
	public:
		CListExpression( const std::vector< IExpression* >& list, Grammar::Symbol symbol, const Grammar::SymbolLoc_t& loc );
		std::string ToString( int indent );
		ExpressionType Type() { return ET_LIST; };
	private:
		std::vector< IExpression* >		m_List;
		Grammar::SymbolLoc_t			m_Loc;
		Grammar::Symbol					m_Symbol;
	};
}
