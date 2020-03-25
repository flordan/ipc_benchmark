CC = g++
CPP_FLAGS = -O3

all: run
    
run: producer consumer
	echo "Running test"
	rm -rf /tmp/fifo
	./launch.sh

fifo:
	mkfifo fifo

producer: producer.cpp
	$(CC) $(CPP_FLAGS) $(<) -o $(@)
	chmod +x ./producer

consumer: consumer.cpp
	$(CC) $(CPP_FLAGS) $(<) -o $(@)
	chmod +x ./consumer

clean:
	rm -rf consumer producer /tmp/fifo
