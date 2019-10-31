#include "Runtime.h"
#include "RuntimeInternal.h"

#include <math.h>

RTI_EXECFN_COMPLEX( S_POW )
{
	auto lhResult = m_LHS->Execute( context ).m_Value;
	auto rhResult = m_RHS->Execute( context ).m_Value;

	bool isDouble = false;
	switch ( lhResult.GetType() )
	{
	case Value::ValueType::VT_DOUBLE:
		isDouble = true;
	case Value::ValueType::VT_INTEGER:
	{
		switch ( rhResult.GetType() )
		{
		case Value::ValueType::VT_DOUBLE:
			isDouble = true;
		case Value::ValueType::VT_INTEGER:
		{
			if ( isDouble )
				return { Value::CValue( pow( lhResult.GetDouble(), rhResult.GetDouble() ) ) };
			else
				return { Value::CValue( pow( lhResult.GetInt(), rhResult.GetInt() ) ) };
		}
		default:
			throw RuntimeException( m_Loc, "Invalid value for right hand expression" );
		}
	}
	default:
		throw RuntimeException( m_Loc, "Invalid value for left hand expression" );
	}
}
