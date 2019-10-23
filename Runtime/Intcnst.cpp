#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN( S_INTCNST )
{
	return { m_Value };
}
