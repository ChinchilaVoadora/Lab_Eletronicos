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

class SERVER {
    const string PORT="3490"; // the port users will be connecting to
    const int BACKLOG=1; // how many pending connections queue will hold
    
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

    static void *get_in_addr(struct sockaddr *sa) {
        if (sa->sa_family == AF_INET) {
            return &(((struct sockaddr_in*)sa)->sin_addr);
        }
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }

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
    void sendBytes(int nBytesToSend, BYTE *buf2) {
        const char* buf = reinterpret_cast<const char*>(buf2);
        int remaining = nBytesToSend;
        while (remaining > 0) {
            int result = send(this->new_fd, buf2, strlen(buf)+1, 0);

            if (result == -1)
                perror("send");

            else if (result < remaining) {
                remaining -= result;

                // deslocar o ponteiro de result bytes
                buf += result;
            }
        }
	}
    void receiveBytes(int nBytesToReceive, BYTE *buf2) {
        const char* buf = reinterpret_cast<const char*>(buf2);
        int remaining = nBytesToReceive;
        while (remaining > 0) {
            int result = recv(new_fd, buf2, MAXDATASIZE, 0);

            if (result == -1)
                perror("recv");

            else if (result < remaining) {
                remaining -= result;

                // deslocar o ponteiro de result bytes
                buf += result;
            }
        }
	}
}; //Fim de SERVER

class CLIENT {
    const string PORT="3490"; 
    int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
    
    static void *get_in_addr(struct sockaddr *sa) {
        if (sa->sa_family == AF_INET) {
            return &(((struct sockaddr_in*)sa)->sin_addr);
        }
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }

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

    void sendBytes(int nBytesToSend, BYTE *buf2) {
        const char* buf = reinterpret_cast<const char*>(buf2);
        int remaining = nBytesToSend;
        while (remaining > 0) {
            int result = send(this->sockfd, buf2, strlen(buf)+1, 0);

            if (result == -1)
                perror("send");

            else if (result < remaining) {
                remaining -= result;

                // deslocar o ponteiro de result bytes
                buf += result;
            }
        }
    }
    void receiveBytes(int nBytesToReceive, BYTE *buf2) {
        const char* buf = reinterpret_cast<const char*>(buf2);
        int remaining = nBytesToReceive;
        while (remaining > 0) {
            int result = recv(this->sockfd, buf2, MAXDATASIZE, 0);

            if (result == -1)
                perror("recv");

            else if (result < remaining) {
                remaining -= result;

                // deslocar o ponteiro de result bytes
                buf += result;
            }
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