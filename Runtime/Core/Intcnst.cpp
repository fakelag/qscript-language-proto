#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_VALUE( S_INTCNST )
{
	return { m_Value };
}
