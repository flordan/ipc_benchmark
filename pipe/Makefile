CC = g++
CPP_FLAGS = -O3

all: run
    
run: producer consumer
	echo "Running test"
	./producer | ./consumer

producer: producer.cpp
	$(CC) $(CPP_FLAGS) $(<) -o $(@)
	chmod +x ./producer

consumer: consumer.cpp
	$(CC) $(CPP_FLAGS) $(<) -o $(@)
	chmod +x ./consumer

clean:
	rm -rf consumer producer
