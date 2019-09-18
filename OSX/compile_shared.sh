g++ ../Shared/Value.cpp -I ../Includes/ -Wc++11-extensions -std=c++11 -D _OSX -c -o ./lib/Value.o -D _OSX
g++ ../Shared/AST.cpp -I ../Includes/ -Wc++11-extensions -std=c++11 -D _OSX -c -o ./lib/AST.o -D _OSX
ar rvs ./Lib/Value.a ./Lib/Value.o
ar rvs ./Lib/AST.a ./Lib/AST.o
