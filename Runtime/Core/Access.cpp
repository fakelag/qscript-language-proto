#include "Runtime.h"
#include "RuntimeInternal.h"

#include <string>

RTI_EXECFN_COMPLEX( S_ACCESS )
{
	if ( context.m_ExecFlags & Runtime::CContext::ContextFlags::CF_NORESOLVE )
	{
		// Either in recursion or a top-level assign
		// return access info to the element
		auto elementName = m_LHS->Execute( context ).m_Value;

		// Enable resolving and retrieve the index
		context.RemoveFlag( Runtime::CContext::ContextFlags::CF_NORESOLVE );
		auto index = m_RHS->Execute( context ).m_Value;
		context.AddFlag( Runtime::CContext::ContextFlags::CF_NORESOLVE );

		// Return a structure which contain the element and the array index
		// to the desired element. With nested access expressions (such as arr[1][0][2])
		// the elementName might contain another accessInfo depending on the current
		// recursion level
		std::vector<Value::CValue> accessInfo = { elementName, index };
		return { Value::CValue( accessInfo ) };
	}
	else
	{
		// top-level (not in recursion) and we want the value
		auto element = m_LHS->Execute( context ).m_Value;
		auto index = m_RHS->Execute( context ).m_Value;
		return { element[ index.GetInt() ] };
	}
}
