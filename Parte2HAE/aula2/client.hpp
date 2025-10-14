#include <unistd.h>

#ifndef CLIENT_HPP
#define CLIENT_HPP

typedef uint8_t BYTE;

class CLIENT: public DEVICE {
    const string PORT="3490"; 
    int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
    
    static void *get_in_addr(struct sockaddr *sa);

public:
    CLIENT(string endereco);
    
    ~CLIENT();

    void sendBytes(int nBytesToSend, BYTE *buf);
    void receiveBytes(int nBytesToReceive, BYTE *buf);
};

#endif