#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_INCREMENT )
{
	bool isPost = m_LHS != NULL;
	Runtime::IExec* operand = isPost ? m_LHS : m_RHS;

	Value::CValue* variable = RuntimeInternal::ResolveAssignable( m_Loc, operand, context );

	auto varType = variable->GetType();
	if ( varType != Value::ValueType::VT_INTEGER && varType != Value::ValueType::VT_DOUBLE )
		throw RuntimeException( m_Loc, "Variable is not a decimal or int" );

	if ( isPost )
	{
		auto oldValue = *variable;
		*variable = ( *variable ) + Value::CValue( 1 );
		return { oldValue };
	}
	else
	{
		*variable = ( *variable ) + Value::CValue( 1 );
		return { *variable };
	}
}
