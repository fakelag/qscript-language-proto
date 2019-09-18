#include <stdio.h>
#include <iostream>
#include "Lexer.h"
#include "Parser.h"
#include "Value.h"

bool GetArg( const std::string& argument, std::string& value, int argc, const char** argv )
{
	// Iterate through all arguments -- the first one is the program path
	for ( int i = 1; i < argc; ++i )
	{
		if ( argument == argv[ i ] )
		{
			value = argv[ i + 1 ];
			return true;
		}
	}

	return false;
}

int main( int argc, const char** argv )
{
	std::string target;
	if ( !GetArg( "-target", target, argc, argv ) )
	{
		std::cout << "Usage: " << argv[ 0 ] << " -target <lexer/parser/bytecode/interpreter>" << std::endl;
		return 0;
	}

	std::string command;
	for ( ;; )
	{
		std::cout << "> ";
		std::getline( std::cin, command );

		if ( command == "exit" )
			break;

		if ( target == "lexer" )
		{
			auto symbols = Lexer::Parse( command );
			std::cout << Lexer::Stringify( symbols ) << std::endl;
		}
		else if (target == "parser")
		{
			Value::CValue varName("x");
			Value::CValue value("hello world...");

			AST::CValueExpression* valueExpr = new AST::CValueExpression( value, "\"hello world...\"", { 0, 42 } );
			AST::CValueExpression* varNameExpr = new AST::CValueExpression( varName, "x", { 0, 13 } );
			AST::CComplexExpression* assignExpr = new AST::CComplexExpression( varNameExpr, valueExpr, "=", { 0, 32 } );
			AST::CComplexExpression* assignExpr2 = new AST::CComplexExpression( varNameExpr, valueExpr, "=", { 0, 46 } );
			AST::CListExpression* listExpression = new AST::CListExpression({ assignExpr, assignExpr2 }, "LIST", { 0, 1337 });
			AST::CSimpleExpression* scopeExpr = new AST::CSimpleExpression( listExpression, "SCOPE", { 0, 0 } );

			std::cout << Parser::Stringify({ scopeExpr }) << std::endl;
		}
		else
		{
			std::cout << "Unknown target: " << target << std::endl;
		}
	}

	return 0;
}
