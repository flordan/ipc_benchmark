CC = g++
CPP_FLAGS = -O3
CPP_LIBS = -lrt

all: run
    
run: producer consumer
	echo "Running test"
	./launch.sh


producer: producer.cpp
	$(CC) $(CPP_FLAGS) $(<) -o $(@) $(CPP_LIBS)
	chmod +x ./producer

consumer: consumer.cpp
	$(CC) $(CPP_FLAGS) $(<) -o $(@) $(CPP_LIBS)
	chmod +x ./consumer

clean:
	rm -rf consumer producer