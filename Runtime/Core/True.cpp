#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_VALUE( S_TRUE )
{
	return { Value::CValue( true ) };
}
