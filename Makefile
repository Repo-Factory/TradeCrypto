# CXX Make variable for compiler
CXX=g++
# -std=c++11  C/C++ variant to use, e.g. C++ 2011
# -Wall       show the necessary warning files
# -g3         information for symbolic debugger e.g. gdb 
CXXFLAGS=-std=c++11 -Wall -g3 -c
LDFLAGS=-pthread
INCLUDE=-I./

# object files
OBJS = report.o main.o args_handling.o producer.o consumer.o parent_thread.o
HEADERS = report.h tradecrypto.h args_handling.h producer.h consumer.h parent_thread.h
# Program name
PROGRAM = tradecrypto

# Rules format:
# target : dependency1 dependency2 ... dependencyN
#     Command to make target, uses default rules if not specified

# First target is the one executed if you just type make
# make target specifies a specific target
# $^ is an example of a special variable.  It substitutes all dependencies
$(PROGRAM) : $(OBJS) $(HEADERS)
	$(CXX) -o $(PROGRAM) $^ $(LDFLAGS)

report.o : report.h report.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) report.cpp
	
tradecrypto.o : tradecrypto.h tradecrypto.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) tradecrypto.cpp
	
args_handling.o : args_handling.h args_handling.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) args_handling.cpp
	
producer.o : producer.h producer.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) producer.cpp

consumer.o : consumer.h consumer.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) consumer.cpp

parent_thread.o : parent_thread.h parent_thread.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) parent_thread.cpp

main.o : report.cpp args_handling.cpp producer.cpp consumer.cpp parent_thread.cpp main.cpp 
	$(CXX) $(CXXFLAGS) $(INCLUDE) main.cpp
	
clean :
	rm -f *.o $(PROGRAM)
