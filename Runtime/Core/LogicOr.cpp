#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_LOGIC_OR )
{
	auto lhResult = m_LHS->Execute( context );

	if ( bool ( lhResult.m_Value ) )
		return lhResult;

	auto rhResult = m_RHS->Execute( context );

	if ( bool ( rhResult.m_Value ) )
		return rhResult;

	return { Value::CValue( false ) };
}
