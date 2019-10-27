#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_ASSIGN )
{
	context.AddFlag( Runtime::CContext::CF_NORESOLVE );
	auto varName = m_LHS->Execute( context );
	context.RemoveFlag( Runtime::CContext::CF_NORESOLVE );

	auto newValue = m_RHS->Execute( context );

	if ( varName.m_Value.GetType() != Value::ValueType::VT_STRING )
		throw RuntimeException( m_Loc, "Invalid variable name: " + varName.m_Value.GetString() );

	Value::CValue variable;
	if ( !context.FindVariable( varName.m_Value.GetString(), &variable ) )
		throw RuntimeException( m_Loc, "Variable \"" + varName.m_Value.GetString() + "\" is not defined" );

	if ( !context.SetVariable( varName.m_Value.GetString(), newValue.m_Value ) )
		throw RuntimeException( m_Loc, "Error assigning value (name=" + varName.m_Value.GetString() + " value=" + newValue.m_Value.GetString() + ")" );

	return newValue;
}
