sh compile_shared.sh
g++ -I ../Includes/ ../Parser/Parser.cpp -Wc++11-extensions -std=c++11 -c -o ./lib/Parser.o -D _OSX
ar rvs ./Lib/Parser.a ./Lib/Parser.o
