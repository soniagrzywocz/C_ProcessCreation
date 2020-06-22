#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*  References
 https://www.geeksforgeeks.org/ctime-function-in-c-c/ - the idea of how to use ctime
 https://www.gnu.org/software/libc/manual/html_node/Process-Completion-Status.html#Process-Completion-Status - compilation status
 https://www.geeksforgeeks.org/escape-sequences-c/ - for the tab replacing
 */

/*
 To compile:
 ~ gcc -o lab7 lab7.c
 ~ ./lab7 <filename.txt>
 */

void logCont(char *temp, char **arr) {
    // creating an array here
    int j;
    int c; // for count

   temp[strlen(temp)-1] = '\0'; //putting /0 as the last char
    for (j = 0, arr[0] = &temp[0], c = 1; j < strlen(temp); j++) {
        if (temp[j] == ' ') {
           temp[j] = '\0'; //assiging here as well
           arr[c++] = &temp[j+1];
        }
    }
    arr[c] = (char *)NULL; //initializing
}



int main(int argc, char **argv) {
    
    pid_t pid;
    
    int status;
    char *arguments[BUFSIZ];
    char temp2[BUFSIZ];
    char line[BUFSIZ];

    FILE *file1;
    FILE *file2;
    time_t start; //initial time
    time_t finish; //finish time

    if (argc < 2) {
        printf("Error. Not enough arguments");
        exit(-1);
    }

    file1 = fopen(argv[1],"r");
    if (file1 == NULL) {
    printf("Error. Could not open reading file");
    exit(-1);
    }

    file2 = fopen("result.log","w");
    if (file2 == NULL) {
    printf("Error. Could not open the file for writing\n");
    exit(-1);
    }

    while (fgets(line, BUFSIZ, file1) != NULL) {
      strcpy(temp2, line); /* save line read */
      logCont(line, arguments);
      time(&start);
      pid = fork();
      if (pid == 0) { /* this is child process */
        execvp(arguments[0], arguments);
    perror("exec");
    exit(-1);
      } else if (pid > 0) { /* this is the parent process */
        printf("Child started at time: %s", ctime(&start)); // initial time
        printf("Wait for the child process to terminate\n");
        wait(&status); /* wait for the child process to terminate */
        time(&finish); // finish time
        printf("Child ended at %s", ctime(&finish));
        if (WIFEXITED(status)) { /* child process terminated normally */
      printf("Child process exited with status = %d\n", WEXITSTATUS(status));
        } else { /* child process did not terminate normally */
      printf("Child process did not terminate normally!\n");
        }
    temp2[strlen(temp2) - 1] = '\t'; //adding the \t here
    strcat(temp2, ctime(&start)); // here I am adding the start time
    temp2[strlen(temp2) - 1] = '\t'; //adding the \t at the end again
    strcat(temp2, ctime(&finish)); //adding the finish time
    fprintf(file2, "%s", temp2); // printing to the output file
    fflush(file2); //flushing the file
      } else { /* we have an error */
        perror("fork"); /* use perror to print the system error message */
        exit(EXIT_FAILURE);
      }
    }

    fclose(file1);
    fclose(file2);
    printf("[%ld]: Exiting main program .....\n", (long)getpid());

    return 0;
}

