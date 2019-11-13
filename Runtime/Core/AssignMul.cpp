#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_COMPLEX( S_ASSIGN_MUL )
{
	auto newValue = m_RHS->Execute( context );

	auto variable = RuntimeInternal::ResolveAssignable( m_Loc, m_LHS, context );
	( *variable ) = ( *variable ) * newValue.m_Value;

	return { *variable };
}
