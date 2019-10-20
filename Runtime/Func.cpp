#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN( S_FUNC )
{
	auto def = m_LHS->Execute( context );

	if ( def.m_Value.GetType() != Value::ValueType::VT_ARRAY )
		throw RuntimeException( m_Loc, "Invalid function definition: " + def.m_Value.GetString() );

	if ( def.m_Value.ArraySize() != 2 )
		throw RuntimeException( m_Loc, "Invalid function definition: " + def.m_Value.GetString() );

	context.PushFunction( def.m_Value[ 0 ].GetString(), m_RHS, &m_Loc );
	return { def.m_Value[ 0 ] };
}
