sh compile_shared.sh

cd ./Lib/Runtime/
g++ ../../../Runtime/*.cpp -I ../../../Includes/ -Wc++11-extensions -std=c++11 -c -D _OSX -D RTI_DEBUG_ENABLED
ar rvs ./Rt.a *.o

cd ../..
