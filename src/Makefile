

OBJS=datatab.o metric.o node.o lacart.o utils.o data_handler.o
OBJS2=datatab.o metric.o node.o utils.o data_handler.o forest.o laforest.o

LFLAGS=-g

CFLAGS=-std=c++11 -g -Wall -O2

INCLUDES=-I.

all: lacart laforest

lacart: $(OBJS)
	g++ $(LFLAGS) -o lacart $(OBJS) -lm

laforest: $(OBJS2)
	g++ $(LFLAGS) -o laforest $(OBJS2) -lm

.cc.o:
	g++ $(INCLUDES) $(CFLAGS) -c $<


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
