#!/bin/bash
./consumer &
consumer_pid=$!
./producer ${consumer_pid} &
producer_pid=$!

wait ${producer_pid}
wait ${consumer_pid}


