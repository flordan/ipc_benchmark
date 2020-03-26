#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <sys/time.h>
#include <unistd.h>
#include <cstdlib> // std::atoi
#include <cstdio>
#include <cstring> //std:strerror
#include <errno.h> // errno

#include <boost/interprocess/ipc/message_queue.hpp>
#include "libboost_mq.h"

int main(int argc, char *argv[])
{
    std::cout << "Producer running" << std::endl;
    int pid = getpid();
    std::cout << "Producer PID " << pid << std::endl;

    std::ostringstream producer_queue_name_builder;
    producer_queue_name_builder << "producer" << pid;
    const char *producer_queue_name = producer_queue_name_builder.str().c_str();

    boost::interprocess::message_queue producer_mq(boost::interprocess::open_or_create, producer_queue_name, 20, 200);
    std::cout << "Obtained producer queue." << std::endl;

    char *consumer_pid = argv[1];
    std::cout << "Synchronizing with consumer " << consumer_pid << std::endl;

    std::ostringstream consumer_queue_name_builder;
    consumer_queue_name_builder << "consumer" << consumer_pid;
    const char *consumer_queue_name = consumer_queue_name_builder.str().c_str();
    boost::interprocess::message_queue consumer_mq(boost::interprocess::open_or_create, consumer_queue_name, 20, 200);

    std::cout << "Obtained consumer queue " << consumer_queue_name << std::endl;
    consumer_mq.send(producer_queue_name, std::strlen(producer_queue_name) + 1, 0);

    char in_buffer[200] = "";
    size_t recvd_size;
    unsigned int priority = 0;
    producer_mq.receive(in_buffer, 200, recvd_size, priority);
    boost::interprocess::message_queue::remove(producer_queue_name);
    std::cout << "Synchronized" << std::endl;

    std::cout << "Latency test" << std::endl;
    struct timeval st;
    for (int i = 0; i < LATENCY_PROBES; i++)
    {
        gettimeofday(&st, NULL);
        std::ostringstream os;
        os << st.tv_sec << "." << st.tv_usec + 1000000;
        std::string mystr = os.str();
        const char *time = mystr.c_str();
        consumer_mq.send(time, std::strlen(time) + 1, 0);
        usleep(250000);
    }

    std::cout << "Finished" << std::endl;
}