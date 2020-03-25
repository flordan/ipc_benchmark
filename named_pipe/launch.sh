#!/bin/bash

mkfifo /tmp/fifo
./producer /tmp/fifo &
producer_pid=$!
./consumer /tmp/fifo &
consumer_pid=$!
wait ${producer_pid}
wait ${consumer_pid}