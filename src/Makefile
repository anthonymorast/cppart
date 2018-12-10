

OBJS=datatab.o  metric.o  node.o lacart.o utils.o data_handler.o

# for function inlining, use at least -O3 or -Ofast

LFLAGS=-g -Ofast -lm -lomp
CFLAGS=-std=c++11 -g -Wall -Ofast -fopenmp

#LFLAGS=-g -Ofast
#CFLAGS=-std=c++11 -g -Wall -Ofast

INCLUDES=-I.

lacart: $(OBJS)
	g++ -o lacart $(OBJS) $(LFLAGS) 

.cc.o:
	g++ $(INCLUDES) $(CFLAGS) -c $<


clean:
	rm -f *.o lacart


# make depend will create a file ".depend" with all the dependencies

depend: 
	rm -f .depend 
	g++ $(CFLAGS) $(DEFINES) $(INCLUDES) $(DEPENDFLAGS) $(OBJS:.o=.cc) > .depend



# if we have a .depend file, include it

ifeq (.depend,$(wildcard .depend))
include .depend
endif
