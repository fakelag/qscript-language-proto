sh compile_lexer.sh
sh compile_parser.sh
sh compile_runtime.sh
g++ -I ../Includes/ ../UnitTests/UnitTests.cpp ../UnitTests/LexerTests.cpp ../UnitTests/ParserTests.cpp ../UnitTests/ValueTests.cpp ../UnitTests/RuntimeTests.cpp ./Lib/Lexer.a ./Lib/Parser.a ./Lib/Value.a ./Lib/AST.a ./Lib/Runtime/Rt.a -Wc++11-extensions -std=c++11 -o ./lib/Tests.o -D _OSX
