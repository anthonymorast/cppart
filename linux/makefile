all: data_handler.o anova.o bestsplit.o xval.o cptable.o print_tree.o partition.o cppart.o cppart

data_handler.o: data_handler.cpp
	g++ --std=c++11 -c data_handler.cpp
anova.o: anova.cpp
	g++ --std=c++11 -c anova.cpp
bestsplit.o: bestsplit.cpp
	g++ --std=c++11 -c bestsplit.cpp
xval.o: xval.cpp
	g++ --std=c++11 -c xval.cpp
cptable.o: cptable.cpp
	g++ --std=c++11 -c cptable.cpp
print_tree.o: print_tree.cpp
	g++ --std=c++11 -c print_tree.cpp
partition.o: partition.cpp
	g++ --std=c++11 -c partition.cpp
cppart.o: cppart.cpp 
	g++ --std=c++11 -c cppart.cpp
cppart: data_handler.o anova.o bestsplit.o xval.o cptable.o print_tree.o partition.o cppart.o
	g++ --std=c++11 data_handler.o anova.o bestsplit.o xval.o cptable.o print_tree.o partition.o cppart.o -o cppart

clean:
	rm *.o cppart *.tree
