#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

const unsigned long MIN_THREADS = 1;
const unsigned long MAX_THREADS = 30;
const unsigned long MIN_LIMIT = 100;
const unsigned long BLOCK = 5000;
unsigned long       currCheck = 1;
unsigned long       userLimit = 0;
unsigned long       duckNumberCount = 0;
unsigned int        threadCount = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int getParams(int argc, char **argv){

    if(argc != 5){
        printf("Usage: ./duckNums -th <threadCount> -lm <limitValue>\n");
        exit(0);
    }


    char limit[256];
    char thread[256];
    strcpy(thread,  argv[2]);
    strcpy(limit,  argv[4]);
    unsigned long limitVal = atol(limit); 
    int threadCnt = atoi(thread);

    if(threadCnt == 0){
        printf("Error, invalid thread count value\n");
        exit(0);
    }
    if(limitVal == 0){
        printf("Error, invalid limit value\n");
        exit(0);
    }
    for (int i = 0; i < strlen(limit); i++){
        if(limit[i] < 48 || limit[i] > 57){
            printf("Error, invalid limit value.\n");
            exit(0);
        }
    }
    for (int i = 0; i < strlen(thread); i++){
        if(thread[i] < 48 || thread[i] > 57){
            printf("Error, invalid thread count value.\n");
            exit(0);
        }
    }

    if(strcmp(argv[1], "-th")){
        printf("Error, invalid thread count specifier.\n");
        exit(0);
    } 
    if(strcmp(argv[3], "-lm")){
        printf("Error, invalid limit value.\n");
        exit(0);
    }
    if(threadCnt < MIN_THREADS || threadCnt > MAX_THREADS){
        printf("Error, thread count out of range\n");
        exit(0);
    } else{
        threadCount = threadCnt; 
    }

    if(limitVal < MIN_LIMIT){
        printf("Error, invalid limit value.\n");
        exit(0);
    }else{
        userLimit = limitVal;
    }
    return 1;
}

int isDuckNum(unsigned long num){
    int firstZero = 0;
    while(num > 0){
        if(num % 10 == 0 && firstZero){
            // Number is not a duck num since there's more than one zero
            // Set firstZeroFound to zero to signify that num is not duck num
            firstZero = 0;
            return 0;
        }
        if(num % 10 == 0 && !firstZero){
            firstZero = 1;
        } 
        num /= 10;
    }
    return firstZero;
}


// ---------------------------------------------------------

// ORIGINAL COPY OF DUCKCNTTHREAD WITH PROPER MUTUAL EXCLUSION

void *duckCntThread(){
    unsigned long  currentBlock = 0;
    unsigned long  localDuckCnt = 0;
    // Obtain the next block of numbers to check
    // Update the global counter by the block size
    pthread_mutex_lock(&lock);
    currCheck += BLOCK;    
    currentBlock = currCheck;
    pthread_mutex_unlock(&lock);
    // Check if the number in a given block is within the userLimit
    unsigned long  num = currentBlock - BLOCK;
    while(num < userLimit){
        while(num < currentBlock && num < userLimit){
            // Check if number is a duck number
                // Increment local duck counter if yes
            if(isDuckNum(num)){
                localDuckCnt += 1;
            }
            // Increment num within currentBlock
            num++;
        }
        
        // Update Global Duck Counter and current number after finding all duck numbers within a block
        
        pthread_mutex_lock(&lock);

        duckNumberCount += localDuckCnt;
        if(currCheck <= userLimit){
            currCheck += BLOCK;
            currentBlock = currCheck;
            num = currentBlock - BLOCK;
        }
        else{
            currentBlock = currCheck;
            num = currentBlock;
        }
        
        pthread_mutex_unlock(&lock);
        
        localDuckCnt = 0;
    }
    return NULL;
}

// COPY OF DUCKCNTTHREAD WITHOUT MUTUAL EXCLUSION 

// void *duckCntThread(){
//     unsigned long  currentBlock = 0;
//     unsigned long  localDuckCnt = 0;
//     // Obtain the next block of numbers to check
//     // Update the global counter by the block size
//     // pthread_mutex_lock(&lock);
//     currCheck += BLOCK;    
//     currentBlock = currCheck;
//     // pthread_mutex_unlock(&lock);
//     // Check if the number in a given block is within the userLimit
//     unsigned long  num = currentBlock - BLOCK;
//     while(num < userLimit){
//         while(num < currentBlock && num < userLimit){
//             // Check if number is a duck number
//                 // Increment local duck counter if yes
//             if(isDuckNum(num)){
//                 localDuckCnt += 1;
//             }
//             // Increment num within currentBlock
//             num++;
//         }
        
//         // Update Global Duck Counter and current number after finding all duck numbers within a block
        
//         // pthread_mutex_lock(&lock);

//         duckNumberCount += localDuckCnt;
//         if(currCheck <= userLimit){
//             currCheck += BLOCK;
//             currentBlock = currCheck;
//             num = currentBlock - BLOCK;
//         }
//         else{
//             currentBlock = currCheck;
//             num = currentBlock;
//         }
        
//         // pthread_mutex_unlock(&lock);
        
//         localDuckCnt = 0;
//     }
//     return NULL;
// }


// ---------------------------------------------------------


// COPY OF DUCKCNTTHREAD WITH IMPROPER MUTUAL EXCLUSION

// void *duckCntThread(){
//     unsigned long  currentBlock = 0;
//     // unsigned long  localDuckCnt = 0;
//     // Obtain the next block of numbers to check
//     // Update the global counter by the block size
//     pthread_mutex_lock(&lock);
//     currCheck += BLOCK;    
//     currentBlock = currCheck;
//     pthread_mutex_unlock(&lock);
//     // Check if the number in a given block is within the userLimit
//     unsigned long  num = currentBlock - BLOCK;
//     while(num < userLimit){
//         while(num < currentBlock && num < userLimit){
//             // Check if number is a duck number
//                 // Increment local duck counter if yes
//             if(isDuckNum(num)){
//                 pthread_mutex_lock(&lock);
//                 duckNumberCount += 1;
//                 pthread_mutex_unlock(&lock);
//             }
//             // Increment num within currentBlock
//             num++;
//         }
        
//         // Update Global Duck Counter and current number after finding all duck numbers within a block
        
//         pthread_mutex_lock(&lock);

//         // duckNumberCount += localDuckCnt;
//         if(currCheck <= userLimit){
//             currCheck += BLOCK;
//             currentBlock = currCheck;
//             num = currentBlock - BLOCK;
//         }
//         else{
//             currentBlock = currCheck;
//             num = currentBlock;
//         }
        
//         pthread_mutex_unlock(&lock);
        
//         // localDuckCnt = 0;
//     }
//     return NULL;
// }

int main(int argc, char **argv){
    getParams(argc, argv);

    // Creating threadCount amount of threads
    pthread_t thread[threadCount];

    for (int i = 0; i < threadCount; i++){
        if(pthread_create(&thread[i], NULL, duckCntThread, NULL)){
            printf("Error creating thread number %d \n", i);
            exit(-1);
        }
    }

    for (int i = 0; i < threadCount; i++){
        if (pthread_join(thread[i], NULL)){
            printf("Error joining thread %d \n", i);
        }
    }
    printf("Number of Duck Counts: %ld\n", duckNumberCount);

    return 0;
}