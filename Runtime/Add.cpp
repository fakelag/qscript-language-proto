#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_ADD )
{
	auto lhResult = m_LHS->Execute( context );
	auto rhResult = m_RHS->Execute( context );

	return { lhResult.m_Value + rhResult.m_Value };
}

RTI_EXECFN_SIMPLE( S_ADD )
{
	return m_Expr->Execute( context );
}
