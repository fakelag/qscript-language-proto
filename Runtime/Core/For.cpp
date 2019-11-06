#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_LIST( S_FOR )
{
	context.PushScope( Runtime::CContext::ScopeType::ST_LOOP );

	if ( m_List[ 0 ] )
		m_List[ 0 ]->Execute( context );

	auto result = Value::CValue();
	for (; m_List[ 1 ] ? bool ( m_List[ 1 ]->Execute( context ).m_Value ) : true; )
	{
		result = m_List[ 3 ]->Execute( context ).m_Value;

		if ( context.GetCurrentScope().m_IsBreaking )
			break;

		if ( m_List[ 2 ] )
			m_List[ 2 ]->Execute( context );
	}

	context.PopScope();
	return { result };
}
