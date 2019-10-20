#pragma once
#include "Grammar.h"
#include "AST.h"

#define RTI_COMPLEX_HANDLER( symbol ) \
class CExec_##symbol : public Runtime::IExec \
{ \
public: \
	CExec_##symbol( Runtime::IExec* lhs, Runtime::IExec* rhs, const Grammar::SymbolLoc_t& loc ) \
	{ \
		m_LHS = lhs; \
		m_RHS = rhs; \
		m_Loc = loc; \
		Grammar::Symbol::##symbol; \
	} \
	Runtime::Statement_t Execute( Runtime::CContext& context ); \
private: \
	Runtime::IExec*				m_LHS; \
	Runtime::IExec*				m_RHS; \
	Grammar::SymbolLoc_t		m_Loc; \
};

#define RTI_SIMPLE_HANDLER( symbol ) \
class CExec_##symbol : public Runtime::IExec \
{ \
public: \
	CExec_##symbol( Runtime::IExec* expr, const Grammar::SymbolLoc_t& loc ) \
	{ \
		m_Expr = expr; \
		m_Loc = loc; \
		Grammar::Symbol::##symbol; \
	} \
	Runtime::Statement_t Execute( Runtime::CContext& context ); \
private: \
	Runtime::IExec*				m_Expr; \
	Grammar::SymbolLoc_t		m_Loc; \
};

#define RTI_VALUE_HANDLER( symbol ) \
class CExec_##symbol : public Runtime::IExec \
{ \
public: \
	CExec_##symbol( const Value::CValue& value, const Grammar::SymbolLoc_t& loc ) \
	{ \
		m_Value = value; \
		m_Loc = loc; \
		Grammar::Symbol::##symbol; \
	} \
	Runtime::Statement_t Execute( Runtime::CContext& context ); \
private: \
	Value::CValue				m_Value; \
	Grammar::SymbolLoc_t		m_Loc; \
};

#define RTI_EXECFN( symbol ) \
Runtime::Statement_t RuntimeInternal::CExec_##symbol::Execute( Runtime::CContext& context )

namespace RuntimeInternal
{
	RTI_COMPLEX_HANDLER( S_ADD );
	RTI_COMPLEX_HANDLER( S_FUNC );
	RTI_COMPLEX_HANDLER( S_FUNCDEF );

	RTI_SIMPLE_HANDLER( S_FUNCBODY );

	RTI_VALUE_HANDLER( S_INTCNST );
}
