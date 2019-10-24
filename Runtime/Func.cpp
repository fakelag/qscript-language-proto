#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_FUNC )
{
	auto def = m_LHS->Execute( context );

	if ( def.m_Value.GetType() != Value::ValueType::VT_ARRAY )
		throw RuntimeException( m_Loc, "Invalid function definition: " + def.m_Value.GetString() );

	if ( def.m_Value.ArraySize() != 2 )
		throw RuntimeException( m_Loc, "Invalid function definition: " + def.m_Value.GetString() );

	// def.m_Value[0] = the function name
	// def.m_Value[1] = a list of names for the function args

	context.PushFunction( def.m_Value[ 0 ].GetString(), m_RHS, &m_Loc );
	return { def.m_Value[ 0 ] };
}
