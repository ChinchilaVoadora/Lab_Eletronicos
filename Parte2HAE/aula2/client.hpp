#include <unistd.h>

#ifndef CLIENT_HPP
#define CLIENT_HPP

typedef uint8_t BYTE;

class CLIENT: public DEVICE {
    const string PORT="3490"; // the port client will be connecting to
    int sockfd, numbytes;
    //...
    static void *get_in_addr(struct sockaddr *sa);

public:
    CLIENT(string endereco);
    
    ~CLIENT();

    void sendBytes(int nBytesToSend, BYTE *buf);
    void receiveBytes(int nBytesToReceive, BYTE *buf);
}; //Fim de CLIENT

#endif