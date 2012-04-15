all: kdindex

# skiplist: skiplist.h skiplist.cpp
# 	g++ skiplist.cpp -o skiplist

kdindex: kd_test.cpp kd_skiplist.o skiplist.o
	g++ -g -std=c++0x -o kdindex kd_skiplist.o skiplist.o kd_test.cpp

kd_skiplist.o: kd_skiplist.h kd_skiplist.cpp skiplist.o
	g++ -g -c -std=c++0x -o kd_skiplist.o kd_skiplist.cpp

skiplist.o: skiplist.h skiplist.cpp
	g++ -g -c -std=c++0x -o skiplist.o skiplist.cpp

clean:
	rm *.o
# 	rm naive
	rm kdindex