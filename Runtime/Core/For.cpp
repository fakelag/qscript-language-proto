#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_LIST( S_FOR )
{
	context.PushScope( false, false );

	for ( m_List[ 0 ]->Execute( context );
		m_List[ 1 ]->Execute( context ).m_Value.GetBool();
		m_List[ 2 ]->Execute( context ) )
	{
		m_List[ 3 ]->Execute( context );
	}

	context.PopScope();
	return { Value::CValue() };
}
