#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_SIMPLE( S_VAR )
{
	context.AddFlag( Runtime::CContext::CF_NORESOLVE );
	auto varName = m_Expr->Execute( context );
	context.RemoveFlag( Runtime::CContext::CF_NORESOLVE );

	if ( varName.m_Value.GetType() != Value::ValueType::VT_STRING )
		throw RuntimeException( m_Loc, "Invalid variable name: " + varName.m_Value.GetString() );

	auto variableList = context.GetCurrentScope().m_Variables;

	// Does it already exist in the current scope?
	if ( variableList.find( varName.m_Value.GetString() ) != variableList.end() )
		throw RuntimeException( m_Loc, "Variable \"" + varName.m_Value.GetString() + "\" already exists in the current scope" );

	context.PushVariable( varName.m_Value.GetString(), Value::CValue(), &m_Loc );
	return { varName };
}
