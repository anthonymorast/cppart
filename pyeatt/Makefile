

OBJS=datatab.o  metric.o  node.o lacart.o utils.o data_handler.o

LFLAGS=-g

CFLAGS=-std=c++11 -g -Wall

INCLUDES=-I.

lacart: $(OBJS)
	g++ $(LFLAGS) -o lacart $(OBJS) -lm

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
