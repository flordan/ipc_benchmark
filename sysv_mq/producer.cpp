#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <sys/time.h>
#include <unistd.h>
#include <cstdlib> // std::atoi
#include <sys/ipc.h>
#include <sys/msg.h>
#include "sysv_mq.h"

int main(int argc, char *argv[])
{
    std::cout << "Producer running" << std::endl;
    int pid = getpid();
    std::cout << "Producer PID " << pid << std::endl;

    int mq_producer, mq_consumer;
    if ((mq_producer = msgget (IPC_PRIVATE, 0660)) == -1) {
        std::cerr << "Cannot get queue for producer." << std::endl;
        return 1;
    }

    char *key_path=argv[1];
    char *consumer_pid = argv[2];
    
    std::cout << "Synchronizing with consumer key_path " << key_path << " project " << consumer_pid << std::endl;
    key_t msg_queue_key;
    if ((msg_queue_key = ftok (key_path, std::atoi(consumer_pid))) == -1) {
        std::cerr << "Cannot get key for file " << key_path <<" and project " << consumer_pid << std::endl;
         return 1;
    }
    if ((mq_consumer = msgget (msg_queue_key, 0)) == -1) {
        std::cerr << "Cannot get consumer queue." << std::endl;
        return 1;
    }
    else
    {
        std::cout << "Obtained consumer queue." << std::endl;
    }

    struct message_text out_message;
    out_message.reply_to = mq_producer;
    if (msgsnd (mq_consumer, &out_message, sizeof (struct message_text), 0) == -1) {
            std::cerr << "Not able to send message to consumer" << std::endl;
            return 1;
    }
    std::cout<<"Waiting for consumer ACK on queue " << mq_producer <<"."<<std::endl;

    struct message_text in_message;
    if (msgrcv (mq_producer,  &in_message, sizeof (struct message_text), 0, 0) == -1) {
            std::cerr << "Cannot read from producer queue."  << std::endl;
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
        strcpy(out_message.message, time);
        msgsnd (mq_consumer, &out_message, sizeof (struct message_text), 0);
        usleep(250000);
    }

    std::cout << "Finished" << std::endl;
}