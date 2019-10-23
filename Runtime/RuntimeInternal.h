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
		Grammar::Symbol::symbol; \
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
		Grammar::Symbol::symbol; \
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
		Grammar::Symbol::symbol; \
	} \
	Runtime::Statement_t Execute( Runtime::CContext& context ); \
private: \
	Value::CValue				m_Value; \
	Grammar::SymbolLoc_t		m_Loc; \
};

#define RTI_LIST_HANDLER( symbol ) \
class CExec_##symbol : public Runtime::IExec \
{ \
public: \
	CExec_##symbol( const std::vector< Runtime::IExec* >& list, const Grammar::SymbolLoc_t& loc ) \
	{ \
		m_List = list; \
		m_Loc = loc; \
		Grammar::Symbol::symbol; \
	} \
	Runtime::Statement_t Execute( Runtime::CContext& context ); \
private: \
	std::vector< Runtime::IExec* >	m_List; \
	Grammar::SymbolLoc_t			m_Loc; \
};

#define RTI_INTERNALFUNC_HANDLER( func ) \
class CExec_internal_##func : public Runtime::IExec \
{ \
public: \
	Runtime::Statement_t Execute( Runtime::CContext& context ); \
};

#define RTI_EXECFN( symbol ) \
Runtime::Statement_t RuntimeInternal::CExec_##symbol::Execute( Runtime::CContext& context )

#define RTI_INTERNALFN( func ) \
Runtime::Statement_t RuntimeInternal::CExec_internal_##func::Execute( Runtime::CContext& context )

namespace RuntimeInternal
{
	RTI_COMPLEX_HANDLER( S_ADD );
	RTI_COMPLEX_HANDLER( S_CALL );
	RTI_COMPLEX_HANDLER( S_FUNC );
	RTI_COMPLEX_HANDLER( S_FUNCDEF );

	RTI_SIMPLE_HANDLER( S_FUNCBODY );

	RTI_LIST_HANDLER( S_SCOPE );
	RTI_LIST_HANDLER( S_LIST );

	RTI_VALUE_HANDLER( S_INTCNST );
	RTI_VALUE_HANDLER( S_NAME );

	// Debug functions
#ifdef RTI_DEBUG_ENABLED
	RTI_INTERNALFUNC_HANDLER( __setFlag );
#endif
}
