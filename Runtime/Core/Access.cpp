#include "Runtime.h"
#include "RuntimeInternal.h"

#include <string>

RTI_EXECFN_COMPLEX( S_ACCESS )
{
	bool resolveNames = !( context.m_ExecFlags & Runtime::CContext::ContextFlags::CF_NORESOLVE );

	context.RemoveFlag( Runtime::CContext::ContextFlags::CF_NORESOLVE );
	auto element = m_LHS->Execute( context ).m_Value;
	auto index = m_RHS->Execute( context ).m_Value;

	if ( !resolveNames )
		context.AddFlag( Runtime::CContext::ContextFlags::CF_NORESOLVE );

	switch ( element.GetType() )
	{
		case Value::ValueType::VT_STRING:
		{
			if ( resolveNames )
				throw RuntimeException( m_Loc, "Invalid string access" );

			auto variable = context.FindVariable( element.GetString() );

			if ( !variable )
				throw RuntimeException( m_Loc, "Variable \"" + element.GetString() + "\" is not defined" );

			return { Value::CValue( std::to_string( variable->GetString().at( index ) ) ) };
		}
		case Value::ValueType::VT_ARRAY:
		{
			if ( resolveNames )
			{
				return { element[ index.GetInt() ] };
			}
			else
			{
				std::vector<Value::CValue> accessInfo = { element, index };
				return { Value::CValue( accessInfo ) };
			}
		}
		default:
			throw RuntimeException( m_Loc, "Invalid variable reference: " + element.GetString() );
	}

	/*if ( context.m_ExecFlags & Runtime::CContext::ContextFlags::CF_NORESOLVE )
	{
		auto name = m_LHS->Execute( context ).m_Value;

		context.RemoveFlag( Runtime::CContext::ContextFlags::CF_NORESOLVE );
		auto index = m_RHS->Execute( context ).m_Value;
		context.AddFlag( Runtime::CContext::ContextFlags::CF_NORESOLVE );

		if ( name.GetType() != Value::ValueType::VT_STRING )
			throw RuntimeException( m_Loc, "Invalid variable reference: " + name.GetString() );

		if ( index.GetType() != Value::ValueType::VT_INTEGER )
			throw RuntimeException( m_Loc, "Invalid array index: " + index.GetString() );

		std::vector<Value::CValue> accessInfo = { name, index };
		return { Value::CValue( accessInfo ) };
	}
	else
	{
		context.AddFlag( Runtime::CContext::ContextFlags::CF_NORESOLVE );
		auto name = m_LHS->Execute( context ).m_Value;
		context.RemoveFlag( Runtime::CContext::ContextFlags::CF_NORESOLVE );

		auto index = m_RHS->Execute( context ).m_Value;

		if ( index.GetType() != Value::ValueType::VT_INTEGER )
			throw RuntimeException( m_Loc, "Invalid array index: " + index.GetString() );

		switch ( name.GetType() )
		{
			case Value::ValueType::VT_STRING:
			{
				auto variable = context.FindVariable( name.GetString() );

				if ( !variable )
					throw RuntimeException( m_Loc, "Variable \"" + name.GetString() + "\" is not defined" );

				return { ( *variable )[ index.GetInt() ] };
			}
			case Value::ValueType::VT_ARRAY:
			{
				// name = accessInfo
				auto variable = context.FindVariable( name[ 0 ].GetString() );

				if ( !variable )
					throw RuntimeException( m_Loc, "Variable \"" + name.GetString() + "\" is not defined" );

				return { ( ( *variable )[ name[ 1 ].GetInt() ] )[ index.GetInt() ] };
			}
			default:
				throw RuntimeException( m_Loc, "Invalid variable reference: " + name.GetString() );
		}
	}*/
}
