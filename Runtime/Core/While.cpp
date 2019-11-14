#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_LIST( S_WHILE )
{
	context.PushScope( Runtime::CContext::ScopeType::ST_LOOP );

	auto result = m_List[ 1 ] ? m_List[ 1 ]->Execute( context ).m_Value : Value::CValue();
	while ( bool ( m_List[ 0 ]->Execute( context ).m_Value ) )
	{
		auto currentResult = m_List[ 2 ]->Execute( context ).m_Value;

		if ( context.GetCurrentScope().m_IsBreaking )
		{
			result = currentResult;
			break;
		}
	}

	context.PopScope();
	return { result };
}
