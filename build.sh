# clear
g++ -c include/ConsolePrint/ConsolePrint.cc -o cp.o
g++ -c src/implementations/fs-utils.cc -o fs-utils.o -I src
g++ -c -Iinclude -Isrc src/implementations/core.cc -o core.o
g++ -c -Iinclude -Isrc src/implementations/executer.cc -o exec.o
g++ -Iinclude -Isrc src/gdbs.cc core.o cp.o fs-utils.o exec.o -o gdbs
./gdbs sd