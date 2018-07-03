all: gini.o anova.o utils.o data_handler.o bestsplit.o xval.o cptable.o print_tree.o partition.o cppart.o cppart runmany.o runmany

gini.o: gini.cpp
	g++ --std=c++11 -c gini.cpp
utils.o: utils.cpp
	g++ --std=c++11 -c utils.cpp
data_handler.o: data_handler.cpp
	g++ --std=c++11 -c data_handler.cpp
anova.o: anova.cpp
	g++ --std=c++11 -c anova.cpp
bestsplit.o: bestsplit.cpp
	g++ --std=c++11 -pthread -c bestsplit.cpp
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
cppart: gini.o utils.o data_handler.o anova.o bestsplit.o xval.o cptable.o print_tree.o partition.o cppart.o
	g++ --std=c++11 -pthread gini.o utils.o data_handler.o anova.o bestsplit.o xval.o cptable.o print_tree.o partition.o cppart.o -o cppart
runmany.o: runmany.cpp
	g++ --std=c++11 -c runmany.cpp
runmany: gini.o utils.o data_handler.o anova.o bestsplit.o xval.o cptable.o print_tree.o partition.o cppart.o
	g++ -pthread --std=c++11 gini.o utils.o data_handler.o anova.o bestsplit.o xval.o cptable.o print_tree.o partition.o runmany.o -o runmany

clean:
	rm *.o cppart runmany
