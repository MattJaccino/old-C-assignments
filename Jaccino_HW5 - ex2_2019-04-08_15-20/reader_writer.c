#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define MAX 50
#define NUM_READER_THREADS  MAX/2
#define NUM_WRITER_THREADS  MAX/2

#define READER 1
#define WRITER 2
#define EITHER 3

int     reader_in_db=0;        //whether reader is in db
int     writer_in_db=0;        //wehter writer is in db
int     who_is_next=EITHER;    //Who is next : READER, WRITER OR EITHER

int     reader_in_queue=0;    //number of readers in queue
int     writer_in_queue=0;    //number of writers in queue

struct log_t
{
    int id;                //id of the reader or writer
    int type;            //type of operation(READER or WRITER)
}db_log[MAX];            //log of all the db operations

int log_length = 0;        //length of the log


pthread_mutex_t db_mutex;    //mutex
pthread_cond_t db_idle;        //condition variable

//implement the reader
//Note we need to use the follow line to simualte the time spent in the system
//usleep(10000 + 10000*(my_id*100 % 3));
//The above line is cirtical for the inspection in the main() function
//Make sure it is not in a critical section (not locked by a mutex)
//Also we need to use the following line before the thread exit to match the output 
//printf("leaving db\n"); 

void * reader(void *t) 
{
    int my_id = *(int *)t;
    //fill in the code below
    reader_in_queue++;

    usleep(10000 + 10000*(my_id*100 % 3));
    // Lock
    pthread_mutex_lock(&db_mutex);
    

    while(writer_in_queue > 0 && who_is_next == WRITER){
        //printf("W? %d, R? %d, WIN: %d, reader queue is now %d\n", reader_in_db, writer_in_db, who_is_next, reader_in_queue);
        pthread_cond_wait(&db_idle, &db_mutex);
    }
    reader_in_db = 1;
    db_log[log_length].id = my_id;
    db_log[log_length].type = READER;
    log_length++;
    
    who_is_next = WRITER;
    reader_in_queue--;

    reader_in_db = 0;

    //printf("Reader %d is done. %d logged\n", my_id, readers_logged);

    pthread_mutex_unlock(&db_mutex);
    pthread_cond_broadcast(&db_idle);
    
    
    printf("leaving db\n");
    pthread_exit(NULL);
} 

//implement the writer
//Note we need to use the follow line to simualte the time spent in the system
//usleep(10000 + 10000*(my_id*100 % 3));
//The above line is cirtical for the inspection in the main() function
//Make sure it is not in a critical section (not locked by a mutex)
//Also we need to use the following line before the thread exit to match the output 
//printf("leaving db\n");              

void *writer(void *t) 
{
    int my_id = *(int *)t;
    //fill in the code below    
    writer_in_queue++;
    usleep(10000 + 10000*(my_id*100 % 3));
    
    // Lock 
    pthread_mutex_lock(&db_mutex);
    
    while(reader_in_queue > 0 && who_is_next == READER){
        //printf("W? %d, R? %d, WIN: %d writer queue is now %d\n", reader_in_db, writer_in_db, who_is_next, writer_in_queue);
        pthread_cond_wait(&db_idle, &db_mutex);
    }
    writer_in_db = 1;
    db_log[log_length].id = my_id;
    db_log[log_length].type = 2;
    log_length++;
    
    writer_in_db = 0;
    writer_in_queue--;
    
    who_is_next = READER;

    //printf("Writer %d is done. %d logged\n", my_id, writers_logged);

    // This part has to be right
    pthread_mutex_unlock(&db_mutex);
    pthread_cond_broadcast(&db_idle);


    printf("leaving db\n");
    pthread_exit(NULL);
}


//Do not change the main function
int main (int argc, char *argv[])
{
    if(argc != 3)
    {
        printf("Usage: %s n_readers n_writers\n", argv[0]);
        return -1;
    }
    int n_readers = atoi(argv[1]);
    int n_writers = atoi(argv[2]);
    assert(n_readers >= 0 && n_readers <= NUM_READER_THREADS);
    assert(n_writers >= 0 && n_writers <= NUM_WRITER_THREADS);    
    int i;
    int reader_ids[NUM_READER_THREADS];
    int writer_ids[NUM_WRITER_THREADS];
    pthread_t reader_threads[NUM_READER_THREADS];
    pthread_t writer_threads[NUM_WRITER_THREADS];
    
    pthread_attr_t attr;

    /* Initialize mutex and condition variable objects */
    pthread_mutex_init(&db_mutex, NULL);
    pthread_cond_init (&db_idle, NULL);
    

    /* For portability, explicitly create threads in a joinable state */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(i=0;i<n_readers;i++)
    {
        reader_ids[i]=i;    
        pthread_create(&reader_threads[i], &attr, reader, &reader_ids[i]);
    }
  
  
    for(i=0;i<n_writers;i++)
    {
        writer_ids[i]=i;    
        pthread_create(&writer_threads[i], &attr, writer, &writer_ids[i]);
    }
    /* Wait for all threads to complete */
    

    for (i=0; i<n_readers; i++) {
        pthread_join(reader_threads[i], NULL);
    }
    
    for (int j=0; j<n_writers; j++) {
        pthread_join(writer_threads[j], NULL);
    }
    
    
    printf ("main(): done waiting.\n");

    /* Clean up and exit */
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&db_mutex);
    pthread_cond_destroy(&db_idle);
    
    // print db_log
    for(int j=0; j<n_readers + n_writers; j++){
        //printf("db_log[%d]: type: %d\n", j, db_log[j].type);
    }

    //the following code is for the test case
    int min_n = n_readers > n_writers ? n_writers : n_readers;
    for(i= 1; i<2*min_n; i++)
    {
        //printf("db_log[%d]: type: %d\n", i-1, db_log[i-1].type);
        //printf("db_log[%d]: type: %d\n", i, db_log[i].type);
        if(db_log[i].type + db_log[i-1].type != 3)
        {
            printf("Incorrect result.\n");
            return(-1);
        } 
    }
    printf("Result passed inspection.\n");
    return 0;
} 

