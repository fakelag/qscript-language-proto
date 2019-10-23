#include "Runtime.h"
#include "RuntimeInternal.h"

#ifdef RTI_DEBUG_ENABLED
	RTI_INTERNALFN( __setFlag )
	{
		return { Value::CValue( ++context.m_Flag ) };
	}
#endif
