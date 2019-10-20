#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN( S_FUNCBODY )
{
	return m_Expr->Execute( context );
}
