
// prodcon.c
// Deven Schwartz
// 10/7/2021
// Project 2: Synchronization: Single Producer/ Single Consumer Bounded Buffer 
// 
// Description: Program is using semaphores to add up a set amount of numbers.
// Flow: Adds first 1000 digits using shared memory and semaphores
// References: Examples on website.
// Assistance: Dr. Bracken
//
//Testing: I have thoroughly tested my code and have errors that I have not fixed.   
//1.) Not completly sure that it is 100% correctly working but answer is correct and it logically should be right 
//but it looks like it is still only going from one to the other everytime
//2.) I did not return anything from the wait and signal calls not sure if that what you wanted for a retun on system calls
//
//Time: ~ 6 hours.


//include files
#include "sys/types.h"
#include "sys/ipc.h"
#include <sys/errno.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "wait.h"
#include "unistd.h"


void p(int semid){//wait
    struct sembuf p_buf;
    p_buf.sem_num = 0;
    p_buf.sem_op = -1;
    p_buf.sem_flg = 0;

    //check if call fails
    if (semop(semid,&p_buf,1) < 0)
    {
        printf("fatal p error on semaphore %i\n",semid);
        fflush(stdout);
    }
} //end p


void v(int semid){//signl
    struct sembuf v_buf;
    v_buf.sem_num = 0;
    v_buf.sem_op = 1;
    v_buf.sem_flg = 0;

    //printf("in signal: \n");
    //fflush(stdout);
    //check if call fails
    if(semop(semid,&v_buf, 1) < 0)
    {
        printf("fatal v error on semaphore %i\n",semid);
        fflush(stdout);
    }
} //end v



int main(){

    //declare the shared memory and the semaphore 
    #define SEMPERM 0600 /* permission for semaphore */
    int sem_id;
    sem_id = semget(IPC_PRIVATE,1,SEMPERM|IPC_CREAT|IPC_EXCL);
    int sem_id2;
    sem_id2 = semget(IPC_PRIVATE,1,SEMPERM|IPC_CREAT|IPC_EXCL); 
    
    #define SHMPERM 0600
    int segid;
    segid= shmget (IPC_PRIVATE,sizeof(int)*10,IPC_CREAT|IPC_EXCL|SHMPERM); 

    //statloc for wait
    int statloc;

    //union for the semaphores
    union {
        int val;
        struct semid_ds *buffer;
        ushort *array;
    }semnum1, semnum2;

    //set size for buffer
    //min val of 0
    semnum1.val = 0;
    //set buffer both semaphores
    semctl(sem_id,0,SETVAL, semnum1); 
    //cap sem set to 10
    semnum2.val = 10;
    semctl(sem_id2,0,SETVAL, semnum2); 

    //set up buffer to hold nums
    int * buffer;
    buffer = (int *)shmat (segid, (int *)0,0);
  
    //total count to keep track of final answer
    int total = 0;

    int retval;
    //fork program
    retval=fork();

    //bad fork check
    if (retval<0)
    {
        //printf("am i in here fail?\n");
        return(-2); /*fork failed*/
    }
    //child process
    if(retval==0) /*child */ // aka consumer

    {

        //print out child pid
        int c_pid = getpid();
        printf("I am child process my pid is %i.\n", c_pid);
        fflush(stdout);
       
        //child loop for adding values
        for(int i = 1; i <= 1000; i++){
            //wait for sem 1
            p(sem_id);
            //printf("i in child?\n");
            //fflush(stdout);

            //add the correct number from the buffer(use %10 to only use first 10 spots of buffer)
            total = total + buffer[i%10];
            //printf("i in child adding %i?\n", buffer[i%10]);
            //fflush(stdout);
            

            //signal sem 2
            v(sem_id2);
        }
        //print final sum of nums (500500)
        printf("\nTotal out of loop: %i\n", total);
        fflush(stdout);


    }else{ // parent aka producer

        //print parent pid
        int p_pid = getpid();
        //printf("I am parent process my pid is %i.\n", p_pid);
        //fflush(stdout);

        //loop to add ints to buffer
        for(int i = 1; i <= 1000; i++){
            //wait for sem2 starts as 10
            p(sem_id2);
            //printf("am i in here?\n");
            //fflush(stdout);

            //add a number to the buffer(%10 to only use first 10 (0-9) of buffer)
            buffer[i%10] = i;
            printf("i in parent adding to %i?\n", i%10);
            //printf("i in parent adding %i?\n", i);
            fflush(stdout);

            //printf("parent\n");
            //fflush(stdout);
            //signal sem1 starts at 0
            v(sem_id);

        }
        
        //print that loop has ended 
        printf("Parent is done with the loop and waiting for child to terminate.\n ");
        fflush(stdout);

        //wait for child to terminate
        wait(&statloc);


    }

        //printf("outside parent\n");
        //fflush(stdout);
        //remove shared memory and sems
        semctl(sem_id,0,IPC_RMID,NULL); 
        semctl(sem_id2,0,IPC_RMID,NULL);
        shmctl(segid,IPC_RMID,NULL); 
        

}





