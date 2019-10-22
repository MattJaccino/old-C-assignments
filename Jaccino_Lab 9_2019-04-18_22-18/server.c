#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <dirent.h>
#include <time.h>
#include "data.h"
//_TMPL_ CUT
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

int compareLetters(const void* a, const void * b){
  char first = *((char*)a);
  if(first >= 'A' && first <= 'Z'){
    first += 32;
  }
  char second = *((char*)b);
  if(second >= 'A' && second <= 'Z'){
    second += 32;
  }
  if(first < second){
    return -1;
  }else if(first == second){
    return 0;
  }else{
    return 1;
  }
}

int wc(char* string){
    int i=0;
    int count=0;
    while(i < strlen(string)){
        if(isspace(string[i])){
            count++;
        }
        i++;
    }
    return count;
}

//_TMPL_ END

int main(int argc,char* argv[]) 
{
    //_TMPL_ CUT
    unsigned int port = DEFAULT_PORT;

    if (argc >= 2)
        port = atoi(argv[1]);

    if (port == 0 || port > 65535) {
        fprintf(stderr, "Error: invalid port number %u.\n", port);
        exit(1);
    }

    fprintf(stderr, "Using port %u.\n", port);

    // Create a socket
    int sid = socket(PF_INET,SOCK_STREAM,0);
    CHECK_STATUS(sid);

    // setup our address and port
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    //pairs the newly created socket with the requested address.
    int status = bind(sid,(struct sockaddr*)&addr,sizeof(addr));
    CHECK_STATUS(status);

    // listen on that socket for "Let's talk" message. No more than 10 pending at once
    status = listen(sid, 70);
    CHECK_STATUS(status);

    // The typical while loop in a server.
    // You can revise the loop from an example or your pevious work.
    // It is fine to print out some messages, as in examples.
    // TODO
    //_TMPL_ CUT

    while(1) {
        struct sockaddr_in client;
        socklen_t clientSize = sizeof (client);

        int chatSocket = accept(sid,(struct sockaddr*)&client,&clientSize);
        CHECK_STATUS(chatSocket);

        fprintf(stderr, "We accepted a socket: %d\n",chatSocket);

        pid_t child = fork();
        if (child == 0) {
            int sum = 0;
            char buf[MAX_LINELEN + 1];
            int len, done;

            close(sid);

            done = 0;
            while (! done) {
                char *reply = "";

                // read a line from the socket
                len = socket_gets(buf, sizeof(buf), chatSocket);
                CHECK_STATUS(len); // take acare of len < 0

                if (! len) {    // len == 0
                    break;
                }
                else if (buf[len-1] != '\n') {
                    reply = "Error LongLine \n";
		            memcpy(buf, reply, strlen(reply) + 1);	   
		            len = strlen(reply);
                }
                else if (! strcmp(buf, "exit\n")) {
                    reply = "Bye \n";
		            memcpy(buf, reply, strlen(reply) + 1);
		            len = strlen(reply);
                    done = 1;
                }
                else {
                    int wcount = wc(buf);
                    //fprintf(stderr, "About to sort %d words\n", wcount);
                    int words_done = 0;
                    int c = 0;
                    char delim[] = " \n";
                    char* token = strtok(buf, delim);
                    char newbuf[MAX_LINELEN+1];
                    while(token != NULL){
                        //fprintf(stderr, "token: %s\n", token);
                        qsort(token, strlen(token), sizeof(char), compareLetters);
                        words_done++;
                        //fprintf(stderr, "token sorted: %s\n", token);
                        strcpy(&newbuf[c], token);
                        c += strlen(token) + 1;
                        //newbuf[c] = ' ';
                        if(words_done == wcount){
                            strcat(newbuf, "\n");
                        }
                        else{
                            strcat(newbuf, " ");
                        }
                        token = strtok(NULL, delim);
                        //fprintf(stderr, "Current buf: %s\n", newbuf);
                    }
                    newbuf[c] = '\n';
                    strcpy(buf, newbuf);
    		        //fprintf(stderr, "Done sorting: %s\n", newbuf);
                }
                    len = socket_send(chatSocket, buf, len);
                    CHECK_STATUS(len);
                
            }
            close(chatSocket);
            exit(0); 
        } else if (child > 0) {
            fprintf(stderr, "Created a child: %d\n", child);
            close(chatSocket);		

            int status = 0;
            pid_t deadChild; // reap the dead children (as long as we find some)
            do {
                deadChild = waitpid(-1, &status, WNOHANG);
                CHECK_STATUS(deadChild);
                if (deadChild > 0)
                    fprintf(stderr, "Reaped %d\n",deadChild);
            } while (deadChild > 0); 
        } 
    }

    //_TMPL_ END
    return 0;
}
