#include "Runtime.h"
#include "RuntimeInternal.h"

#ifdef RTI_DEBUG_ENABLED
	RTI_EXECFN_INTERNAL( __setFlag )
	{
		// Increment the flag + return it's new value
		return { Value::CValue( ++context.m_Flag ) };
	}

	RTI_EXECFN_INTERNAL( __getLen )
	{
		// Find value passed as "arr" argument
		auto variable = context.FindVariable( "arr" );

		if ( !variable )
			throw RuntimeException( m_Loc, "Variable \"arr\" is not defined" );

		if ( variable->GetType() != Value::ValueType::VT_ARRAY )
			throw RuntimeException( m_Loc, "Variable \"arr\" is not an array" );

		// Return the length (size) of array
		return { Value::CValue( ( int ) variable->ArraySize() ) };
	}
#endif
