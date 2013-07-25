#ifndef MESSAGES_H_
#define MESSAGES_H_

enum user_status {
    USER_STATUS_ONLINE,
    USER_STATUS_OFFLINE,
    USER_STATUS_LEAVE,
};

int msg_init();

void notify_status(enum user_status);


#endif