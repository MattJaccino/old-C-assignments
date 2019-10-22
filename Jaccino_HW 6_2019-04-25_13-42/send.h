

/* sid: socket id
 * message: location of the message
 * length: number of bytes to be sent
 *
 * return the number of bytes sent or -1 for error.
 */
int socket_sendall(int sid, void *message, int length)
{
  /*
    Fill this in!
   */
   int sent = 0;
   while(sent < length){
       int s = send(sid, message + sent, length - sent, 0);
       if(s < 0){
           return -1;
       }
       //printf("s: %d\n", s);
       sent += s;
   }
   //printf("sent: %d\n", sent);
   return sent;
}

 
/*
 * Opens the file, reads it from memory, and sends it over the given chatsocket
 * 
 * @param fname - string name of the file name to send
 * @param chatSocket - the socket to send the file over.
 */
int     sendFileOverSocket (char* fName, int chatSocket)
{
    char buf[1024];

    int fileSize = getFileSize(fName);
    if (fileSize < 0)
        return -1;

    int nbread = 0, nbRem = fileSize;
    FILE* fp = fopen(fName,"r");
    if (fp == NULL)
        return -1;
    int rv = 0;
    //nbRem is the number of bytes remaining to be send
    while(nbRem != 0) {
        //printf("nbRem: %d\n", nbRem);
        //int rd = read(fp, &buf, 1024);
        nbread = fread(&buf, sizeof(char), 1024, fp);
        //int sending = nbRem < 1024 ? nbRem : 1024;
        rv = socket_sendall(chatSocket, buf, nbread);
        if(rv < 0){
            fclose(fp);
            return -1;
        }
        nbRem -= rv;

    }
    fclose(fp);
    return 0;
}

