#!/bin/bash
KEY_PATH=/tmp/sysv_key
touch ${KEY_PATH}

./consumer ${KEY_PATH} &
consumer_pid=$!
./producer ${KEY_PATH} ${consumer_pid} &
producer_pid=$!

wait ${producer_pid}
wait ${consumer_pid}
rm -rf ${KEY_PATH}


