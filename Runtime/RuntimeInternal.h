#pragma once
#include "Grammar.h"
#include "AST.h"

#define DEF_COMPLEX_HANDLER( symbol ) \
class CExec_##symbol : public Runtime::IExec \
{ \
public: \
	CExec_##symbol( Runtime::IExec* lhs, Runtime::IExec* rhs, AST::CComplexExpression* expr ) \
	{ \
		m_LHS = lhs; \
		m_RHS = rhs; \
		m_Expr = expr; \
		Grammar::Symbol::##symbol; \
	} \
	Runtime::Statement_t Execute( Runtime::Context_t& context ); \
private: \
	Runtime::IExec*				m_LHS; \
	Runtime::IExec*				m_RHS; \
	AST::CComplexExpression*	m_Expr; \
};

#define DEF_VALUE_HANDLER( symbol ) \
class CExec_##symbol : public Runtime::IExec \
{ \
public: \
	CExec_##symbol( AST::CValueExpression* expr ) { m_Expr = expr; Grammar::Symbol::##symbol; } \
	Runtime::Statement_t Execute( Runtime::Context_t& context ); \
private: \
	AST::CValueExpression*		m_Expr; \
};

#define DEF_EXECFN( symbol ) \
Runtime::Statement_t RuntimeInternal::CExec_##symbol::Execute( Runtime::Context_t& context )

namespace RuntimeInternal
{
	DEF_COMPLEX_HANDLER( S_ADD );
	DEF_VALUE_HANDLER( S_INTCNST );
}