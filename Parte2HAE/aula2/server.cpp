//projeto.hpp
#include "raspberry.hpp"
#include <unistd.h>

//...

class SERVER {
    const string PORT="3490"; // the port users will be connecting to
    const int BACKLOG=1; // how many pending connections queue will hold
    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
    
    struct addrinfo hints, *servinfo, *p;
...
    static void *get_in_addr(struct sockaddr *sa) {
    //Esta funcao esta repetida em SERVER e CLIENT.
    //E’ possivel unificar usando DEVICE.
    ...
    }

public:
    SERVER() {
        memset(&hints, 0, sizeof hints);
        //...
    }
    ~SERVER() {
        close( ...
    }

    void waitConnection() {
        printf("server: waiting for connections...\n");
        while (1) {
    ...
    }
    void sendBytes(int nBytesToSend, BYTE *buf) {
    ...
    }
    void receiveBytes(int nBytesToReceive, BYTE *buf) {
    ...
    }
}; //Fim de SERVER