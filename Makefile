CCOMPILE=mpic++ -std=c++11

#HADOOP_HOME= [path]
#JAVA_HOME= [path]
CPPFLAGS= -I$(HADOOP_HOME)/include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I/home/naughtycat/PregelPlus/system  -Wno-deprecated -O2

CPPFLAGS_N= -I/home/naughtycat/PregelPlus/system  -Wno-deprecated -O2
LIB = -L$(HADOOP_HOME)/lib/native
LDFLAGS = -lhdfs

CFLAGS += -DHDFS

all: run

run: run.cpp
	$(CCOMPILE) $(CFLAGS) run.cpp $(CPPFLAGS) $(LIB) $(LDFLAGS)  -o run

run_tianhe: run.cpp
	$(CCOMPILE) run.cpp $(CPPFLAGS_N) $(LIB)  -o run

clean:
	-rm run
