#include <iostream>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <mqueue.h>
#include <unistd.h>
#include <stdlib.h> /* atoi */
#include <cstdio>

#include "posix_mq.h"

int main(int argc, char *argv[])
{
    std::cout << "\t\t\t\t\tConsumer running" << std::endl;
    int pid = getpid();
    std::cout << "\t\t\t\t\tConsumer PID " << pid << std::endl;

    mqd_t mq_producer, mq_consumer;

    std::ostringstream os;
    os << "/consumer" << pid;
    const char *consumer_queue_name = os.str().c_str();

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    if ((mq_consumer = mq_open(consumer_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1)
    {
        std::cerr << "\t\t\t\t\tCannot open " << consumer_queue_name << std::endl;
        return 1;
    }
    else
    {
        std::cout << "\t\t\t\t\tCreated queue " << consumer_queue_name << std::endl;
    }

    std::cout << "\t\t\t\t\tSynchronizing with producer" << std::endl;
    char producer_queue_name[MSG_BUFFER_SIZE];
    if (mq_receive(mq_consumer, producer_queue_name, MSG_BUFFER_SIZE, NULL) == -1)
    {
        std::cerr << "\t\t\t\t\tCannot read from queue " << consumer_queue_name << std::endl;
        return 1;
    }

    std::cout << "\t\t\t\t\tProducer queue: " << producer_queue_name << std::endl;
    if ((mq_producer = mq_open(producer_queue_name, O_WRONLY)) == -1)
    {
        std::cerr << "\t\t\t\t\tQueue " << producer_queue_name << " not yet open." << std::endl;
        return -1;
    }
    if (mq_send(mq_producer, "ACK", 3 + 1, 0) == -1)
    {
        std::cerr << "\t\t\t\t\tNot able to send message to consumer" << std::endl;
        return 0;
    }
    std::cout << "\t\t\t\t\tSynchronized" << std::endl;
    if (mq_close (mq_producer) == -1) {
        std::cerr << "\t\t\t\t\tNot able to close consumer queue" << std::endl;
        return 1;
    }

    struct timeval st;

    int max = 0;
    int min = 10000000;
    int total = 0;
    char in_buffer[MSG_BUFFER_SIZE];
    for (int i = 0; i < LATENCY_PROBES; i++)
    {
        if (mq_receive(mq_consumer, in_buffer, MSG_BUFFER_SIZE, NULL) == -1)
        {
            std::cerr << "\t\t\t\t\tCannot read from queue " << consumer_queue_name << std::endl;
            return 1;
        }
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

    if (mq_close (mq_consumer) == -1) {
        std::cerr << "\t\t\t\t\tNot able to close consumer queue" << std::endl;
        return 1;
    }
    if (mq_unlink (consumer_queue_name) == -1) {
        std::cerr << "\t\t\t\t\tNot able to delete consumer queue" << std::endl;
        return 1;
    }
    
    std::cout << "\t\t\t\t\tFinished" << std::endl;
}