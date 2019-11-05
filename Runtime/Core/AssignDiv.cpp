#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_ASSIGN_DIV )
{
	context.AddFlag( Runtime::CContext::CF_NORESOLVE );
	auto varName = m_LHS->Execute( context );
	context.RemoveFlag( Runtime::CContext::CF_NORESOLVE );

	auto newValue = m_RHS->Execute( context );

	if ( varName.m_Value.GetType() != Value::ValueType::VT_STRING )
		throw RuntimeException( m_Loc, "Invalid variable name: " + varName.m_Value.GetString() );

	auto variable = context.FindVariable( varName.m_Value.GetString() );

	if ( !variable )
		throw RuntimeException( m_Loc, "Variable \"" + varName.m_Value.GetString() + "\" is not defined" );

	*variable = *variable / newValue.m_Value;
	return { *variable };
}
