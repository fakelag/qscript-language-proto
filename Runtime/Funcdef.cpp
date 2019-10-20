#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN( S_FUNCDEF )
{
	auto definition = Value::CValue( { m_LHS->Execute( context ).m_Value, m_RHS->Execute( context ).m_Value } );
	return { definition };
}
