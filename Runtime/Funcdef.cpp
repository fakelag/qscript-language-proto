#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN( S_FUNCDEF )
{
	std::vector< Value::CValue > list = { m_LHS->Execute( context ).m_Value, m_RHS->Execute( context ).m_Value };
	auto definition = Value::CValue( list );
	return { definition };
}
