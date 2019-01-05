all:	main.cpp
	g++ -Wall -o main main.cpp Struct.cpp Struct.hpp
	
clean:
	rm -f *o main
