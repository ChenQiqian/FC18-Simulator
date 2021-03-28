TARGET = worker.exe
COMPILER = g++-10 -std=c++11 -g

target : $(TARGET)

definition.o : definition.cpp definition.h
	$(COMPILER) -c definition.cpp -o definition.o

main.o : main.cpp
	$(COMPILER) -c main.cpp -o main.o

json_value.o : lib_json/json_value.cpp
	$(COMPILER) -c lib_json/json_value.cpp -o json_value.o

json_writer.o : lib_json/json_writer.cpp
	$(COMPILER) -c lib_json/json_writer.cpp -o json_writer.o

json_reader.o : lib_json/json_reader.cpp
	$(COMPILER) -c lib_json/json_reader.cpp -o json_reader.o

$(TARGET) : definition.o main.o json_value.o json_writer.o json_reader.o
	$(COMPILER) -o $(TARGET) definition.o main.o json_value.o json_writer.o json_reader.o

clean :
	rm -f $(TARGET) definition.o main.o json_value.o json_writer.o json_reader.o