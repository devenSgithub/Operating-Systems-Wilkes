
//oldlady.c

// Deven Schwartz
// 11/18/2021
// Project 5: There Was an Old Lady Who Swallowed a Fly
// Assigment 5: Program prints out the old lady poem in sync with messages
//used strcpy() for coping of strings: strcpy(dest, src);
//example usage I looked at https://www.tutorialspoint.com/c_standard_library/c_function_strcpy.htm
//dont know if you wanted it copied another way but it is in string.h and the file was included in the skeleton file provided 

//include files
#include <string.h>         // string
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <wait.h>
#include <sys/msg.h>
#include <sys/errno.h>
#include <sys/shm.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define NUMBEROFVERSES 8

extern int errno; 

#define PERM 0600

//struct for storing info
int msgqid;
struct sendverse{
    long mtype;
    
    struct verse{  
        int childno;
        char sheswalloweda[30];
        char theverse[200];
  
    }verse;
}qentry;

//swallowed array
const char *swallowed[NUMBEROFVERSES] = {
"fly; ",
"spider",
"bird",
"cat",
"dog",
"goat",
"cow",
"horse"

};

//words array
const char *thewords[200]={
"I don't know why she swallowed a fly, perhaps she'll die!",
"It wriggled and jiggled and tickled inside her! \n She swallowed the spider to catch the fly;",
"How absurd to swallow a bird! She swallowed the bird to catch the spider; ",
"Imagine that! She swallowed a cat! She swallowed the cat to catch the bird, ",
"What a hog, to swallow a dog! She swallowed the dog to catch the cat, ",
"She just opened her throat and swallowed a goat! \n She swallowed the goat to catch the dog, ",

"I do not know how she swallowed the cow! \n She swallowed the cow to catch the goat, ",
"She is dead of course!!! \n She swallowed the horse to catch the cow, "

};





void childsender(int childnumber){
    /*
    To send a message in wait mode:
    struct sendverse recite;
    rc = msgsnd(msgqid,&recite,sizeof(qentry.verse),0); 
    */
    //create a struct to send messages 
    struct sendverse recite;

    //set long val (pos non 0 val)
    recite.mtype = 1;

    //set child coming from
    recite.verse.childno = childnumber;
    //set recite ->verse-> swallowed and verse to respective strings
    //childnumber-1 positions 0-7 not 1-8
    //recite.verse.sheswalloweda = swallowed[childnumber-1];

    //The child should then build a message with its child number, what was swallowed, and the verse.
    //cpy verses to send from array into recite pos
    strcpy(recite.verse.sheswalloweda, swallowed[childnumber-1]);
    
    strcpy(recite.verse.theverse, thewords[childnumber-1]);

    //print out who is sending message
    printf("Child: %i, pid(%i) is sending a message.\n", childnumber, getpid());
    fflush(stdout);
    //send message onto queue
    msgsnd(msgqid, &recite, sizeof(recite.verse), 0);

    //end the function
   // exit(1);



}//end child





//start of assembler function
void assembler() {
    /*To receive a message in wait mode:
    struct sendverse torecite;
    rc = msgrcv(msgqid,&torecite,sizeof(qentry.verse),0,0); 
    */
    //struct for messages
    struct sendverse recite;

    //testing prints
    printf("Child 0 has entered the assembler function.\n");
    fflush(stdout);

    printf("Child for assembly, pid(%i) working.\n", getpid());
    fflush(stdout);

    //arrays to store messages
    char versearray[8][200];
    char swallowarray[8][30];

    //receive a message, print out the child number it received the message from, and copy the verse into the appropriate entry in the versus character array and what was swallowed in the swallowed array.
    for(int i = 0; i < 8; i++) {
        //recive message and store in recite
        msgrcv(msgqid, &recite, sizeof(recite.verse), 1, 0);
        printf("\nRecieved a message from child %i\n\n", recite.verse.childno);
        fflush(stdout);

        printf("Constructing the messages...\n");
        fflush(stdout);
        
        //copy strings to approiate entrys
        
        strcpy(swallowarray[recite.verse.childno - 1], recite.verse.sheswalloweda);
        strcpy(versearray[recite.verse.childno - 1], recite.verse.theverse);
    }//end of for

    //the rhyme has been assembled correctly at this point

    //This is a nested loop. The outer loop will execute 8 times, one for each verse
    for(int i = 0; i < 8; i++) {
        //swallowed message
        printf("There was an old lady who swallowed a %s\n",swallowarray[i]);
        fflush(stdout);
        //followed by approiate verse
        printf("%s\n",versearray[i]);
        fflush(stdout);
        //The inner loop executes from j=i-1;j>=0;j-- printing the remainder of the verse
        for(int j = i - 1; j >= 0; j--) {
            printf("%s\n",versearray[j]);
            fflush(stdout);
        }//end of inner for
    }//end of for



}//end of assembler





//sart of main
int main(){

    //statloc for wait
    int statloc;

    //for msg
    int msgget (key_t key, int msgflag); 
    msgqid = msgget(IPC_PRIVATE,PERM|IPC_CREAT|IPC_EXCL);

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
    if(retval==0) /*child */ // assembly
    {

        //print out child pid
        int c_pid = getpid();
        printf("I am assembler process my pid is %i.\n", c_pid);
        fflush(stdout);

        //call assemble function to recieve and put together messages being sent
        assembler();

        //print ending message
        printf("\n\tAssembly of the messages has ended. Goodbye!\n");
        fflush(stdout);

        //terminate
        //exit for child process
        exit(0);
        

    }else{ // inital parent 


        //the loop to create the patron processes 
        for(int i = 1; i<= 8; i++){
            
            printf("\tCreating a child to send messages\n");
            fflush(stdout);

            //fork each time 
            retval=fork();

            //check for a bad fork 
             if (retval<0){
                printf("---fork fail---\n");
                fflush(stdout);
                return(-2); /*fork failed*/
            }//end of bad fork

            //if == 0 good to contiune
            if(retval==0){ // senders

                //print pid and id when created
                int c_pid = getpid();
                printf("I am a child process, my pid is %i my id is: %i.\n", c_pid, i);
                fflush(stdout);
        

                //call sender with the id of child(1-8)
                childsender(i);
                
                //print that process is ending 
                int pid = getpid();
                printf("child process (pid:%i) is done and terminating.\n ", pid);
                fflush(stdout);
                

                //exit the process
                exit(0);
        
            }//end of  == 0
                    
        }//end of for

        //wait at end of parent for any process left
        wait(&statloc);         

    }//end of else


    //wait for the process to end
    for(int i = 0; i <= 8; i++) {
        wait(&statloc);	
    }


    //remove msg
    msgctl(msgqid,IPC_RMID,NULL);

    printf("\nMessage cleared, program ending.\n\n ");
    fflush(stdout);


}//end of program
//leave now!




//go away!




