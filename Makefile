CCOMPILE=mpic++ -std=c++11

#HADOOP_HOME= [path]
#JAVA_HOME= [path]
SYSTEM_PATH=[Your PregelPlus system path]

CPPFLAGS= -I$(HADOOP_HOME)/include -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I$(SYSTEM_PATH)  -Wno-deprecated -O2
CPPFLAGS_N= -I$(SYSTEM_PATH)  -Wno-deprecated -O2

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
