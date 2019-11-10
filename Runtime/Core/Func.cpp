#include "Runtime.h"
#include "RuntimeInternal.h"

#include <algorithm>

RTI_EXECFN_COMPLEX( S_FUNC )
{
	auto def = m_LHS->Execute( context );

	if ( def.m_Value.GetType() != Value::ValueType::VT_ARRAY )
		throw RuntimeException( m_Loc, "Invalid function definition: " + def.m_Value.GetString() );

	if ( def.m_Value.ArraySize() != 2 )
		throw RuntimeException( m_Loc, "Invalid function definition: " + def.m_Value.GetString() );

	// def.m_Value[0] = the function name
	// def.m_Value[1] = a list of names for the function args

	std::vector< std::string > arguments;
	auto argsArray = def.m_Value[ 1 ].GetArray();

	std::transform( argsArray.begin(), argsArray.end(), std::back_inserter( arguments ), []( Value::CValue& arg ) {
		return arg.GetString();
	});

	context.PushFunction( def.m_Value[ 0 ].GetString(), m_RHS, arguments, &m_Loc );
	return { def.m_Value[ 0 ] };
}
