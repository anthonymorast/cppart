all: data_handler.o anova.o bestsplit.o xval.o cptable.o print_tree.o partition.o cppart.o cppart

data_handler.o: data_handler.cpp
	x86_64-w64-mingw32-g++ --std=c++11 -c data_handler.cpp
anova.o: anova.cpp
	x86_64-w64-mingw32-g++ --std=c++11 -c anova.cpp
bestsplit.o: bestsplit.cpp
	x86_64-w64-mingw32-g++ --std=c++11 -c bestsplit.cpp
xval.o: xval.cpp
	x86_64-w64-mingw32-g++ --std=c++11 -c xval.cpp
cptable.o: cptable.cpp
	x86_64-w64-mingw32-g++ --std=c++11 -c cptable.cpp
print_tree.o: print_tree.cpp
	x86_64-w64-mingw32-g++ --std=c++11 -c print_tree.cpp
partition.o: partition.cpp
	x86_64-w64-mingw32-g++ --std=c++11 -c partition.cpp
cppart.o: cppart.cpp 
	x86_64-w64-mingw32-g++ --std=c++11 -c cppart.cpp
cppart: data_handler.o anova.o bestsplit.o xval.o cptable.o print_tree.o partition.o cppart.o
	x86_64-w64-mingw32-g++ --std=c++11 data_handler.o anova.o bestsplit.o xval.o cptable.o print_tree.o partition.o cppart.o -o cppart.exe

clean:
	rm *.o cppart.exe data/*.tree data/*.delayed
