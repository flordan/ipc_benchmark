#include <iostream>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <mqueue.h>
#include <unistd.h>
#include <cstdlib>      // std::atoi
#include <cstdio>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <cstring>      //std:strerror
#include <errno.h>      // errno

#include "sysv_mq.h"

int main(int argc, char *argv[])
{
    std::cout << "\t\t\t\t\tConsumer running" << std::endl;
    int pid = getpid();
    std::cout << "\t\t\t\t\tConsumer PID " << pid << std::endl;

    int mq_producer, mq_consumer;
    char *key_path=argv[1];

    key_t msg_queue_key;
    if ((msg_queue_key = ftok (key_path, pid)) == -1) {
        std::cerr << "\t\t\t\t\tCannot get key for file " << key_path <<" and project " << pid << std::endl;
         return 1;
    }
    if ((mq_consumer = msgget (msg_queue_key, IPC_CREAT |0660)) == -1) {
        std::cerr << "\t\t\t\t\tCannot get consumer queue. -"<<errno<<"-" << std::strerror(errno) << std::endl;
        return 1;
    }
    else
    {
        std::cout << "\t\t\t\t\tObtained consumer queue." << std::endl;
    }

    
    std::cout << "\t\t\t\t\tSynchronizing with producer" << std::endl;
    struct message_text in_message;
    std::cout<<"\t\t\t\t\tWaiting for producer message on queue " << mq_consumer <<"."<<std::endl;
    if (msgrcv (mq_consumer,  &in_message, sizeof (struct message_text), 0, 0) == -1) {
            std::cerr << "\t\t\t\t\tCannot read from consumer queue."  << std::endl;
            return 1;
    }
    mq_producer = in_message.reply_to;
    std::cout << "\t\t\t\t\tProducer queue: " << mq_producer << std::endl;
    struct message_text out_message;
    out_message.reply_to = mq_consumer;
    if (msgsnd (mq_producer, &out_message, sizeof (struct message_text), 0) == -1) {
            std::cerr << "\t\t\t\t\tNot able to send message to producer" << std::endl;
            return 1;
    }

    std::cout << "\t\t\t\t\tSynchronized" << std::endl;

    struct timeval st;

    int max = 0;
    int min = 10000000;
    int total = 0;
    for (int i = 0; i < LATENCY_PROBES; i++)
    {
        if (msgrcv (mq_consumer,  &in_message, sizeof (struct message_text), 0, 0) == -1) {
                std::cerr << "\t\t\t\t\tCannot read from consumer queue."  << std::endl;
                return 1;
        }
        gettimeofday(&st, NULL);

        double send;
        std::sscanf(in_message.message, "%lf", &send);
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