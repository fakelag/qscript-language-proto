#include "Runtime.h"
#include "RuntimeInternal.h"

#ifdef RTI_DEBUG_ENABLED
	RTI_EXECFN_INTERNAL( __setFlag )
	{
		// Increment the flag + return it's new value
		return { Value::CValue( ++context.m_Flag ) };
	}
#endif
