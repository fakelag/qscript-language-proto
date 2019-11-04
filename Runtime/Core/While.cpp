#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_WHILE )
{
	context.PushScope( false, true );

	while ( m_LHS->Execute( context ).m_Value.GetBool() )
		m_RHS->Execute( context );

	context.PopScope();
	return { Value::CValue() };
}
