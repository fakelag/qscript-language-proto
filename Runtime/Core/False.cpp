#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_VALUE( S_FALSE )
{
	return { Value::CValue( false ) };
}
