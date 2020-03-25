#include <iostream>
#include <string>
#include <sys/time.h>
#include <cstdlib>
#include <cstdio>

#include "pipe.h"

int main()
{
    std::cout.precision(17);
    std::cout << "Consumer runnning" << std::endl;
    std::string mystr;
    struct timeval st;

    int max = 0;
    int min = 10000000;
    int total = 0;
    for (int i = 0; i < LATENCY_PROBES; i++)
    {
        std::cin >> mystr;
        gettimeofday(&st, NULL);
        double send;
        std::sscanf(mystr.c_str(), "%lf", &send);
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
}