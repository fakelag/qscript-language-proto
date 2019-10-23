#pragma once

#include <string>
#include <vector>
#include "Value.h"
#include "Lexer.h"

#if defined(_WIN32)
#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif
#elif defined(_OSX)
#ifndef FORCEINLINE
#define FORCEINLINE inline
#endif
#endif

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
		virtual ExpressionType 					Type() const = 0;
		virtual Grammar::Symbol 				Symbol() const = 0;
		virtual const Grammar::SymbolLoc_t& 	Location() const = 0;
		virtual std::string 					ToString( int indent = 1 ) const = 0;
	};

	class CComplexExpression : public IExpression
	{
	public:
		CComplexExpression( IExpression* lhs, IExpression* rhs, Grammar::Symbol symbol, const Grammar::SymbolLoc_t& loc );
		std::string ToString( int indent ) const;

		ExpressionType 					Type() 						const { return ET_COMPLEX; };
		Grammar::Symbol 				Symbol()					const { return m_Symbol; }
		const Grammar::SymbolLoc_t& 	Location() 					const { return m_Loc; }
		IExpression* 					Lhs() 						const { return m_LHS; }
		IExpression* 					Rhs() 						const { return m_RHS; }

		void* operator new ( size_t size );
		void operator delete ( void* p );

	protected:
		CComplexExpression() {}

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
		std::string ToString( int indent ) const;

		ExpressionType 					Type() 						const { return ET_SIMPLE; };
		Grammar::Symbol 				Symbol()					const { return m_Symbol; }
		const Grammar::SymbolLoc_t& 	Location() 					const { return m_Loc; }
		IExpression*					Expression() 				const { return m_Expression; }

		void* operator new ( size_t size );
		void operator delete ( void* p );

	protected:
		CSimpleExpression() {}

	private:
		IExpression*			m_Expression;
		Grammar::SymbolLoc_t	m_Loc;
		Grammar::Symbol			m_Symbol;
	};

	class CValueExpression : public IExpression
	{
	public:
		CValueExpression( const Value::CValue& value, Grammar::Symbol symbol, const Grammar::SymbolLoc_t& loc );
		std::string ToString( int indent ) const;

		ExpressionType 					Type() 					const { return ET_VALUE; };
		Grammar::Symbol 				Symbol()				const { return m_Symbol; }
		const Grammar::SymbolLoc_t& 	Location() 				const { return m_Loc; }
		Value::CValue&					Value()					{ return m_Value; }

		void* operator new ( size_t size );
		void operator delete ( void* p );

	protected:
		CValueExpression() {}

	private:
		Value::CValue			m_Value;
		Grammar::SymbolLoc_t	m_Loc;
		Grammar::Symbol			m_Symbol;
	};

	class CListExpression : public IExpression
	{
	public:
		CListExpression( const std::vector< IExpression* >& list, Grammar::Symbol symbol, const Grammar::SymbolLoc_t& loc );
		std::string ToString( int indent ) const;

		ExpressionType 					Type() 					const { return ET_LIST; };
		Grammar::Symbol 				Symbol()				const { return m_Symbol; }
		const Grammar::SymbolLoc_t& 	Location() 				const { return m_Loc; }
		std::vector< IExpression* >&	List()					{ return m_List; }

		void* operator new ( size_t size );
		void operator delete ( void* p );

		protected:
			CListExpression() {}

	private:
		std::vector< IExpression* >		m_List;
		Grammar::SymbolLoc_t			m_Loc;
		Grammar::Symbol					m_Symbol;
	};

	void FreeNode( IExpression* expression );
	void FreeTree( const std::vector< IExpression* > tree );

	FORCEINLINE bool IsNumberConstant( AST::IExpression* expression )
	{
		if ( expression->Symbol() != Grammar::Symbol::S_INTCNST && expression->Symbol() != Grammar::Symbol::S_DBLCNST )
			return false;

		return true;
	}

	FORCEINLINE bool IsNameConstant( AST::IExpression* expression )
	{
		if ( expression->Symbol() != Grammar::Symbol::S_NAME )
			return false;

		return true;
	}

	FORCEINLINE bool IsVariable( AST::IExpression* expression )
	{
		auto rightExpr = expression;

		while ( rightExpr->Symbol() == Grammar::Symbol::S_DOT )
			rightExpr = static_cast< CComplexExpression* >( rightExpr )->Rhs();

		return IsNameConstant( rightExpr );
	}

	void PushTrackAlloc();
	void PopAllocTracking();
	const std::vector< IExpression* >& AllocatedExpressions();
}
