#pragma once
#include "Grammar.h"
#include "AST.h"

#define RTI_COMPLEX_HANDLER( symbol ) \
class CExec_Complex_##symbol : public Runtime::IExec \
{ \
public: \
	CExec_Complex_##symbol( Runtime::IExec* lhs, Runtime::IExec* rhs, const Grammar::SymbolLoc_t& loc ) \
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
class CExec_Simple_##symbol : public Runtime::IExec \
{ \
public: \
	CExec_Simple_##symbol( Runtime::IExec* expr, const Grammar::SymbolLoc_t& loc ) \
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
class CExec_Value_##symbol : public Runtime::IExec \
{ \
public: \
	CExec_Value_##symbol( const Value::CValue& value, const Grammar::SymbolLoc_t& loc ) \
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
class CExec_List_##symbol : public Runtime::IExec \
{ \
public: \
	CExec_List_##symbol( const std::vector< Runtime::IExec* >& list, const Grammar::SymbolLoc_t& loc ) \
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
class CExec_Internal_##func : public Runtime::IExec \
{ \
public: \
	Runtime::Statement_t Execute( Runtime::CContext& context ); \
};

#define RTI_EXECFN_COMPLEX( symbol ) \
Runtime::Statement_t RuntimeInternal::CExec_Complex_##symbol::Execute( Runtime::CContext& context )

#define RTI_EXECFN_SIMPLE( symbol ) \
Runtime::Statement_t RuntimeInternal::CExec_Simple_##symbol::Execute( Runtime::CContext& context )

#define RTI_EXECFN_VALUE( symbol ) \
Runtime::Statement_t RuntimeInternal::CExec_Value_##symbol::Execute( Runtime::CContext& context )

#define RTI_EXECFN_LIST( symbol ) \
Runtime::Statement_t RuntimeInternal::CExec_List_##symbol::Execute( Runtime::CContext& context )

#define RTI_EXECFN_INTERNAL( func ) \
Runtime::Statement_t RuntimeInternal::CExec_Internal_##func::Execute( Runtime::CContext& context )

#define EXEC_COMPLEX( symbol ) \
if ( expression->Symbol() == Grammar::Symbol::symbol && expression->Type() == AST::ExpressionType::ET_COMPLEX ) {\
	auto complex = new RuntimeInternal::CExec_Complex_##symbol( \
		convert( static_cast< AST::CComplexExpression* > ( expression )->Lhs() ), \
		convert( static_cast< AST::CComplexExpression* > ( expression )->Rhs() ), \
		expression->Location() ); \
	context->AddObject( complex ); \
	return complex; \
}

#define EXEC_SIMPLE( symbol ) \
if ( expression->Symbol() == Grammar::Symbol::symbol && expression->Type() == AST::ExpressionType::ET_SIMPLE ) {\
	auto simple = new RuntimeInternal::CExec_Simple_##symbol( \
		convert( static_cast< AST::CSimpleExpression* > ( expression )->Expression() ), \
		expression->Location() ); \
	context->AddObject( simple ); \
	return simple; \
}

#define EXEC_LIST( symbol ) \
if ( expression->Symbol() == Grammar::Symbol::symbol && expression->Type() == AST::ExpressionType::ET_LIST ) {\
	std::vector< IExec* > objects; \
	auto astList = static_cast< AST::CListExpression* > ( expression )->List(); \
	std::transform( astList.begin(), astList.end(), std::back_inserter( objects ), convert ); \
	auto list = new RuntimeInternal::CExec_List_##symbol( \
		objects, \
		expression->Location() ); \
	context->AddObject( list ); \
	return list; \
}

#define EXEC_VALUE( symbol ) \
if ( expression->Symbol() == Grammar::Symbol::symbol && expression->Type() == AST::ExpressionType::ET_VALUE ) { \
	auto value = new RuntimeInternal::CExec_Value_##symbol( static_cast< AST::CValueExpression* > ( expression )->Value(), expression->Location() ); \
	context->AddObject( value ); \
	return value; \
}

namespace RuntimeInternal
{
	// Implement RTI executor classes
	RTI_COMPLEX_HANDLER( S_ADD );
	RTI_COMPLEX_HANDLER( S_SUB );
	RTI_COMPLEX_HANDLER( S_DIV );
	RTI_COMPLEX_HANDLER( S_MUL );
	RTI_COMPLEX_HANDLER( S_CALL );
	RTI_COMPLEX_HANDLER( S_FUNC );
	RTI_COMPLEX_HANDLER( S_FUNCDEF );

	RTI_SIMPLE_HANDLER( S_FUNCBODY );
	RTI_SIMPLE_HANDLER( S_ADD );
	RTI_SIMPLE_HANDLER( S_SUB );

	RTI_LIST_HANDLER( S_SCOPE );
	RTI_LIST_HANDLER( S_LIST );

	RTI_VALUE_HANDLER( S_DBLCNST );
	RTI_VALUE_HANDLER( S_INTCNST );
	RTI_VALUE_HANDLER( S_NAME );

	// Debug functions
#ifdef RTI_DEBUG_ENABLED
	RTI_INTERNALFUNC_HANDLER( __setFlag );
#endif
}
