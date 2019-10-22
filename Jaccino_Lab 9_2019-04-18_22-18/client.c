#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <assert.h>
#include "data.h"
//_TMPL_ CUT
#define     DEFAULT_HOSTNAME    "localhost" 
#define     DEFAULT_PORT        3100
#define     MAX_LINELEN         70

#define     CHECK_STATUS(x)     checkStatus((x), __LINE__)

void checkStatus(int status, int line)
{
    if (status < 0) {
        fprintf(stderr, "%d:%d:", getpid(), line);
        perror("");
        exit(1);
    }
}


//_TMPL_ END 

int main(int argc,char* argv[]) 
{
    //_TMPL_ CUT
    char *hostname = DEFAULT_HOSTNAME;
    int port = DEFAULT_PORT;

    if (argc >= 2)
        hostname = argv[1];
    if (argc >= 3)
        port = atoi(argv[2]);

    if (port == 0 || port > 65535) {
        fprintf(stderr, "Error: invalid port number %u.\n", port);
        exit(1);
    }

    // fprintf(stderr, "Connecting to %s:%u.\n", hostname, port);

    // create socket 
    int sid = socket(PF_INET,SOCK_STREAM,0);
    int status;

    CHECK_STATUS(sid);

    // setup our address and port
    struct sockaddr_in srv;
    struct hostent *server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr, "Error: cannot find a host named '%s'\n",argv[1]);
        return 2;
    }

    srv.sin_family = AF_INET;
    srv.sin_port   = htons(port);
    memcpy(&srv.sin_addr, server->h_addr_list[0], server->h_length);

    //connect 
    status = connect(sid, (struct sockaddr*)&srv, sizeof(srv));
    CHECK_STATUS(status);
    char ch;
    int done = 0;
    char* buf = malloc(sizeof(char)*MAX_LINELEN);
    size_t len = 0;
    char* input;
    int buf_size = MAX_LINELEN;
    int i = 0;
    do {
      i = 0;
      while(1){
	input = fgets(&buf[i], MAX_LINELEN, stdin);
	if(input == NULL){
	  return 1;
	}else if(strlen(input) == MAX_LINELEN - 1 && input[MAX_LINELEN - 2] != '\n'){
	  i += MAX_LINELEN - 1;
	  if(i + MAX_LINELEN > buf_size){
	    buf_size += MAX_LINELEN - 1;
	    buf = realloc(buf, sizeof(char)*buf_size);
	  }
	}else{
	  break;
	}
      }
      len = strlen(buf);
      len = socket_send(sid, buf, len);
      CHECK_STATUS(len);
      
      len = socket_gets(buf, MAX_LINELEN, sid);
      CHECK_STATUS(len);
      if (len == 0) {
	printf("Connection lost.\n");
	break;
      }

      printf("%s", buf);
      if (!strncmp(buf, "Bye ", 4)) 
	break;
      
    } while (1);

    close(sid);
    //_TMPL_ END
    return 0;
}
