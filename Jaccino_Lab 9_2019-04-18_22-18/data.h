#include <unistd.h>
#include <stdio.h>
/* similar to fgets()
 *
 * read from the socket until a newline is found.
 * the first bufsize - 1 characters will be saved.
 * Return the number of bytes placed in the buffer or -1.
 */
int socket_gets(char *p, int bufsize, int fd)
{
    int received = 0;
    int done = 0;
    char temp;
    //fprintf(stderr, "Getting s\n");
    while(! done){
        int r = read(fd, &temp, 1);
        if(r <= 0){
            if(received < bufsize - 1){
                p[received] = '\0';
            }else{
                p[bufsize - 1] = '\0';
            }
            return r;
        }
        if(received < bufsize - 1){
            p[received] = temp;
            received++;
        }
        if(temp == '\n'){
            done = 1;
            p[received] = '\0';
            //fprintf(stderr, "Received: %s\n", p);
            //fprintf(stderr, "Done\n");
        }
    }
    return received;
}

/* sid: socket id
 * message: location of the message
 * length: number of bytes to be sent
 *
 * return the number of bytes sent or -1 for error.
 */
int socket_send(int sid, char *message, int length)
{
    int sent = 0;
    //if(length > 71){
    //    return -1;
    //}
    while(sent < length){
        int s = send(sid, message + sent, length - sent, 0);
        if(s <= 0){
            return -1;
        }
        sent += s;
    }
    //fprintf(stderr, "Sent %d\n", sent);
    return sent;
}
