#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_MORETHAN )
{
	return { m_LHS->Execute( context ).m_Value > m_RHS->Execute( context ).m_Value };
}
