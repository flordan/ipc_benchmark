#include <iostream>
#include <string>
#include <sys/time.h>
#include <cstdlib>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <cstdio>

#include "named_pipe.h"

int main(int argc, char *argv[])
{
    std::cout.precision(17);
    std::cout << "\t\t\t\t\tConsumer running" << std::endl;
    char * pipe_name = argv[1];
    std::cout << "\t\t\t\t\tUsing pipe " << pipe_name << std::endl;

    std::cout << "\t\t\t\t\tSynchronizing with producer" << std::endl;
    int fd;
    ssize_t size;
    char str1[80];

    fd = open(pipe_name,O_RDONLY);
    size = 0;
    while (size == 0) {
        size = read(fd, str1, 80);
    } 
    close(fd); 

    fd = open(pipe_name,O_WRONLY); 
    size = write(fd, "ACK", 4); 
    close(fd); 

    std::cout << "\t\t\t\t\tSynchronized" << std::endl;


    fd = open(pipe_name,O_RDONLY); 
    struct timeval st;
    int max = 0;
    int min = 10000000;
    int total = 0;
    for (int i = 0; i < LATENCY_PROBES; i++)
    {
        size = read(fd, str1, 80);
        gettimeofday(&st, NULL);

        double send;
        std::sscanf(str1, "%lf", &send);
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

    std::cout << "\t\t\t\t\tFinished" << std::endl;
}