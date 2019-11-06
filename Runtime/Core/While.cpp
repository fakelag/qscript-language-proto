#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_WHILE )
{
	context.PushScope( Runtime::CContext::ScopeType::ST_LOOP );

	auto result = Value::CValue();
	while ( bool ( m_LHS->Execute( context ).m_Value ) )
	{
		result = m_RHS->Execute( context ).m_Value;

		if ( context.GetCurrentScope().m_IsBreaking )
			break;
	}

	context.PopScope();
	return { result };
}
