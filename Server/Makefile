all:
	g++ -std=c++11 *.cpp -o myserver -Wall -pthread -lldap -DLDAP_DEPRECATED -llber
	
clean:
	rm -f $(binaries) *.o
