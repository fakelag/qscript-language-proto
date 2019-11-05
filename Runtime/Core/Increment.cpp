#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_INCREMENT )
{
	bool isPost = m_LHS != NULL;
	Runtime::IExec* operand = isPost ? m_LHS : m_RHS;

	context.AddFlag( Runtime::CContext::CF_NORESOLVE );
	auto varName = operand->Execute( context );
	context.RemoveFlag( Runtime::CContext::CF_NORESOLVE );

	if ( varName.m_Value.GetType() != Value::ValueType::VT_STRING )
		throw RuntimeException( m_Loc, "Invalid variable name: " + varName.m_Value.GetString() );

	auto variable = context.FindVariable( varName.m_Value.GetString() );

	if ( !variable )
		throw RuntimeException( m_Loc, "Variable \"" + varName.m_Value.GetString() + "\" is not defined" );

	auto varType = variable->GetType();
	if ( varType != Value::ValueType::VT_INTEGER && varType != Value::ValueType::VT_DOUBLE )
		throw RuntimeException( m_Loc, "Variable \"" + varName.m_Value.GetString() + "\" is not defined" );

	if ( isPost )
	{
		auto oldValue = *variable;
		*variable = *variable + Value::CValue( 1 );
		return { oldValue };
	}
	else
	{
		*variable = *variable + Value::CValue( 1 );
		return { *variable };
	}
}
