#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h> // for errno

const int DATACNT_MIN = 5;
const int DATACNT_MAX = 20;
const int BUFFER_MIN = 5;
const int BUFFER_MAX = 20;
const int THREAD_CNT = 5;

int DATACNT = 0;
int BUFFER_SIZE = 0;
int *buffer_ptr = NULL;

sem_t empty;
sem_t full;

int front = 0;
int rear = 0;

int totalIn = 0;
int totalOut = 0;

typedef struct{
    int threadId;
    // pthread_mutex_t *lock;
} myarg_t;

pthread_mutex_t lock;
void getParams(int argc, char **argv){

    if(argc != 5){
        printf("Usage: ./prod-cons -i <maxValue> -b <sizeValue>\n");
        exit(0);
    }


    char buffer_Size[256];
    char dataCount[256];
    strcpy(dataCount,  argv[2]);
    strcpy(buffer_Size,  argv[4]);

    //Checking for invalid characters from input 
    for (int i = 0; i < strlen(dataCount); i++){
        if(dataCount[i] < 48 || dataCount[i] > 57){
            printf("Error, invalid data count value.\n");
            exit(0);
        }
    }
    for (int i = 0; i < strlen(buffer_Size); i++){
        if(buffer_Size[i] < 48 || buffer_Size[i] > 57){
            printf("Error, invalid buffer size value.\n");
            exit(0);
        }
    }

    int dataCnt = atoi(dataCount); 
    int bufferSize = atoi(buffer_Size);

    if(dataCnt == 0){
        printf("Error, invalid data count value.\n");
        exit(0);
    }
    if(bufferSize == 0){
        printf("Error, invalid buffer size value.\n");
        exit(0);
    }


    if(strcmp(argv[1], "-i")){
        printf("Error, invalid data count specifier.\n");
        exit(0);
    } 
    if(strcmp(argv[3], "-b")){
        printf("Error, invalid buffer size specifier.\n");
        exit(0);
    }
    if(dataCnt < DATACNT_MIN || dataCnt > DATACNT_MAX){
        printf("Error, data count out of range\n");
        exit(0);
    } else{
        DATACNT = dataCnt; 
    }

    if(bufferSize < BUFFER_MIN || bufferSize > BUFFER_MAX){
        printf("Error, invalid buffer size.\n");
        exit(0);
    }else{
        BUFFER_SIZE = bufferSize;
    }
}

void enqueue(int input, pthread_mutex_t lock, int threadId){    
        buffer_ptr[rear] = input;
        printf("\033[0;31mProducer %d: Insert Item %d at %d\033[0m\n", threadId, input, rear);
        rear = (rear + 1) % BUFFER_SIZE;
}

int dequeue(pthread_mutex_t lock, int threadId){
    
        int removedItem = buffer_ptr[front];
        printf("\033[0;92mConsumer %d: Remove Item %d at %d\033[0m\n", threadId, removedItem, front);
        front = (front + 1) % BUFFER_SIZE;
        return removedItem;

}

void *producer(void *arg){
    
    myarg_t *args = (myarg_t *) arg;
    int threadId = args->threadId;
    int randomNum = 0;

    for(int i = 0; i < DATACNT; i++){
        usleep(rand() % 5000);
        randomNum = (rand() % 5000);
        sem_wait(&empty);
            pthread_mutex_lock(&lock);
            enqueue(randomNum, lock, threadId);
            totalIn += randomNum;
            pthread_mutex_unlock(&lock);
        sem_post(&full);
    }
    return NULL;
}
void *consumer(void *arg){
    myarg_t *args = (myarg_t *) arg;
    int threadId = args->threadId;

    int dequedNum = 0;
    for(int i = 0; i < DATACNT; i++){
        sem_wait(&full);
            pthread_mutex_lock(&lock);
            totalOut += dequeue(lock, threadId);
            pthread_mutex_unlock(&lock);
        sem_post(&empty);
        usleep(rand() % 5000);
    }
    return NULL;
}
int main(int argc, char **argv){

    getParams(argc, argv);
    printf("CS 370 Project #5B - Producer-Consumer Project\n-------------------------------------------\n");
    buffer_ptr = (int*)malloc(BUFFER_SIZE * sizeof(int));
    pthread_mutex_init(&lock, NULL);
    pthread_t *producers = (pthread_t *)malloc(THREAD_CNT * sizeof(pthread_t));
    pthread_t *consumers = (pthread_t *)malloc(THREAD_CNT * sizeof(pthread_t));
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    myarg_t p_args = {0};
    myarg_t c_args = {0};

    for (int i = 0; i < THREAD_CNT; i++){

        myarg_t *args = (myarg_t*)malloc(sizeof(myarg_t));
        args->threadId = i;
        
        if(pthread_create(&producers[i], NULL, producer, args)){
            printf("Error creating thread number %d \n", i);
            exit(-1);
        }
    }

    for (int i = 0; i < THREAD_CNT; i++){

        myarg_t *args = (myarg_t*)malloc(sizeof(myarg_t));
        args->threadId = i;

        if(pthread_create(&consumers[i], NULL, consumer, args)){
            printf("Error creating thread number %d \n", i);
            exit(-1);
        }
        
    }

    for (int i = 0; i < THREAD_CNT; i++){
        if(pthread_join(producers[i], NULL) || pthread_join(consumers[i], NULL)){
            printf("Error joining thread %d \n", i);
        }
    }

    free(producers);
    free(consumers);

    printf("\nInput Total Sum: %d\n", totalIn);
    printf("Output Total Sum: %d\n", totalOut);
    printf("\nProject Done\n");

    return 0;
}