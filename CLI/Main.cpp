#include <stdio.h>
#include "Lexer.h"

int main( int argc, const char** argv )
{
	Lexer::Parse( "4 + 2* 4+ 5;" );
	return 0;
}
