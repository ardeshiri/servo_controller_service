CC = g++
TARGET = servo_ctrl_service
CFLAGS  = -Wall -O2 -std=c++20 
INCLUDES = -I./include
LFLAGS = -pthread -lstdc++fs -lssl -lcrypto -lpthread -lrabbitmq
all : $(TARGET)

$(TARGET): main.o utility.o Serial_connector.o Serial_command.o CPPAMQP.o
	$(CC) -o ./bin/Release/$(TARGET) ./obj/Release/CPPAMQP.o ./obj/Release/main.o ./obj/Release/Serial_command.o ./obj/Release/Serial_connector.o ./obj/Release/utility.o $(LFLAGS)

main.o: ./src/main.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c ./src/main.cpp -o ./obj/Release/main.o
	
utility.o: ./src/utility.cpp ./include/utility.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./src/utility.cpp -o ./obj/Release/utility.o
	
Serial_connector.o: ./src/Serial_connector.cpp ./include/Serial_connector.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./src/Serial_connector.cpp -o ./obj/Release/Serial_connector.o
	
Serial_command.o: ./src/Serial_command.cpp ./include/Serial_command.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./src/Serial_command.cpp -o ./obj/Release/Serial_command.o
	
CPPAMQP.o: ./src/CPPAMQP.cpp ./include/CPPAMQP.h
	$(CC) $(CFLAGS) $(INCLUDES) -c ./src/CPPAMQP.cpp -o ./obj/Release/CPPAMQP.o
		
.PHONY: clean

clean:
	rm -f ./obj/Release/*.o  ./bin/Release/$(TARGET)
