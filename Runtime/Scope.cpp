#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN( S_SCOPE )
{
	context.PushScope( false );

	Value::CValue statementList;
	statementList.SetArray( {} );

	for ( auto node : m_List )
		statementList.ArrayPush( node->Execute( context ).m_Value );

	context.PopScope();

	return { statementList };
}
