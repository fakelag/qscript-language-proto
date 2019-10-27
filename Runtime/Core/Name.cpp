#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_VALUE( S_NAME )
{
	if ( context.m_ExecFlags & Runtime::CContext::CF_NORESOLVE )
		return { m_Value };

	if ( m_Value.GetType() != Value::ValueType::VT_STRING )
		throw RuntimeException( m_Loc, "Invalid variable name: " + m_Value.GetString() );

	Value::CValue variable;
	if ( !context.FindVariable( m_Value.GetString(), &variable ) )
		throw RuntimeException( m_Loc, "Variable \"" + m_Value.GetString() + "\" is not defined" );

	return { variable };
}
