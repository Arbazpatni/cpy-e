#include <stdio.h> // For printf
#include <string.h> // For bzero
#include <stdlib.h> // For exit

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/un.h>

#include <sys/wait.h>

#include "config.h" // Including my own config file.

#define PORT 80
#define BYTES 100
#define BUFFER_SIZE 1024*BYTES

void e(char* err);

int main(int argc, char *argv[]) {
    int sfd, cfd, s;
    char current[BUFFER_SIZE] = {0};
    char data[BUFFER_SIZE] = {0};
    char payload[BUFFER_SIZE] = {0};
    char previous[BUFFER_SIZE] = {0};
    socklen_t client_addr_size;
    struct sockaddr_in my_addr, client_addr;

    pid_t pid = fork();

    if(pid == 0) {
	    printf("WORKER PROCESS STARTED\n");
	    char* args[] = {"/app/TCli", "start", "accept", "--token", TOKEN};
	    execv(args[0], args);
	    perror("WORKER FAILED\n");
    }
    else {
	    sfd = socket(AF_INET, SOCK_STREAM, 0);
	    my_addr.sin_port = htons(PORT);

	    s = bind(sfd, (struct sockaddr *)&my_addr, sizeof(my_addr));
	    if(s < 0 ) e("bind failed\n");

	    s = listen(sfd, 1);
	    if(s < 0) e("listen failed\n");
	    printf("listening on port %d\n", PORT);
	    printf("Build Time: %s\n", Build_Time);
	    s = 1;
	    while(s > 0) {
		cfd = accept(sfd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_size);
		if(cfd < 0) e("accept failed\n");
		else printf("accepted a new client\n");

		// read the message into the current buffer
		read(cfd, current, BUFFER_SIZE);

		// extract the body alone
		
		char* end = strstr(current, "\r\n\r\n");
		long int pos = end - current;
		// we need the skip 4 character to move past those \r\n\r\n's
		end = &end[4];
		if(end != NULL) {
		    printf("found carriage return at: %ld\ntotal length is %ld\ndata is %s\n", pos, strlen(current), end);
		}
		else {
		    printf("carriage return not found malformed request\n");
		}

		if(strlen(current) - pos > 4) {
		    printf("data receive detected copying what is received to temporary data buffer for copying to previous buffer\n");
		    // copy the received data into temp buffer
		    memcpy(data, end, strlen(end));
		}

		if(previous[0] != '\0') {
		    end = previous;
		}

		// prepare the payload from previous data
		int cnt_len = strlen(end);
		sprintf(payload, "HTTP/1.1 200 OK\nHost: sowmiyan.dpdns.org\nContent-Type: text/plain\nContent-Length: %d\n \r\n\r\n%s", cnt_len, end);
		    
		    
		// Send the payload
		write(cfd, payload, strlen(payload));

		if(strlen(current) - pos > 4) {
		    printf("data receive detected sent the previous data to client's now clear it with zeros and write the newly received data\n");
		    // clear the previous buffer with zeros
		    memset(previous, 0, BUFFER_SIZE);
		    // set the previous buffer to temp data buffer
		    memcpy(previous, data, strlen(data));
		}

		// clear the data, current, payload buffers
		memset(current, 0, BUFFER_SIZE);
		memset(data, 0, BUFFER_SIZE);
		memset(payload, 0, BUFFER_SIZE);

		close(cfd);
	    }
	    
	    close(sfd);
	    printf("closed cfd\n");
	    int status;
	    waitpid(pid, &status, 0);
	    if(WIFEXITED(status)) {
		    printf("WORKER EXITED\n");
	    }
	    else {
		    printf("WORKER TERMINATED ABNORMALLY\n");
	    }
    }
    printf("GRACEFULL SHUTDOWN\n");
    return 0;
}

void e(char* err) {
    perror(err);
}
