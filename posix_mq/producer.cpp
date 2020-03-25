#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <sys/time.h>
#include <mqueue.h>
#include <unistd.h>

#include "posix_mq.h"

int main(int argc, char *argv[])
{
    std::cout << "Producer running" << std::endl;

    int pid = getpid();
    std::cout << "Producer PID " << pid << std::endl;

    mqd_t mq_producer, mq_consumer;

    std::ostringstream producer_queue_name_builder;
    producer_queue_name_builder << "/producer" << pid;
    const char *producer_queue_name = producer_queue_name_builder.str().c_str();

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    if ((mq_producer = mq_open(producer_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1)
    {
        std::cerr << "Cannot open " << producer_queue_name << std::endl;
        return 1;
    }
    else
    {
        std::cout << "Created queue " << producer_queue_name << std::endl;
    }

    char *consumer_pid = argv[1];
    std::ostringstream consumer_queue_name_builder;
    consumer_queue_name_builder << "/consumer" << consumer_pid;
    const char *consumer_queue_name = consumer_queue_name_builder.str().c_str();

    std::cout << "Synchronizing with consumer " << consumer_queue_name << std::endl;
    while ((mq_consumer = mq_open(consumer_queue_name, O_WRONLY)) == -1)
    {
        std::cerr << "Queue " << consumer_queue_name << " not yet open." << std::endl;
        usleep(500000);
    }
    if (mq_send(mq_consumer, producer_queue_name, std::strlen(producer_queue_name) + 1, 0) == -1)
    {
        std::cerr << "Not able to send message to consumer" << std::endl;
        return 0;
    }
    char in_buffer[MSG_BUFFER_SIZE];
    if (mq_receive(mq_producer, in_buffer, MSG_BUFFER_SIZE, NULL) == -1)
    {
        std::cerr << "Cannot read from queue " << producer_queue_name << std::endl;
        return 1;
    }

    if (mq_close (mq_producer) == -1) {
        std::cerr << "Not able to close producer queue" << std::endl;
        return 1;
    }
    if (mq_unlink (producer_queue_name) == -1) {
        std::cerr << "Not able to delete producer queue "<< producer_queue_name << std::endl;
        return 1;
    }
    std::cout << "Synchronized" << std::endl;

    std::cout << "Latency test" << std::endl;
    
    struct timeval st;
    for (int i = 0; i < LATENCY_PROBES; i++)
    {
        gettimeofday(&st, NULL);
        std::ostringstream os ;
        os << st.tv_sec << "." << st.tv_usec+1000000;
        std::string mystr = os.str();
        const char* time = mystr.c_str();
        mq_send(mq_consumer,time, std::strlen(time)+1,0);
        usleep(250000);
    }

    if (mq_close (mq_consumer) == -1) {
        std::cerr << "Not able to close consumer queue" << std::endl;
        return 1;
    }

    std::cout << "Finished" << std::endl;
}