#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_EQUALS )
{
	auto lhValue = m_LHS->Execute( context ).m_Value;
	auto rhValue = m_RHS->Execute( context ).m_Value;

	return { lhValue == rhValue };
}
