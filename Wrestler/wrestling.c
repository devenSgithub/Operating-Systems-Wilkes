
//wrestling.c
//
// Deven Schwartz
// 11/9/2021
// Project 4: Arm Wrestler: semaphores, shared memory, and chance
// Assigment 4: Program creates multiple patrons and a wrestler and sees who will win in runs
//there is a case that causes starvation however during the day that i worked on this i could not 
//see the solution that does not cause this with the semaphores that i used


//include files
#include "sys/types.h"
#include "sys/ipc.h"
#include <sys/errno.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include "wait.h"
#include "unistd.h"
#include <stdlib.h>





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

    //constants to be used 
    #define TOTAL_COMPETITIONS 20
    #define WAITING_ROOM_CAPICITY 3

    //declare the shared memory and the semaphore 
    #define SEMPERM 0600 /* permission for semaphore */
    int sem_id;
    sem_id = semget(IPC_PRIVATE,1,SEMPERM|IPC_CREAT|IPC_EXCL);
    int sem_id2;
    sem_id2 = semget(IPC_PRIVATE,1,SEMPERM|IPC_CREAT|IPC_EXCL); 
    int sem_id3;
    sem_id3 = semget(IPC_PRIVATE,1,SEMPERM|IPC_CREAT|IPC_EXCL); 
    int sem_id4;
    sem_id4 = semget(IPC_PRIVATE,1,SEMPERM|IPC_CREAT|IPC_EXCL); 
    int sem_id5;
    sem_id5 = semget(IPC_PRIVATE,1,SEMPERM|IPC_CREAT|IPC_EXCL); 
    
    #define SHMPERM 0600
    /*
    int segid;
    segid= shmget (IPC_PRIVATE,sizeof(int)*10,IPC_CREAT|IPC_EXCL|SHMPERM); 
     int segid2;
    segid2= shmget (IPC_PRIVATE,sizeof(int)*10,IPC_CREAT|IPC_EXCL|SHMPERM); 
    int segid3;
    segid3= shmget (IPC_PRIVATE,sizeof(int)*10,IPC_CREAT|IPC_EXCL|SHMPERM); 
    int segid4;
    segid4= shmget (IPC_PRIVATE,sizeof(int)*10,IPC_CREAT|IPC_EXCL|SHMPERM); 
    int segid5;
    segid5= shmget (IPC_PRIVATE,sizeof(int)*10,IPC_CREAT|IPC_EXCL|SHMPERM); 
*/


    int segidall;
    segidall= shmget (IPC_PRIVATE,sizeof(int)*10,IPC_CREAT|IPC_EXCL|SHMPERM); 

    //@@shared memory (change to one with indexed values if time)@@
    int * matches_remaining;
    int * closed_stat;
    int * num_waiting;
    int * Wwin;
    int * Pwin;

    int * Shared_Data;

    Shared_Data = (int *)shmat (segidall, (int *)0,0);
    Shared_Data[1] = 0;//matches remaining
    Shared_Data[2] = 0;//num waiting
    Shared_Data[3] = 0;//patron wins
    Shared_Data[4] = 0;//wrestler wins
    Shared_Data[5] = 0;//closed status

/*
    matches_remaining = (int *)shmat (segid, (int *)0,0);
    closed_stat = (int *)shmat (segid2, (int *)0,0);
    num_waiting = (int *)shmat (segid3, (int *)0,0);
    Wwin = (int *)shmat (segid4, (int *)0,0);
    Pwin = (int *)shmat (segid5, (int *)0,0);
    //initilize all values to 0
    matches_remaining[1] = 0;
    closed_stat[1] = 0;
    num_waiting[1] = 0;
    Wwin[1] = 0;
    Pwin[1] = 0;
*/
    //statloc for wait
    int statloc;

    //union for the semaphores
    union {
        int val;
        struct semid_ds *buffer;
        ushort *array;
    }wakewres, admitpat, wrestle, mutex, syncr;



    //(barber/arm wrestler) start at 0
    wakewres.val = 0;
    semctl(sem_id,0,SETVAL, wakewres); 

    //(custumer/patron) start at 0 
    admitpat.val = 0;
    semctl(sem_id2,0,SETVAL, admitpat);

    //(cs for wrestling) start at 1 
    //its decrease to one is in sysc with the increase of syncr semaphore
    wrestle.val = 1;
    semctl(sem_id3,0,SETVAL, wrestle); 

    //mutex start at 1
    mutex.val = 1;
    semctl(sem_id4,0,SETVAL, mutex); 

    //semaphore used for the syncronization of the wrestling start at 0
    //its increase to one is in sysc with the decrement of wrestle
    syncr.val = 0;
    semctl(sem_id5,0,SETVAL, syncr); 
    //id 3 and id 5 note: the idea is that as one is taken the other is released
    // in the process so that both are in the wrestle cs at the same time


    //get the random seed for random nums later
    srand (getpid()); 


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
    if(retval==0) /*child */ // aka wrestler
    {



        //print out child pid
        int c_pid = getpid();
        printf("I am wrestler process my pid is %i.\n", c_pid);
        fflush(stdout);


        while(Shared_Data[1] <= TOTAL_COMPETITIONS -1){

            
            //sleep while no patrons
            //ids:
            //wakewres(id1) admitpat(id2) wrestle(id3) mutex(id4) syncr(id5) p(wait) v(signal)

            //sleep until a patron signals
            p(sem_id);

            //take the mutex
            p(sem_id4);

            //signal the patron/admit
            v(sem_id2);

            //decrement the number waiting (still in mutex lock)
            Shared_Data[2]--;

           // printf("wait dec = %i\n",num_waiting[1]);
            //fflush(stdout);

            //signal mutex to release
            v(sem_id4);

            //wait on the wrestling sync (sets up lock on this end so that both will wrestle at same time)
            p(sem_id5);

            //get a random number
            int val = rand();

            //print the random number 
            printf("The random number of the match is: %i.\n", val);
            fflush(stdout);

            //lock mutex to update the winnings and matches remaining count 
            p(sem_id4);
            //add one to remaing count 
            Shared_Data[1] = Shared_Data[1] + 1;
            
            //figure out who won and update there count and print the message of who won
            if(val % 4 == 0){
                Shared_Data[3] = Shared_Data[3] + 1;
                printf("Patron has won this round with a result of %i.\n",val%4);
                fflush(stdout);
                printf("The patron has won $25. In total patrons have won $%i.\n", Shared_Data[3]*25);
                fflush(stdout);
                printf("The patrons have won %i rounds so far.\n", Shared_Data[3]);
                fflush(stdout);
            }else{
                Shared_Data[4] = Shared_Data[4] + 1;
                printf("Arm Wrestler has won this round with a result of %i.\n",val%4);
                fflush(stdout);
                printf("The Arm Wrestler has won $10. In total the Arm Wrestler has won $%i.\n", Shared_Data[4]*10);
                fflush(stdout);
                printf("The Arm Wrestler has won %i rounds so far.\n", Shared_Data[4]);
                fflush(stdout);
            }
            //signal mutex to open it 
            v(sem_id4);


            //signal the sync for the wrestling (eo wres)
            //opens the wrestling in patrons both can now exit the wrestle portion
            v(sem_id3);
            fflush(stdout);
           

            printf("The match has ended, the Arm Wrestler will come back soon!\n");
            fflush(stdout);


        }


        //Set closed status
        printf("Sorry the Arm Wrestler has closed. Have a good day!\n");
        fflush(stdout);
        //open mutex to edit values
        p(sem_id4);
        //set closed to true
        Shared_Data[5] = 1;
        //clear the number in waiting
        Shared_Data[2] = 0;
        
        //done changing values reopen mutex
        v(sem_id4);

        //Wake up patrons in the waiting room 
        for(int i = 0; i<WAITING_ROOM_CAPICITY; i++){
            //signal anyone who may be left to have them leave
            v(sem_id2);
        }
        //terminate
       
        printf("\nWrestler has went home. Goodbye!\n\n");
        fflush(stdout);

        //exit the process once done just in case it decides not to end on own this forces it to
        exit(1);
        //break;


    }else{ // parent aka patron

        //the loop to create the patron processes 
        for(int i = 0; i< 5; i++){
            
            printf("\tCreating a child (patron)\n");
            fflush(stdout);

            //fork each time 
            retval=fork();

            //check for a bad fork 
             if (retval<0){
                printf("---fork fail---\n");
                fflush(stdout);
                return(-2); /*fork failed*/
            }
            //otherwise take the process as a patron if 0
            if(retval==0){

                //testing
                int c_pid = getpid();
                printf("I am patron process who will attempt to enter the tent, my pid is %i.\n", c_pid);
                fflush(stdout);
        
                //testing prints
               // printf("\t retval number fork number %i      %i\n", retval, getpid());
               // fflush(stdout);

              //  int p_pid = getpid();
              //  printf("       ****      I am patron process my pid is %i.\n", p_pid);
              //  fflush(stdout);

                do{

                   // printf("   ^^^^^^^  ****   in the loop of patron  \n ");
                   // fflush(stdout);

                    //ids:
                    //wakewres(id1) admitpat(id2) wrestle(id3) mutex(id4) syncr(id5) p(wait) v(signal)

                    //take control of mutex with a wait
                    p(sem_id4);

                    //verify that the tent is still open and the waiting room is not full
                    if(Shared_Data[2] >= 3 || Shared_Data[5] == 1){
                        //if eiter is true release the mutex 
                        //next attempt at loop will kill process if closed so do not break out here
                        v(sem_id4);
                    }else{
                        //otherwise the patron can enter the waiting room and prepare to wrestle
                        //inc in mutex
                        Shared_Data[2]++;

                        //signal to wake the wrestler
                        v(sem_id);

                        //can release the mutex (allows for other to join)
                        v(sem_id4);
                        //wait on the admit
                        p(sem_id2);

                        //check again if it has closed since start of wait
                        if(Shared_Data[5] == 1){
                            //if it has since closed the process breaks out so that it can end
                            break;
                        }

                        //signal the wrestling semaphore to enter the wrestling on the patron side
                        p(sem_id3);
                        //now the wrestler and this patron are locked into a wrestling match
                        
                        //print id of the wrestling patron
                        int c_pid = getpid();
                        printf("\tI am patron process in the wrestle  my pid is %i.\n", c_pid);
                        fflush(stdout);

                        //signal the sync to get out of the wrestling
                        v(sem_id5);
                    }
                    //loop end at the the close of the tent
                }while(Shared_Data[5] != 1);
                
                
                //print that loop has ended 
                int pid = getpid();
                printf("Patron process (pid:%i) is done with the loop and terminating.\n ", pid);
                fflush(stdout);
                //break;
                //exit in case it needs it
                exit(0);
        
            }
                    
        }
        //wait at end of parent for any process left
        wait(&statloc);         

    }

    wait(&statloc);



    //wait for the child and parent process to end
     //wait for the process to end
    for(int i = 0; i <= 6; i++) {
        wait(&statloc);	
    }
    //wait(&statloc);
    //wait(&statloc);


        printf("\nFork has ended clearing the memory\n\n");
        fflush(stdout);
        //remove shared memory and sems
        semctl(sem_id,0,IPC_RMID,NULL); 
        semctl(sem_id2,0,IPC_RMID,NULL);
        semctl(sem_id3,0,IPC_RMID,NULL); 
        semctl(sem_id4,0,IPC_RMID,NULL);
        semctl(sem_id5,0,IPC_RMID,NULL);  
        


        shmctl(segidall,IPC_RMID,NULL);




/*
        shmctl(segid,IPC_RMID,NULL); 
        shmctl(segid2,IPC_RMID,NULL); 
        shmctl(segid3,IPC_RMID,NULL); 
        shmctl(segid4,IPC_RMID,NULL); 
        shmctl(segid5,IPC_RMID,NULL); 
  */      

}

