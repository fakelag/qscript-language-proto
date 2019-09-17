g++ -I ../Includes/ ../Parser/Parser.cpp -Wc++11-extensions -std=c++11 -c -o ./lib/Parser.o
ar rvs ./Lib/Parser.a ./Lib/Parser.o
