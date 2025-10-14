#include <unistd.h>

#ifndef SERVER_HPP
#define SERVER_HPP

typedef uint8_t BYTE;

class SERVER {
    const string PORT="3490"; // the port users will be connecting to
    const int BACKLOG=1; // how many pending connections queue will hold
    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
    
    struct addrinfo hints, *servinfo, *p;
    //...
    static void *get_in_addr(struct sockaddr *sa);

public:
    SERVER();
    ~SERVER();

    void waitConnection();
    void sendBytes(int nBytesToSend, BYTE *buf);
    void receiveBytes(int nBytesToReceive, BYTE *buf);
}; //Fim de SERVER

#endif