#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_SIMPLE( S_LOGIC_NOT )
{
	auto lhResult = m_Expr->Execute( context );
	return { !lhResult.m_Value };
}
