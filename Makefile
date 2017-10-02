all:
	g++ *.cpp -o server -static -O3 -Wall 
	
clean:
	rm -f $(binaries) *.o
