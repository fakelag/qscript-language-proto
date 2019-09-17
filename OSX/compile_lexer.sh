sh compile_shared.sh
g++ ../Lexer/Lexer.cpp -I ../Includes/ -Wc++11-extensions -std=c++11 -c -o ./lib/Lexer.o
ar rvs ./Lib/Lexer.a ./Lib/Lexer.o
