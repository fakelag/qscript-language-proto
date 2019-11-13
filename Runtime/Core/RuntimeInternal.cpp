#include "Runtime.h"
#include "RuntimeInternal.h"

namespace RuntimeInternal
{
	Value::CValue* ResolveAssignable( const Grammar::SymbolLoc_t& location, Runtime::IExec* node, Runtime::CContext& context )
	{
		// Disable resolving and retrieve variable name (or accessInfo for arrays)
		context.AddFlag( Runtime::CContext::CF_NORESOLVE );
		auto varName = node->Execute( context );
		context.RemoveFlag( Runtime::CContext::CF_NORESOLVE );

		switch ( varName.m_Value.GetType() )
		{
			case Value::ValueType::VT_STRING:
			{
				// It's a direct name reference, find the variable and return a pointer to it
				auto variable = context.FindVariable( varName.m_Value.GetString() );

				if ( !variable )
					throw RuntimeException( location, "Variable \"" + varName.m_Value.GetString() + "\" is not defined" );

				return variable;
			}
			case Value::ValueType::VT_ARRAY:
			{
				// It's an array (accessInfo), traverse it to find
				// 1) The root variable
				// 2) All the dereferencing indices
				// 3) The final CValue, which is the target of the chain
				auto varPtr = &varName.m_Value;

				std::vector< int > indexes;
				while ( varPtr->GetType() != Value::ValueType::VT_STRING )
				{
					// While the current varPtr holds an accessInfo,
					// push it's index to the stack and check the next one
					indexes.push_back( (*varPtr)[ 1 ].GetInt() );
					varPtr = &(*varPtr)[ 0 ];
				}

				// varPtr should be the root variable name now.
				// Find it from current context
				auto variable = context.FindVariable( varPtr->GetString() );

				if ( !variable )
					throw RuntimeException( location, "Variable \"" + varName.m_Value.GetString() + "\" is not defined" );

				auto varValue = variable;

				// Traverse the index stack backwards to dereference
				// the access chain in the correct order. Once the index stack
				// is depleted, target CValue should be at varValue
				for ( int i = indexes.size() - 1; i >= 0; --i )
					varValue = &((*varValue)[ indexes[ i ] ]);

				// Return it
				return varValue;
			}
			default:
				throw RuntimeException( location, "Invalid variable reference: " + varName.m_Value.GetString() );
		}
	}
}
