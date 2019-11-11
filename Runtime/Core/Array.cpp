#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_LIST( S_ARRAY )
{
	Value::CValue array;

	array.SetArray({});
	for ( auto node : m_List )
		array.ArrayPush( node->Execute( context ).m_Value );

	return { array };
}
