//projeto.hpp
#include "raspberry.hpp"
#include <unistd.h>

#include "./server.hpp"

void *SERVER::get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr)
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr)
}

void SERVER::SERVER() {
    memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP
	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
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

void SERVER::~SERVER() {
    close (this->new_fd);
}

	void SERVER::waitConnection() {
		printf("server: waiting for connections...\n");
	
		while (1) {
			sin_size = sizeof their_addr;
			new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);	
			if (new_fd == -1) {
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

	void SERVER::sendBytes(int nBytesToSend, BYTE *buf) {

	}

	void SERVER::receiveBytes(int nBytesToReceive, BYTE *buf) {

	}