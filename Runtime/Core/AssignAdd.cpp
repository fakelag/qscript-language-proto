#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_ASSIGN_ADD )
{
	context.AddFlag( Runtime::CContext::CF_NORESOLVE );
	auto varName = m_LHS->Execute( context );
	context.RemoveFlag( Runtime::CContext::CF_NORESOLVE );

	auto newValue = m_RHS->Execute( context );

	switch ( varName.m_Value.GetType() )
	{
		case Value::ValueType::VT_STRING:
		{
			auto variable = context.FindVariable( varName.m_Value.GetString() );

			if ( !variable )
				throw RuntimeException( m_Loc, "Variable \"" + varName.m_Value.GetString() + "\" is not defined" );

			*variable = ( *variable ) + newValue.m_Value;
			return { *variable };
		}
		case Value::ValueType::VT_ARRAY:
		{
			auto& varRef = varName.m_Value;

			std::vector< int > indexes;
			while ( varRef.GetType() != Value::ValueType::VT_STRING )
			{
				indexes.push_back( varRef[ 1 ].GetInt() );
				varRef = varRef[ 0 ];
			}

			auto variable = context.FindVariable( varRef.GetString() );

			if ( !variable )
				throw RuntimeException( m_Loc, "Variable \"" + varName.m_Value.GetString() + "\" is not defined" );

			auto varValue = variable;
			for ( int i = indexes.size() - 1; i >= 0; --i )
				varValue = &((*varValue)[ indexes[ i ] ]);

			( *varValue ) = ( *varValue ) + newValue.m_Value;
			return { *varValue };
		}
		default:
			throw RuntimeException( m_Loc, "Invalid variable reference: " + varName.m_Value.GetString() );
	}
}
