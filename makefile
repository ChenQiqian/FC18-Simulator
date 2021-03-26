TARGET = work.exe
COMPILER = g++ -std=c++11

target : $(TARGET)

ai.o : ai/ai.cpp
	$(COMPILER) -c ai/ai.cpp -o ai.o

definition.o : ai/definition.cpp
	$(COMPILER) -c ai/definition.cpp -o definition.o

main.o : ai/main.cpp
	$(COMPILER) -c ai/main.cpp -o main.o

json_value.o : lib_json/json_value.cpp
	$(COMPILER) -c lib_json/json_value.cpp -o json_value.o

json_writer.o : lib_json/json_writer.cpp
	$(COMPILER) -c lib_json/json_writer.cpp -o json_writer.o

json_reader.o : lib_json/json_reader.cpp
	$(COMPILER) -c lib_json/json_reader.cpp -o json_reader.o

$(TARGET) : ai.o definition.o main.o json_value.o json_writer.o json_reader.o
	$(COMPILER) -o $(TARGET) ai.o definition.o main.o json_value.o json_writer.o json_reader.o

clean :
	rm -f $(TARGET) ai.o definition.o main.o json_value.o json_writer.o json_reader.o