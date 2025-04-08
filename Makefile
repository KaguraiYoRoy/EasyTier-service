CC = g++
OUTPUT_DIR = out

easytier-service:main.o log.o
	$(CC) main.o log.o -o easytier-service -lcurl -ljsoncpp

main.o:EasyTier-service.cpp 
	$(CC) EasyTier-service.cpp -c -Wall -g -o main.o -I/usr/include/jsoncpp

log.o:Log.h Log.cpp
	$(CC) Log.cpp -c -Wall -g -o log.o
	
.PHONY : clean
clean:
	rm -f *.o easytier-service
	