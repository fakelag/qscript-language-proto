#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_WHILE )
{
	context.PushScope( false, true );

	while ( bool ( m_LHS->Execute( context ).m_Value ) )
		m_RHS->Execute( context );

	context.PopScope();
	return { Value::CValue() };
}
