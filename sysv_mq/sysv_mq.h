const int LATENCY_PROBES=1000;

const int MESSAGE_LENGTH=200;

struct message_text {
    int reply_to;
    char message [MESSAGE_LENGTH];
};

