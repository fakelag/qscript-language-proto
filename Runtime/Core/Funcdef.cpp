#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_FUNCDEF )
{
	context.AddFlag( Runtime::CContext::CF_NORESOLVE );
	std::vector< Value::CValue > list = { m_LHS->Execute( context ).m_Value, m_RHS->Execute( context ).m_Value };
	context.RemoveFlag( Runtime::CContext::CF_NORESOLVE );

	return { Value::CValue( list ) };
}
