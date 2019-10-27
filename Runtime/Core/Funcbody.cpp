#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_SIMPLE( S_FUNCBODY )
{
	return m_Expr->Execute( context );
}
