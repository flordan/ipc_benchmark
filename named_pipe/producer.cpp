#include <iostream>
#include <string>
#include <sys/time.h>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include<sstream>

#include "named_pipe.h"

int main(int argc, char *argv[])
{
    std::cout << "Producer running" << std::endl;
    char * pipe_name = argv[1];
    std::cout << "Using pipe " << pipe_name << std::endl;

    std::cout << "Synchronizing with consumer" << std::endl;
    int fd;
    ssize_t size;
    char str1[80];

    fd = open(pipe_name, O_WRONLY);
    size = write(fd, "SYN", 4);
    close(fd);

    fd = open(pipe_name, O_RDONLY);
    size = read(fd, str1, sizeof(str1));
    std::cout << str1 <<std::endl;
    close(fd);

    std::cout << "Synchronized" << std::endl;

    fd = open(pipe_name, O_WRONLY);
    struct timeval st;
    std::cout << "Latency test" << std::endl;
    for (int i = 0; i < LATENCY_PROBES; i++)
    {
        gettimeofday(&st, NULL);
        std::ostringstream os ;
        os << st.tv_sec << "." << st.tv_usec+1000000;
        std::string mystr = os.str();
        size = write(fd, mystr.c_str(), mystr.length());
        usleep(250000);
    }
    std::cout << "Finished" << std::endl;
}