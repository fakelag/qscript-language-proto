#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_VALUE( S_TYPEBOOLEAN )
{
	return { m_Value };
}
