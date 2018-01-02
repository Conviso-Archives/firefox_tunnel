CC=c++
st=-std=c++11 -lpthread -ldl -Wl,-z,relro,-z,now -fstack-protector-all

firefoxtunnel: src/firefox_tunnel.cpp 
	gcc -c src/lib/sqlite3.c 
	$(CC) -c src/*.cpp $(st)
	$(CC) -o bin/firefox_tunnel *.o $(st)
	
