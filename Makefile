all: run

run: clean
	cd pipe && make run && cd ..
	cd named_pipe && make run && cd ..
	cd posix_mq && make run && cd ..
	cd sysv_mq && make run && cd ..

clean:
	cd pipe && make clean && cd ..
	cd named_pipe && make clean && cd ..
	cd posix_mq && make clean && cd ..
	cd sysv_mq && make clean && cd ..
