

OBJS=datatab.o metric.o node.o lacart.o utils.o data_handler.o
OBJS2=datatab.o metric.o node.o utils.o data_handler.o forest.o laforest.o

# for function inlining, use at least -O3 or -Ofast

#LFLAGS=-g -Ofast -lm -lomp
#CFLAGS=-std=c++11 -g -Wall -Ofast -fopenmp

LFLAGS=-g -Ofast -fopenmp
CFLAGS=-std=c++11 -g -Wall -Ofast -fopenmp

INCLUDES=-I.

all: lacart laforest

lacart: $(OBJS)
	mpic++ -o lacart $(OBJS) $(LFLAGS) 

laforest: $(OBJS2)
	mpic++ $(LFLAGS) -o laforest $(OBJS2) -lm

.cc.o:
	mpic++ $(INCLUDES) $(CFLAGS) -c $<


clean:
	rm -f *.o lacart laforest


# make depend will create a file ".depend" with all the dependencies

depend: 
	rm -f .depend 
	g++ $(CFLAGS) $(DEFINES) $(INCLUDES) $(DEPENDFLAGS) $(OBJS:.o=.cc) > .depend



# if we have a .depend file, include it

ifeq (.depend,$(wildcard .depend))
include .depend
endif
