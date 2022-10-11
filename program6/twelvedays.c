//
//Deven Schwartz
//twelvedays.c
//12/7/21
//Unix Signals Programming Assignment
//12 days of christmas with signals 
//
//
//
//In the header comments of your program, provide an expression for the complexity of the number of total gifts given.
// 
// There are a constant number of days (12), it loops through n times giving n gifts each time so it would loop n times 
// so it would be O(n) time given it is 12 days that it is looping it would be c(for the number of days) * (n for the loop) 
// meaning it would be C * n






#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>


void sig_user (int signo);
void child(int prevchildpid, int currentchildnum);


/* please note the use of global variables. Global variables are 
 * very dangerous and should only be used when absolutely necessary.
 * this simple example does not justify the use of global variables.
 * It was, however, taken from a much more complicated program, 
 * complicated enough to justify the use of global variables.
 */
static sigset_t newmask, oldmask, zeromask;

#define DAYSOFCHRISTMAS 12

const char *gifts[DAYSOFCHRISTMAS]={

"Twelve Drummers Drumming",

"Eleven Pipers Piping",

"Ten Lords a Leaping",

"Nine Ladies Dancing",

"Eight Maids a Milking",

"Seven Swants a Swimming",

"Six Geese a Laying",

"Five Golden Rings",

"Four Calling Birds",

"Three French Hens",

"Two Turtle Doves",

"A Partridge in a Pear Tree"}; 

    //array for days
    const char *daysArray[DAYSOFCHRISTMAS]={"Twelfth", "eleventh", "tenth", "ninth", "eighth", "seventh", "sixth", "fifth", "fourth", "third", "second", "first"}; 

int main (void){

    int pid, status;
    
    /*  stuff for blocking signals */

    if (signal(SIGUSR1, sig_user) == SIG_ERR){
        printf("error creating signal handler \n");
        fflush(stdout);
    }

    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask,SIGUSR1);

    if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0){
        printf("error in sigprocmask \n");
        fflush(stdout);
    }

    //var for prevpid
    int previouspidnum = -1;
    for(int i = 0; i<DAYSOFCHRISTMAS; i++){

        pid = fork();
        if (pid < 0){
            printf("fork failed \n");
            fflush(stdout);
            exit (-1);
        }else{/*end if pid < 0 */
            if (pid == 0){

                //send prevpid and the child num
                child(previouspidnum, i);
                //printf("return from child\n");
                fflush(stdout);
                           
                exit(0); /* child must exit so as to not execute 
                    parent stuff */
            }/* end if pid == 0 */
            
            
        }/* end else pid < 0*/ 
        //get last pid for usage in child function  
        previouspidnum = pid;
        
    }

	printf("parent signalling child, current pid: %i\n", getpid());
    fflush(stdout);
    kill(pid, SIGUSR1);

    printf("parent waiting for child to terminate\n");
    fflush(stdout);

    //wait days for each process to end 
    for(int i = 0; i<DAYSOFCHRISTMAS; i++){
       if(pid=wait( &status)<0){
	        printf("waitpid error\n");
            fflush(stdout);
        }
    }
 
	
   printf("\nparent terminating \n");
   fflush(stdout);
} /* end of main*/




void child(int prevchildpid, int currentchildnum)
{
   
   printf("child has control (pid %i) number: %i\n", getpid(), currentchildnum);
   fflush(stdout);
   
    /* parent created  signal handler */

    //wait for the signal

   
    sigsuspend(&zeromask); /*wait */
    //(childnum %i)    currentchildnum,
    printf("\nChild: %i\n",  currentchildnum+1);
    fflush(stdout);

    

    //print out day of christmas and rest of statement
    printf("\nOn the %s day of Christmas\n",  daysArray[currentchildnum]);
    fflush(stdout);
    printf("my true love sent to me:\n");
    fflush(stdout);

    //var for and print 
    int lastAnd = 11;

    //loop for prints 
    for(int i = currentchildnum; i<=DAYSOFCHRISTMAS-1; i++){
        //check if not the first day if not add the and before gift otherwise just the first gift
        if (i == lastAnd && currentchildnum !=lastAnd){
            printf("and %s\n",  gifts[i]);
            fflush(stdout);
        }else{
            printf("%s\n",  gifts[i]);
            fflush(stdout);
            
        }    

    }

    //check for signal
    if(prevchildpid >= 0){
        //printf("\nchild about to kill/signal (pid %i), current pid: %i, fork num: %i\n", prevchildpid, getpid(), currentchildnum);
        //fflush(stdout);

        kill(prevchildpid, SIGUSR1);

    }


    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0){
        printf("sigmask error \n");
        fflush(stdout);
    }


    //printf("\nchild terminating after signal  \n");
    //fflush(stdout);


}



void sig_user (int signo)
{
    int sing, index;
    if (signo == SIGUSR1){

        //printf("signal 1 received by signal handler child (pid %i) \n", getpid());
        //fflush(stdout);

    }/* end if */

  
}/* end of signal handler */
