#include "AST.h"

#include "Grammar.h"
#include "Parser.h"
#include "Value.h"
#include "Exception.h"

namespace AST
{
	std::vector< std::vector< IExpression* > > allocationTrack;

	void PushTrackAlloc()
	{
		allocationTrack.push_back( {} );
	}

	void PopAllocTracking()
	{
		allocationTrack.pop_back();
	}

	void ExpressionAllocated( IExpression* expr )
	{
		for ( size_t i = 0; i < allocationTrack.size(); ++i )
			allocationTrack[ i ].push_back( expr );
	}

	void ExpressionFreed( void* expr )
	{
		for ( size_t i = 0; i < allocationTrack.size(); ++i )
		{
			auto position = std::find( allocationTrack[ i ].begin(), allocationTrack[ i ].end(), expr );

			if ( position != allocationTrack[ i ].end() )
				allocationTrack[ i ].erase( position );
		}
	}

	const std::vector< IExpression* >& AllocatedExpressions()
	{
		if ( allocationTrack.size() > 0 )
			return allocationTrack[ allocationTrack.size() - 1 ];
		else
			throw new Exception( "No tracked expressions" );
	}

	CComplexExpression::CComplexExpression( IExpression* lhs, IExpression* rhs, Grammar::Symbol symbol, const Grammar::SymbolLoc_t& loc )
	{
		m_LHS = lhs;
		m_RHS = rhs;
		m_Loc = loc;
		m_Symbol = symbol;
	}

	void* CComplexExpression::operator new ( size_t size )
	{
		auto p = ::new CComplexExpression();
		ExpressionAllocated(p);
		return p;
	}

	void CComplexExpression::operator delete ( void* p )
	{
		ExpressionFreed( p );
		free( p );
	}

	std::string CComplexExpression::ToString( int indent ) const
	{
		std::string token = "UNKNOWN";
		auto commonSymbol = Grammar::LanguageSymbols.find( m_Symbol );

		if ( commonSymbol != Grammar::LanguageSymbols.end() )
			token = commonSymbol->second.m_Token;

		std::string output = std::string( indent - 1 < 0 ? indent : indent - 1, '\t' ) + "{\n";
		output += std::string( indent, '\t' ) + "type: COMPLEX\n";
		output += std::string( indent, '\t' ) + "token: " + token + "\n";
		output += std::string( indent, '\t' ) + "lhs: " + ( m_LHS ? "\n" + m_LHS->ToString( indent + 1 ) : "NULL\n" );
		output += std::string( indent, '\t' ) + "rhs: " + ( m_RHS ? "\n" + m_RHS->ToString( indent + 1 ) : "NULL\n" );
		output += std::string( indent - 1 < 0 ? indent : indent - 1, '\t' ) + "}\n";
		return output;
	}

	CSimpleExpression::CSimpleExpression( IExpression* expression, Grammar::Symbol symbol, const Grammar::SymbolLoc_t& loc )
	{
		m_Expression = expression;
		m_Loc = loc;
		m_Symbol = symbol;
	}

	void* CSimpleExpression::operator new ( size_t size )
	{
		auto p = ::new CSimpleExpression();
		ExpressionAllocated( p );
		return p;
	}

	void CSimpleExpression::operator delete ( void* p )
	{
		ExpressionFreed( p );
		free( p );
	}

	std::string CSimpleExpression::ToString( int indent ) const
	{
		std::string token = "UNKNOWN";
		auto commonSymbol = Grammar::LanguageSymbols.find( m_Symbol );

		if ( commonSymbol != Grammar::LanguageSymbols.end() )
			token = commonSymbol->second.m_Token;

		std::string output = std::string( indent - 1 < 0 ? indent : indent - 1, '\t' ) + "{\n";
		output += std::string( indent, '\t' ) + "type: SIMPLE\n";
		output += std::string( indent, '\t' ) + "token: " + token + "\n";
		output += std::string( indent, '\t' ) + "expression: " + ( m_Expression ? "\n" + m_Expression->ToString( indent + 1 ) : "NULL\n" );
		output += std::string( indent - 1 < 0 ? indent : indent - 1, '\t' ) + "}\n";
		return output;
	}

	CValueExpression::CValueExpression( const Value::CValue& value, Grammar::Symbol symbol, const Grammar::SymbolLoc_t& loc )
	{
		m_Value = Value::CValue( value );
		m_Loc = loc;
		m_Symbol = symbol;
	}

	void* CValueExpression::operator new ( size_t size )
	{
		auto p = ::new CValueExpression();
		ExpressionAllocated( p );
		return p;
	}

	void CValueExpression::operator delete ( void* p )
	{
		ExpressionFreed( p );
		free( p );
	}

	std::string CValueExpression::ToString( int indent ) const
	{
		static const char* s_ValueTypes[] ={
			"STRING",
			"INT",
			"DOUBLE"
		};

		std::string output = std::string( indent - 1 < 0 ? indent : indent - 1, '\t' ) + "{\n";
		output += std::string( indent, '\t' ) + "type: VALUE\n";
		output += std::string( indent, '\t' ) + "value: " + const_cast< Value::CValue& >( m_Value ).GetString() + "\n";
		output += std::string( indent, '\t' ) + "valueType: " + std::string( s_ValueTypes[ m_Value.GetType() ] ) + "\n";
		output += std::string( indent - 1 < 0 ? indent : indent - 1, '\t' ) + "}\n";
		return output;
	}

	CListExpression::CListExpression( const std::vector< IExpression* >& list, Grammar::Symbol symbol, const Grammar::SymbolLoc_t& loc )
	{
		m_List = list;
		m_Loc = loc;
		m_Symbol = symbol;
	}

	void* CListExpression::operator new ( size_t size )
	{
		auto p = ::new CListExpression();
		ExpressionAllocated( p );
		return p;
	}

	void CListExpression::operator delete ( void* p )
	{
		ExpressionFreed( p );
		free( p );
	}

	std::string CListExpression::ToString( int indent ) const
	{
		std::string output = std::string( indent - 1 < 0 ? indent : indent - 1, '\t' ) + "{\n";
		output += std::string( indent, '\t' ) + "type: LIST\n";
		output += std::string( indent, '\t' ) + "length: " + std::to_string( m_List.size() ) + "\n";
		output += std::string( indent, '\t' ) + "items: [\n";

		for ( auto expression : m_List )
			output += expression ? expression->ToString( indent + 1 ) : "NULL\n";

		output += std::string( indent - 1 < 0 ? indent : indent - 1, '\t' ) + "]\n" + "}\n";
		return output;
	}

	void FreeNode( IExpression* expression )
	{
		switch ( expression->Type() )
		{
			case ET_LIST:
			{
				auto list = static_cast< CListExpression* >( expression )->List();
				for ( auto expr : list )
				{
					if ( expr )
						FreeNode( expr );
				}

				break;
			}
			case ET_COMPLEX:
			{
				if ( static_cast< CComplexExpression* >( expression )->Lhs() )
					FreeNode( static_cast< CComplexExpression* >( expression )->Lhs() );
				if ( static_cast< CComplexExpression* >( expression )->Rhs() )
					FreeNode( static_cast< CComplexExpression* >( expression )->Rhs() );
				break;
			}
			case ET_SIMPLE:
			{
				if ( static_cast< CSimpleExpression* >( expression )->Expression() )
					FreeNode( static_cast< CSimpleExpression* >( expression )->Expression() );
				break;
			}
			case ET_VALUE:
				break;
			default:
				throw Exception( "Unknown expression type: " + std::to_string( expression->Type() ) );
		}

		delete expression;
	}

	void FreeTree( const std::vector< IExpression* > tree )
	{
		for ( auto expr : tree )
		{
			FreeNode( expr );
		}
	}
}
