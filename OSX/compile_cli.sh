sh compile_lexer.sh
sh compile_parser.sh
g++ -I ../Includes/ ../CLI/Main.cpp ./Lib/Lexer.a ./Lib/Parser.a -Wc++11-extensions -std=c++11 -o ./lib/CLI.o
