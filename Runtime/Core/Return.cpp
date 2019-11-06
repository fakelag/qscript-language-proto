#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_SIMPLE( S_RETURN )
{
	auto result = m_Expr ? m_Expr->Execute( context ).m_Value : Value::CValue();

	context.BreakScope( true, &m_Loc );

	return { result };
}
