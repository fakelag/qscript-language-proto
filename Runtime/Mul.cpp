#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_MUL )
{
	auto lhResult = m_LHS->Execute( context );
	auto rhResult = m_RHS->Execute( context );

	return { lhResult.m_Value * rhResult.m_Value };
}
