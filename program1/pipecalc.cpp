
// pipecalc.cpp
// Deven Schwartz
// 9/23/2021
// Project 1: Communication with Unix Pipes: calculator 
// 
// Description: Program uses two process with two pipes to communicate between them. 
// The parent process takes user input them sends the information to the child to get the results. 
// The child sends the results back to the parent where it is displayed for the user
// Flow: program successfully produces the correct results for all calculations.
// References: Examples on website.
// Assistance: Dr. Bracken for errors / missing information
//
//Testing: I have thoroughly tested my code and have errors that I have not fixed.   
//1.) For echos of chars the output being display is 0 not the character that is being entered.
//2.) I left my results as an integer not sure if you wanted them converted to double for divide cases 
//Time: ~ 4 hours.


//include files
#include "sys/types.h"

#include "sys/ipc.h"

#include "unistd.h"

#include "iostream"

#include "wait.h"

using namespace std;



int main()

{
    //initilize needed values.
    int operand1 = 0;
    int operand2 = 0;
    char operation = ' ';
    int result = 0;

    int statloc;

    int parentpipe[2];
    int childpipe[2];

    pipe(parentpipe);
    pipe(childpipe);
    int retval;
    //fork program
    retval=fork();
    //bad fork check
    if (retval<0)
    {
        return(-2); /*fork failed*/
    }
    //child process
    if(retval==0) /*child */

    {
    // closes unused pipe ends
        close(childpipe[0]); 
        close(parentpipe[1]); 
        
        //printf("I am the child reading from the pipe\n");
        //fflush(stdout);
        
    do{

            //get operation char
            read(parentpipe[0],&operation,sizeof(char));

            //check for valid operation char (not X)
            if (operation != 'X'){

                //read in operands
                read(parentpipe[0],&operand1,sizeof(int));
                read(parentpipe[0],&operand2,sizeof(int));


                ////switch for calculations
                switch(operation)
                {
                
                case '+': //for addition case
                    cout << "(Addition)" << endl;
                    result = operand1 + operand2;
                    break; 
                

                case '-': //for subtraction case
                    cout << "(Subtraction)" << endl;
                    result = operand1 - operand2;
                    break;
                

                case '*': //for multiplication case
                    cout << "(Multiplication)" << endl; 
                    result = operand1 * operand2;
                    break;
                

                case '/': //for division case
                    cout << "(Division)" << endl;
                    result = operand1 / operand2;
                    
                    break;
                    

                case 'X':
                    //exit case
                    cout<<"-Goodbye-" << endl;
                    break;
                
                default:
                //not needed / will never get to this point
                    cout << "Invalid Selection. Please Enter Again." << endl; 
                }//switch end


                //return results to parent
                write(childpipe[1],&result,sizeof(int));
            }


           /* if(operation == 'X'){ //test: not needed never gets here

               break;
            }*/

        }while(operation != 'X');
    
        //if X is entered it closes and ends

        if (operation == 'X'){
            close(childpipe[1]);
        }

  
    }else{//parent process

        /*parent closes pipesends not using */
        close (parentpipe[0]);
        close (childpipe[1]); 

        //printf("I am the parent about to write to the pipe\n");
        //fflush(stdout);
        
        //operation = ' ';
        do{
            cout << " \nEnter: " << endl; 
            cout << "   + for addition. " << endl; 
            cout << "   - for subtraction. " << endl; 
            cout << "   * for multiplication. " << endl; 
            cout << "   / for division. " << endl;  
            cout << "   X to exit the menu. " << endl; 

            cin >> operation;

            cout << "	\nYou entered: " << operation << endl; 
            
            //for invalid input try again until good input
            while(operation != '+' && operation != '-' && operation != '*' && operation != '/' && operation != 'X'){
                cout << "INVALD INPUT" << endl; 
                cout << " \nEnter: " << endl; 
                cout << "   + for addition. " << endl; 
                cout << "   - for subtraction. " << endl; 
                cout << "   * for multiplication. " << endl; 
                cout << "   / for division. " << endl;  
                cout << "   X to exit the menu. " << endl; 

                cin >> operation;

                cout << "	\nYou entered: " << operation << endl; 

            }

            //send operation to parent
            write(parentpipe[1],&operation,sizeof(char));
            //if X exit loop
            if (operation =='X'){
                //cout<<"breaktest"<<endl;
                break;
            }

            //if not X contiune
            if (operation != 'X'){


                //set flag to test operand inputs
                bool operandflag = false;
                //check operands and repeat until user gives valid input
                while(operandflag != true){
                
                    //enter value 1
                    cout << "Enter the first operand: " << endl;
                    cin >> operand1;
                
                    cout << "You entered: " << operand1 << endl; 
                   
                   
                   
                    if(cin.good()){
                        //for correct value keep true
                        operandflag = true;
                        
                    }else{
                        //first value is bad, set flag to false so loop will repeat, contiune to second test
                        operandflag = false;
                        cin.clear();
                        cin.ignore(120,'\n');
                    }

                    // enter second value 
                    cout << "Enter the second operand: " << endl;
                    cin >> operand2;
                    cout << "You entered: " << operand2 << endl; 

                

                    if (operandflag != true){
                        //if flag is bad clear cin and loop restarts
                        cin.clear();
                        cin.ignore(120,'\n');
                    }else{
                        //otherwise check for valid input and set flag true is good
                        if(cin.good()){
                        
                        operandflag = true;
                        
                        }else{
                            // value is bad set flag to false and restart loop
                            operandflag = false;
                            cin.clear();
                            cin.ignore(120,'\n');
                        }
                    }

                }
                //end of check

                //send operands to child 
                write(parentpipe[1],&operand1,sizeof(int));
                write(parentpipe[1],&operand2,sizeof(int));

                //get results back and print
                read(childpipe[0],&result,sizeof(int));
                printf("%i %c %i = %i\n",operand1, operation, operand2, result);

            }

        }while (operation != 'X');

        //wait for child to end 
        wait(&statloc); //parent must wait for child to terminate

        //when X is entered after child closes parent must close pipe ends
        close(parentpipe[1]);
        close(childpipe[0]);

    }

}