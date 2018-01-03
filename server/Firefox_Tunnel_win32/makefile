CC=c++
st= -fstack-protector-all 
firefoxtunnel: src/firefox_tunnel.cpp 
	gcc -c src/lib/sqlite3.c 
	$(CC) -c src/*.cpp $(st)
	$(CC) -o bin/firefox_tunnel.exe *.o  $(st)
	