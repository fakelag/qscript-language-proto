#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_MORE_OR_EQUALS )
{
	return { m_LHS->Execute( context ).m_Value >= m_RHS->Execute( context ).m_Value };
}
