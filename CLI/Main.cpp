#include <stdio.h>
#include <iostream>
#include "Lexer.h"
#include "Parser.h"
#include "Value.h"
#include "Exception.h"

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
		else if ( target == "parser" )
		{
			try {
				auto symbols = Lexer::Parse( command );
				auto syntaxTree = Parser::Parse( symbols );

				std::cout << Parser::Stringify( syntaxTree ) << std::endl;
			}
			catch ( const ParseException& exception )
			{
				std::cout << exception.what() << std::endl;

				auto errors = exception.errors();
				auto locations = exception.locations();
				for ( size_t i = 0; i < errors.size(); ++i )
				{
					std::cout << errors[ i ] << " at line " << locations[ i ].m_LineNr
						<< " col " << locations[ i ].m_ColNr << std::endl;
				}
			}
		}
		else
		{
			std::cout << "Unknown target: " << target << std::endl;
		}
	}

	return 0;
}
