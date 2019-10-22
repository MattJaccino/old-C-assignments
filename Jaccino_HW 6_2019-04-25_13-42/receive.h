/* fd: socket id
 * p: buffer to place received bytes into 
 * nbytes: number of bytes to read from the socket
 *
 * return the number of bytes received.
 * To be clear: Unlike in the lab, you shouldn't stop reading if you encounter a newline!
 */
int socket_recvall(int fd, void *p, int nbytes)
{
    int received = 0;

    while (received < nbytes) {

       int r = recv(fd, p + received, nbytes - received, 0);
       if(r<0){
           return -1;
       }
       //printf("r: %d\n", r);
       received += r;
    }
    //printf("recvd: %d\n", received);
    return received;
}

/*
 * Receives a file over a socket. Saves it as a new file of the form
 *      <fname>.<ext>
 * 
 * @param sid - the socket ID to receive the file on
 * @param fname - string name of the file name to send
 * @param ext - string file extension (i.e. .txt/.pdf/.c/.h/etc)
 * @param fSize - the file size to receive in bytes
 */
int     receiveFileOverSocket(int sid, char* fname, char* ext, int fSize)
{
    
    if (strlen(fname) + strlen(ext) > MAX_FILENAME_LEN)
        return -1;

    char* buf = malloc(sizeof(char)*fSize);
    if (buf == NULL)
        return -1;
    /* 
       call a function to recieve the file contents over the socket
    */
    int rv = socket_recvall(sid, buf, fSize);
    
    if (rv < 0) {
        free(buf);
        return rv;
    }
    
    char* file_n_ext = strcat(fname, ext);
    FILE* fd = fopen(file_n_ext, "w");
    if(fd == NULL){
        free(buf);
        return -1;
    }
    //int written = 
    fwrite(buf, sizeof(char), fSize, fd);
    //printf("written?\n");
     
    fclose(fd); 
    free(buf); 
    return 0;
}
