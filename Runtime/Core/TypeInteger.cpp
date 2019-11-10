#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_VALUE( S_TYPEINTEGER )
{
	return { m_Value };
}
