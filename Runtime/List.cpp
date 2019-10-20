#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN( S_LIST )
{
	Runtime::Statement_t statement;
	statement.m_Value.SetArray( {} );

	for ( auto node : m_List )
		statement.m_Value.ArrayPush( node->Execute( context ).m_Value );

	return statement;
}
