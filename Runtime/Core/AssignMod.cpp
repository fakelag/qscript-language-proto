#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_ASSIGN_MOD )
{
	context.AddFlag( Runtime::CContext::CF_NORESOLVE );
	auto varName = m_LHS->Execute( context );
	context.RemoveFlag( Runtime::CContext::CF_NORESOLVE );

	auto newValue = m_RHS->Execute( context );

	if ( varName.m_Value.GetType() != Value::ValueType::VT_STRING )
		throw RuntimeException( m_Loc, "Invalid variable name: " + varName.m_Value.GetString() );

	switch ( varName.m_Value.GetType() )
	{
		case Value::ValueType::VT_STRING:
		{
			auto variable = context.FindVariable( varName.m_Value.GetString() );

			if ( !variable )
				throw RuntimeException( m_Loc, "Variable \"" + varName.m_Value.GetString() + "\" is not defined" );

			*variable = ( *variable ) % newValue.m_Value;
			return { *variable };
		}
		case Value::ValueType::VT_ARRAY:
		{
			auto variable = &varName.m_Value[ 0 ];

			if ( !variable )
				throw RuntimeException( m_Loc, "Variable \"" + varName.m_Value.GetString() + "\" is not defined" );

			auto& ref = ( *variable )[ varName.m_Value[ 1 ].GetInt() ];
			ref = ref % newValue.m_Value;

			return { ref };
		}
		default:
			throw RuntimeException( m_Loc, "Invalid variable reference: " + varName.m_Value.GetString() );
	}
}
