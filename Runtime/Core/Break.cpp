#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_SIMPLE( S_BREAK )
{
	// Allow a return value from break <expression>;
	auto result = m_Expr ? m_Expr->Execute( context ).m_Value : Value::CValue();

	context.BreakScope( false, &m_Loc );

	return { result };
}
