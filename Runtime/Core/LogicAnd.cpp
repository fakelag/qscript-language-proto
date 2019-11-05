#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_LOGIC_AND )
{
	auto lhResult = m_LHS->Execute( context );

	if ( !lhResult.m_Value )
		return { Value::CValue( false ) };

	auto rhResult = m_RHS->Execute( context );

	if ( !rhResult.m_Value )
		return { Value::CValue( false ) };

	return rhResult;
}
