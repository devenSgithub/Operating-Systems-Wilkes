
//prodconthc.c

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//global variables for buffer and prod/con counts
int consumerpos = 0;
int producerpos = 0;
int buffer[9];

//mutex for locks
pthread_mutex_t lockit=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t signalit=PTHREAD_COND_INITIALIZER;

//producer function
void produce(int thread_number){
    printf("I am the producer and my thread number is %i\n", thread_number);
    fflush(stdout);
    //var to create inf loop
    int a = 1;
    while (a == 1){
        //lock for mutex
        pthread_mutex_lock(&lockit);
        //check if cnt production is met..
        if(producerpos == 100){
            //..if so break
            printf("breaking out of p1\n");
            fflush(stdout);
            break;
        }
        //check next pos
        while (buffer[(producerpos + 1)%10] > -1){
            //wait if needed
            printf("in producer wait\n");
            fflush(stdout);
            pthread_cond_wait(&signalit,&lockit);
        }
        //after wait check if production happened again
        if(producerpos == 100){
            printf("breaking out of p2\n");
            fflush(stdout);
            //if so break out to not over produce
            break;   
        }

        printf("I am producer adding to buffer: %i thread number : %i\n", producerpos, thread_number);
        fflush(stdout);
        //add to buffer position
        buffer[producerpos%10] = producerpos;
        //increment production pos
        producerpos++;
        
        
        //broadcast production and unlock
        pthread_cond_broadcast(&signalit);
        pthread_mutex_unlock(&lockit);

    }//end while
    //if early break out unlock and broadcast/ message ending thread
    printf("producer exiting my id %i\n",thread_number);
    fflush(stdout);
    pthread_cond_broadcast(&signalit);
    pthread_mutex_unlock(&lockit);
    
}//end of producer


//consumer function
void consume(int thread_number){

    printf("I am the consumer and my thread number is %i\n", thread_number);
    fflush(stdout);

    //var to create inf loop
    int b = 1;
    while (b == 1){
        //lock for mutex 
        pthread_mutex_lock(&lockit);
        //check consume cnt
        if(consumerpos == 100){
            printf("breaking out of c1\n");
            fflush(stdout);
            //if above 99 break out of consume
            break;
        }
        
        //loop until next position filled  will work in place of if statement: && consumerpos < 100
        while (buffer[consumerpos%10] == -1 ){
            printf("In consumer wait\n");
            fflush(stdout);

            //break out if count gets over 99 (stop value)
            //checking for cnt over 99, && consumerpos < 100 will work in place of if
            if(consumerpos >= 100){
                //pthread_cond_broadcast(&signalit);
                break;
            }
            
            //wait until a broadcast allows it to move foward
            pthread_cond_wait(&signalit,&lockit);
        }
        //recheck for consume cnt (in case final consumed while in wait)
        if(consumerpos == 100){
            printf("breaking out of c2\n");
            fflush(stdout);
            //break out
            break;
        }
        printf("consumer: the data is:  %i  my thread num is: %i\n",buffer[consumerpos%10],thread_number);
        fflush(stdout);
        //replace buffer to neg again
        buffer[consumerpos%10] = -1;
        consumerpos++;
        
        //broadcast and unlock
        pthread_cond_broadcast(&signalit);
        pthread_mutex_unlock(&lockit);
    }//end while
    //unlock and broadcast if break out early / message leaving thread
    printf("consumer exiting my id %i\n",thread_number);
    fflush(stdout);
    pthread_cond_broadcast(&signalit);
    pthread_mutex_unlock(&lockit);
    
}



int main(){
    //initilize buffer as neg 1
    for (int i = 0; i<10; i++){
        buffer[i] = -1;
    }

    void *statusp;
    pthread_t *threads;
    //create 2 produced and 2 consumer threads
    if((threads=malloc(sizeof(pthread_t)*2))==NULL){
        printf("error allocating\n");
        fflush(stdout);
        exit(-1);
    }

    if((pthread_create(&threads[0],NULL,(void *)produce,(void *)0))!=0){
        printf("error creating producer\n");
        fflush(stdout);
        exit(-1);
    }
    if((pthread_create(&threads[2],NULL,(void *)produce,(void *)2))!=0){
        printf("error creating producer\n");
        fflush(stdout);
        exit(-1);
    }

    if(pthread_create(&threads[1],NULL,(void *)consume,(void *)1)!=0){
        printf("error creating consumer\n");
        fflush(stdout);
    }
    if(pthread_create(&threads[3],NULL,(void *)consume,(void *)3)!=0){
        printf("error creating consumer\n");
        fflush(stdout);
    }

    //join 4 threads
    if((pthread_join(threads[0],&statusp))!=0){
        printf("couldn't join with producer\n");
        fflush(stdout);
    }
    if((pthread_join(threads[2],&statusp))!=0){
        printf("couldn't join with producer\n");
        fflush(stdout);
    }

    if((pthread_join(threads[1],&statusp))!=0){
        printf("couldn't join with consumer\n");
        fflush(stdout);
    }
    if((pthread_join(threads[3],&statusp))!=0){
        printf("couldn't join with consumer\n");
        fflush(stdout);
    }

    if((pthread_mutex_destroy(&lockit))!=0){
        printf("mutex destroy failed\n");
        fflush(stdout);
    }
    if((pthread_cond_destroy(&signalit))!=0){
        printf("cond destroy failed\n");
        fflush(stdout);
    }
} 
