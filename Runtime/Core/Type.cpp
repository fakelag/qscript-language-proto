#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_SIMPLE( S_TYPE )
{
	auto result = m_Expr->Execute( context ).m_Value;
	return { Value::CValue( ( int ) result.GetType() ) };
}
