#include "raspberry.hpp"

//(... outros includes ...)
//(... outras classes como SERVER e CLIENT e funções ...)
typedef uint8_t BYTE;

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define MAXDATASIZE 256 // max number of bytes we can get at once 

typedef uint8_t BYTE;

class DEVICE {
public:
    const string PORT="3490"; // the port users will be connecting to
    
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;

	struct sigaction sa;
	char s[INET6_ADDRSTRLEN];
	int rv;

    
    static void *get_in_addr(struct sockaddr *sa) {
        if (sa->sa_family == AF_INET) {
            return &(((struct sockaddr_in*)sa)->sin_addr);
        }
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }

    virtual void sendBytes(int nBytesToSend, BYTE *buf2) = 0;
    virtual void receiveBytes(int nBytesToReceive, BYTE *buf2) = 0;

    void sendUint(uint32_t m) {
        uint32_t m2 = htonl(m);

        this->sendBytes(4, (BYTE*)&m2);
        std::cout << "Enviou " << m2 << endl;
    }

    void receiveUint(uint32_t &m) {
        uint32_t m2;

        this->receiveBytes(4, (BYTE*)&m2);
        std::cout << "Recebeu " << m2 << endl;

        m = ntohl(m2);
    }


    void sendVb(const vector<BYTE>& vb) {
        this->sendUint(vb.size());
        this->sendBytes(vb.size(), (BYTE*) vb.data());
    }

    void receiveVb(vector<BYTE>& vb) {
        uint32_t m;
        this->receiveUint(m);

        vb.resize(m);

        this->receiveBytes(m, (BYTE*) vb.data());
    }   

    void sendImg(const Mat_<COR>& img) {
        if (img.isContinuous() == false)
            perror("Not continuous image");

        std::cout << "Rows: " << img.rows << "  Cols: " << img.cols << endl;
        this->sendUint(img.rows); this->sendUint(img.cols);

        this->sendBytes(3*img.total(), img.data);
        std::cout << "Enviou " << img.total()*3 << " bytes\n";
    }

    void receiveImg(Mat_<COR>& img) {
        uint32_t rows, cols;

        this->receiveUint(rows); this->receiveUint(cols);
        std::cout << "Rows: " << rows << "  Cols: " << cols << endl;
        
        img.create(rows, cols);

        this->receiveBytes(3*rows*cols, img.data);
        std::cout << "Recebeu " << 3*rows*cols << " bytes\n";
    }

    void sendImgComp(const Mat_<COR> &img) {
        vector<BYTE> vb;
        vector<int> param{CV_IMWRITE_JPEG_QUALITY,80};
        imencode(".jpg",img,vb,param);

        this->sendVb(vb);
    }

    void receiveImgComp(Mat_<COR>& img) {
        vector<BYTE> vb;
        this->receiveVb(vb);
        // Função para receber o vetor de bytes vb
        img=imdecode(vb,1); // Numero 1 indica imagem colorida
    }
};

class SERVER: public DEVICE {
    const int BACKLOG=1; // how many pending connections queue will hold
    
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd

    struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;

    int yes=1;

public:
    SERVER() {
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE; // use my IP
        if ((rv = getaddrinfo(NULL, (PORT).c_str(), &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        }
        
        // loop through all the results and bind to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) {
            if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
                perror("server: socket");
                continue;
            }
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
            }
            if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                close(sockfd);
                perror("server: bind");
                continue;
            }
            break;
        }
        freeaddrinfo(servinfo); // all done with this structure
        if (p == NULL)  {
            fprintf(stderr, "server: failed to bind\n");
            exit(1);
        }
        
        if (listen(sockfd, BACKLOG) == -1) {
            perror("listen");
            exit(1);
        }
    }
    ~SERVER() {
        close (this->new_fd);
    }

    void waitConnection() {
		printf("server: waiting for connections...\n");
	
		while (1) {
			sin_size = sizeof their_addr;
			this->new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);	
			if (this->new_fd == -1) {
				perror("accept");
				continue;
			} else break;
		}
		
		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);
		close(sockfd); // doesn't need the listener anymore
	}
    void sendBytes(int nBytesToSend, BYTE *buf) {
        int sent = 0;
        while (sent < nBytesToSend) {
            int result = send(this->new_fd, buf + sent, nBytesToSend - sent, 0);

            sent += result;
            //std::cout << "Enviando " << result << " bytes" << endl;
            //std::cout << "Faltam " << nBytesToSend - sent << " bytes" << endl << endl;

            if (result == -1)
                perror("send");
        }
	}
    void receiveBytes(int nBytesToReceive, BYTE *buf) {
        int received = 0;
        while (received < nBytesToReceive > 0) {
            int result = recv(new_fd, buf + received, nBytesToReceive - received, 0);

            received += result;

            if (result == -1)
                perror("recv");

        }
	}
}; //Fim de SERVER

class CLIENT: public DEVICE {
    int sockfd, numbytes;  
    

public:
    CLIENT(string endereco) {
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        if ((rv = getaddrinfo((endereco).c_str(), (PORT).c_str(), &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        }
    
        // loop through all the results and connect to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) {
            if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
                perror("client: socket");
                continue;
            }
            if (connect(this->sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                perror("client: connect");
                close(this->sockfd);
                continue;
            }
            break;
        }
        if (p == NULL) {
            fprintf(stderr, "client: failed to connect\n");
        }
    
        inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
        printf("client: connecting to %s\n", s);
        freeaddrinfo(servinfo); // all done with this structure
    
    }
    
    ~CLIENT() {
        close(this->sockfd);
    }

    void sendBytes(int nBytesToSend, BYTE *buf) {
        int sent = 0;
        while (sent < nBytesToSend) {
            int result = send(this->sockfd, buf + sent, nBytesToSend - sent, 0);

            //std::cout << "Enviando " << result << " bytes" << endl;
            //std::cout << "Faltam " << remaining << " bytes" << endl << endl;

            sent += result;
            if (result == -1)
                perror("send");
        }
    }
    void receiveBytes(int nBytesToReceive, BYTE *buf) {
        int received = 0;
        while (received < nBytesToReceive) {
            int result = recv(this->sockfd, buf + received, nBytesToReceive - received, 0);
            
            //std::cout << "Recebendo " << remaining << " bytes" << endl;

            received += result;
            if (result == -1)
                perror("recv");
        }
    }
};



bool testaBytes(BYTE* buf, BYTE b, int n) {
//Testa se n bytes da memoria buf possuem valor b
    bool igual=true;

    for (unsigned i=0; i<n; i++)
        if (buf[i]!=b) { igual=false; break; }
        
    return igual;
}

bool testaVb(const vector<BYTE> vb, BYTE b) {
    //Testa se todos os bytes de vb possuem valor b
    bool igual=true;
    
    for (unsigned i=0; i<vb.size(); i++)
        if (vb[i]!=b) { igual=false; break; }
    
    return igual;
}
