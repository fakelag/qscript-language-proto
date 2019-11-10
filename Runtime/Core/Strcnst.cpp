#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_VALUE( S_STRCNST )
{
	return { m_Value };
}
