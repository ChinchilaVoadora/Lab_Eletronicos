//projeto.hpp (continuacao)
class CLIENT: public DEVICE {
    const string PORT="3490"; // the port client will be connecting to
    int sockfd, numbytes;
    ...
    static void *get_in_addr(struct sockaddr *sa) {
        //Esta funcao esta repetida em SERVER e CLIENT.
        //E’ possivel unificar usando DEVICE.
        ...
    }

public:
    CLIENT(string endereco) {
        memset(&hints, 0, sizeof hints);
        ...
    }
    
    ~CLIENT() {
        close(sockfd);
    }

    void sendBytes(int nBytesToSend, BYTE *buf) {
        ...
    }

    void receiveBytes(int nBytesToReceive, BYTE *buf) {
        ...
    }
}; //Fim de CLIENT