#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "pipe.h"


int main() {
    std::cerr << "Producer running" << std::endl;
    
    struct timeval st;
    std::cerr << "Latency test" << std::endl;
    for (int i=0;i<LATENCY_PROBES;i++){
        gettimeofday(&st,NULL);
        std::cout << st.tv_sec << "." << 1000000 + st.tv_usec << std::endl;
        usleep(250000);
    }

}