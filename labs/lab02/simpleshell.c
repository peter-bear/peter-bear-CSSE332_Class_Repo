/* Copyright 2016 Rose-Hulman
   But based on idea from http://cnds.eecs.jacobs-university.de/courses/caoslab-2007/
   */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>



void handle_sigchld(int ignored)
{
  /* TODO: Insert your code here! */
  int flag;
  wait(&flag);

}

int main() {
    char command[82];
    char *parsed_command[2];
    
    //takes at most two input arguments
    // infinite loop but ^C quits
    signal(SIGCHLD, handle_sigchld);

    while (1) {
        int status;
        printf("RUSH%% ");
        fgets(command, 82, stdin);
        command[strlen(command) - 1] = '\0';//remove the \n
        int len_1;
        for(len_1 = 0;command[len_1] != '\0';len_1++){
            if(command[len_1] == ' ')
                break;
        }
        parsed_command[0] = command;
        int f = fork();
        if(f == 0){
            if(parsed_command[0][0] == 'B' && parsed_command[0][1] == 'G'){
                f = fork();
                if(f == 0){
                    if(len_1 == strlen(command)){
                        printf("Command is '%s' with no arguments\n", parsed_command[0]); 
                        parsed_command[1] = NULL;
                        execlp(parsed_command[0]+2, parsed_command[0]+2, parsed_command[1], NULL);
                        exit(1);
                    }else{
                        command[len_1] = '\0';
                        parsed_command[1] = command + len_1 + 1;
                        printf("Command is '%s' with argument '%s'\n", parsed_command[0], parsed_command[1]); 
                        execlp(parsed_command[0]+2, parsed_command[0]+2, parsed_command[1], NULL);
                        exit(1);
                    }
                }
                wait(&status);
                printf("Background command finished\n");
                exit(1);
                
            }else{
                if(len_1 == strlen(command)){
                    printf("Command is '%s' with no arguments\n", parsed_command[0]); 
                    parsed_command[1] = NULL;
                    execlp(parsed_command[0], parsed_command[0], parsed_command[1], NULL);
                    exit(1);
                }else{
                    command[len_1] = '\0';
                    parsed_command[1] = command + len_1 + 1;
                    printf("Command is '%s' with argument '%s'\n", parsed_command[0], parsed_command[1]); 
                    execlp(parsed_command[0], parsed_command[0], parsed_command[1], NULL);
                    exit(1);
                }
            }
        }
        if(!(parsed_command[0][0] == 'B' && parsed_command[0][1] == 'G'))
            wait(&status);


    }
}
