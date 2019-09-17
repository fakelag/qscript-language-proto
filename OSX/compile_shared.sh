g++ ../Shared/Value.cpp -I ../Includes/ -Wc++11-extensions -std=c++11 -D _OSX -c -o ./lib/Value.o
g++ ../Shared/AST.cpp -I ../Includes/ -Wc++11-extensions -std=c++11 -D _OSX -c -o ./lib/AST.o
ar rvs ./Lib/Value.a ./Lib/Value.o
ar rvs ./Lib/AST.a ./Lib/AST.o
