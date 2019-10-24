#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_VALUE( S_DBLCNST )
{
	return { m_Value };
}
