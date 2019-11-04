#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_IF )
{
	auto lhResult = m_LHS->Execute( context ).m_Value;

	if ( lhResult )
		return m_RHS->Execute( context );

	return { Value::CValue() };
}
