tube: tube.o main.o
	g++ -g tube.o main.o -o tube

tube.o: tube.h tube.cpp
	g++ -Wall -g -c tube.cpp
main.o: tube.h main.cpp
	g++ -Wall -g -c main.cpp
clean:
	rm -f *.o excute
