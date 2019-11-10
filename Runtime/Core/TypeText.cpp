#include "Runtime.h"
#include "RuntimeInternal.h"

RTI_EXECFN_SIMPLE( S_TYPETEXT )
{
	auto result = m_Expr->Execute( context ).m_Value;
	switch ( result.GetType() )
	{
	case Value::ValueType::VT_STRING: return { Value::CValue( "string" ) };
	case Value::ValueType::VT_INTEGER: return { Value::CValue( "int" ) };
	case Value::ValueType::VT_DOUBLE: return { Value::CValue( "decimal" ) };
	case Value::ValueType::VT_BOOLEAN: return { Value::CValue( "bool" ) };
	case Value::ValueType::VT_ARRAY: return { Value::CValue( "array" ) };
	case Value::ValueType::VT_UNINITIALIZED: return { Value::CValue( "none" ) };
	default:
		throw RuntimeException( m_Loc, "Unknown resource type: " + std::to_string( result.GetType() ) );
	}
}
