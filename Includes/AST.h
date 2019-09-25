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
		virtual ExpressionType Type() const = 0;
		virtual std::string ToString( int indent = 1 ) = 0;
	};

	class CComplexExpression : public IExpression
	{
	public:
		CComplexExpression( IExpression* lhs, IExpression* rhs, Grammar::Symbol symbol, const Grammar::SymbolLoc_t& loc );
		std::string ToString( int indent );

		ExpressionType 					Type() 			const { return ET_COMPLEX; };
		Grammar::Symbol 				Symbol()		const { return m_Symbol; }
		const Grammar::SymbolLoc_t& 	Location() 		const { return m_Loc; }
		IExpression* 					Lhs() 			const { return m_LHS; }
		IExpression* 					Rhs() 			const { return m_RHS; }

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

		ExpressionType 					Type() 			const { return ET_SIMPLE; };
		Grammar::Symbol 				Symbol()		const { return m_Symbol; }
		const Grammar::SymbolLoc_t& 	Location() 		const { return m_Loc; }
		IExpression*					Expression() 	const { return m_Expression; }

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

		ExpressionType 					Type() 			const { return ET_VALUE; };
		Grammar::Symbol 				Symbol()		const { return m_Symbol; }
		const Grammar::SymbolLoc_t& 	Location() 		const { return m_Loc; }

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

		ExpressionType 					Type() 			const { return ET_LIST; };
		Grammar::Symbol 				Symbol()		const { return m_Symbol; }
		const Grammar::SymbolLoc_t& 	Location() 		const { return m_Loc; }

	private:
		std::vector< IExpression* >		m_List;
		Grammar::SymbolLoc_t			m_Loc;
		Grammar::Symbol					m_Symbol;
	};
}
