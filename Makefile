CC = g++
OUTPUT_DIR = out

easytier-service:main.o
	$(CC) main.o -o easytier-service -lcurl -ljsoncpp

main.o:EasyTier-service.cpp 
	$(CC) EasyTier-service.cpp -c -Wall -g -o main.o -I/usr/include/jsoncpp

.PHONY : clean
clean:
	rm -f *.o easytier-service
	