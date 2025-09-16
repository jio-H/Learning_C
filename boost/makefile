.PHONY: main clean

preority_que: preority_que.cpp
	g++ preority_que.cpp -o preority_que -lboolst_systream

timer: timer.cpp
	g++ timer.cpp -o timer -lboolst_systream

# $@ 表示目标，$^ 表示所有依赖，$< 表示第一个依赖
# simple:  main.o foo.o foo2.o
# 	g++ -o simple main.o foo.o
# main.o: main.cpp
# 	g++ -o main.o -c main.cpp
# foo.o: foo.cpp
# 	g++ -o foo.o -c foo.cpp
# foo2.o: foo2.cpp
# 	g++ -o foo2.o -c foo2.cpp
# clean:
# 	rm -f *.o simple

# .PHONY: all clean

# CXX = g++
# CXXFLAGS = -Wall -Wextra -std=c++11

# all: simple

# simple: main.o foo.o foo2.o
# 	$(CXX) $(CXXFLAGS) -o $@ $^

# main.o: main.cpp
# 	$(CXX) $(CXXFLAGS) -c -o $@ $<

# foo.o: foo.cpp
# 	$(CXX) $(CXXFLAGS) -c -o $@ $<

# foo2.o: foo2.cpp
# 	$(CXX) $(CXXFLAGS) -c -o $@ $<

# clean:
# 	rm -f *.o simple