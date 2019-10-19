#include "Runtime.h"
#include "RuntimeInternal.h"

DEF_EXECFN( S_INTCNST )
{
	Runtime::Statement_t stmt;
	stmt.m_Value = m_Expr->Value();

	return stmt;
}