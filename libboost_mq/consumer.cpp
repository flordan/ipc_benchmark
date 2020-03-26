#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <sys/time.h>
#include <unistd.h>
#include <cstdlib>      // std::atoi
#include <cstdio>
#include <cstring>      //std:strerror
#include <errno.h>      // errno

#include <boost/interprocess/ipc/message_queue.hpp>
#include "libboost_mq.h"

int main(int argc, char *argv[])
{
    std::cout << "\t\t\t\t\tConsumer running" << std::endl;
    int pid = getpid();
    std::cout << "\t\t\t\t\tConsumer PID " << pid << std::endl;

    std::ostringstream consumer_queue_name_builder;
    consumer_queue_name_builder << "consumer" << pid;
    const char *consumer_queue_name = consumer_queue_name_builder.str().c_str();

    boost::interprocess::message_queue mq_consumer (boost::interprocess::open_or_create, consumer_queue_name, 20, 200);
    std::cout << "\t\t\t\t\tObtained consumer queue " << consumer_queue_name << "."<< std::endl;
    
    
    int mq_producer;
    unsigned int priority=0;
    std::cout << "\t\t\t\t\tSynchronizing with producer" << std::endl;
    size_t recvd_size;
    char in_buffer[200] = "";
    std::cout<<"\t\t\t\t\tWaiting for producer message on consumer queue." <<std::endl;
    mq_consumer.receive((void *)in_buffer, 200, recvd_size, priority);
    std::cout<<"\t\t\t\t\tWaiting for producer message on consumer queue." <<std::endl;
    char * mq_producer_name = in_buffer;
    std::cout << "\t\t\t\t\tProducer queue: " << mq_producer_name << std::endl;

    boost::interprocess::message_queue producer_mq (boost::interprocess::open_or_create, mq_producer_name, 20, 200);
    producer_mq.send(consumer_queue_name, std::strlen(consumer_queue_name) + 1, 0);
    std::cout << "\t\t\t\t\tSynchronized" << std::endl;

    struct timeval st;

    int max = 0;
    int min = 10000000;
    int total = 0;
    for (int i = 0; i < LATENCY_PROBES; i++)
    {
        mq_consumer.receive((void *)in_buffer, 200, recvd_size, priority);
        gettimeofday(&st, NULL);

        double send;
        std::sscanf(in_buffer, "%lf", &send);
        int send_secs = (int)send;
        int send_usec = ((send - send_secs)*10000000)-1000000;
        send = (double)send_secs+((double)send_usec/(double)1000000);
        double now = (double)st.tv_sec + ((double)st.tv_usec)/(double)1000000;
        double diff = (now - send) * 1000000;
        if (diff < 0)
        {
            diff += 1000000;
        }
        total += diff;
        if (diff > max)
        {
            max = diff;
        }
        if (diff < min)
        {
            min = diff;
        }
    }

    std::cout << "Latency test results" << std::endl;
    std::cout << "Average " << total / LATENCY_PROBES << " us" << std::endl;
    std::cout << "Max time " << max << " us" << std::endl;
    std::cout << "Min time " << min << " us" << std::endl;

    /*if (mq_close (mq_consumer) == -1) {
        std::cerr << "\t\t\t\t\tNot able to close consumer queue" << std::endl;
        return 1;
    }
    if (mq_unlink (consumer_queue_name) == -1) {
        std::cerr << "\t\t\t\t\tNot able to delete consumer queue" << std::endl;
        return 1;
    }*/
    
    std::cout << "\t\t\t\t\tFinished" << std::endl;
}