#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_CALL )
{
	context.AddFlag( Runtime::CContext::CF_NORESOLVE );
	auto funcName = m_LHS->Execute( context );
	context.RemoveFlag( Runtime::CContext::CF_NORESOLVE );

	if ( funcName.m_Value.GetType() != Value::ValueType::VT_STRING )
		throw RuntimeException( m_Loc, "Invalid function name: " + funcName.m_Value.GetString() );

	// Find the function to execute
	auto function = context.FindFunction( funcName.m_Value.GetString() );

	if ( !function )
		throw RuntimeException( m_Loc, "Function \"" + funcName.m_Value.GetString() + "\" is not defined" );

	if ( !function->m_Body )
		throw RuntimeException( m_Loc, "Function \"" + funcName.m_Value.GetString() + "\" is declared, but not defined" );

	auto argsList = m_RHS->Execute( context );

	if ( function->m_Args.size() != argsList.m_Value.ArraySize() )
		throw RuntimeException( m_Loc, "Invalid Function call to \"" + funcName.m_Value.GetString() + "\" (missing arguments)" );

	context.PushScope( Runtime::CContext::ScopeType::ST_ARGS );

	int argCount = argsList.m_Value.ArraySize();
	for ( int i = 0; i < argCount; ++i )
		context.PushVariable( function->m_Args[ i ], argsList.m_Value[ i ], &m_Loc );

	auto returnValue = function->m_Body->Execute( context );

	context.PopScope();

	return returnValue;
}
