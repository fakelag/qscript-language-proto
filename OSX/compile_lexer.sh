g++ -I ../Includes/ ../Lexer/Lexer.cpp -Wc++11-extensions -std=c++11 -c -o ./lib/Lexer.o
ar rvs ./Lib/Lexer.a ./Lib/Lexer.o
