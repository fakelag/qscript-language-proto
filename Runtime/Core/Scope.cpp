#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_LIST( S_SCOPE )
{
	// Push a new scope to the stack
	context.PushScope( false, false );

	Value::CValue result;
	for ( auto node : m_List )
	{
		// Execute everything inside the scope
		result = node->Execute( context ).m_Value;
	}

	// Pop the scope -- we are exiting
	context.PopScope();

	// Return the result of the last executed node
	return { result };
}
