#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_CALL )
{
	auto funcName = m_LHS->Execute( context );

	if ( funcName.m_Value.GetType() != Value::ValueType::VT_STRING )
		throw RuntimeException( m_Loc, "Invalid function name: " + funcName.m_Value.GetString() );

	// Find the function to execute
	auto function = context.FindFunction( funcName.m_Value.GetString() );

	if ( function == NULL )
		throw RuntimeException( m_Loc, "Function \"" + funcName.m_Value.GetString() + "\" is not defined" );

	// TODO: Check + Push args
	return function->Execute( context );
}
