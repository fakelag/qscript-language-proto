#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_DECREMENT )
{
	bool isPost = m_LHS != NULL;
	Runtime::IExec* operand = isPost ? m_LHS : m_RHS;

	context.AddFlag( Runtime::CContext::CF_NORESOLVE );
	auto varName = operand->Execute( context );
	context.RemoveFlag( Runtime::CContext::CF_NORESOLVE );

	Value::CValue* targetVariable = NULL;

	switch ( varName.m_Value.GetType() )
	{
		case Value::ValueType::VT_STRING:
		{
			auto variable = context.FindVariable( varName.m_Value.GetString() );

			if ( !variable )
				throw RuntimeException( m_Loc, "Variable \"" + varName.m_Value.GetString() + "\" is not defined" );

			auto varType = variable->GetType();
			if ( varType != Value::ValueType::VT_INTEGER && varType != Value::ValueType::VT_DOUBLE )
				throw RuntimeException( m_Loc, "Variable \"" + varName.m_Value.GetString() + "\" is not defined" );

			targetVariable = variable;
			break;
		}
		case Value::ValueType::VT_ARRAY:
		{
			auto varPtr = &varName.m_Value;

			std::vector< int > indexes;
			while ( varPtr->GetType() != Value::ValueType::VT_STRING )
			{
				indexes.push_back( (*varPtr)[ 1 ].GetInt() );
				varPtr = &(*varPtr)[ 0 ];
			}

			auto variable = context.FindVariable( varPtr->GetString() );

			if ( !variable )
				throw RuntimeException( m_Loc, "Variable \"" + varName.m_Value.GetString() + "\" is not defined" );

			auto varValue = variable;
			for ( int i = indexes.size() - 1; i >= 0; --i )
				varValue = &((*varValue)[ indexes[ i ] ]);

			targetVariable = varValue;
			break;
		}
		default:
			throw RuntimeException( m_Loc, "Invalid variable reference: " + varName.m_Value.GetString() );
	}

	if ( isPost )
	{
		auto oldValue = *targetVariable;
		*targetVariable = *targetVariable - Value::CValue( 1 );
		return { oldValue };
	}
	else
	{
		*targetVariable = *targetVariable - Value::CValue( 1 );
		return { *targetVariable };
	}
}