#include "main.h"
#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <string.h>

//Print the current working directory
void typePrompt(){
    char path[PATH] = "";

    //Get the current working directory
    getcwd(path, sizeof(char)*PATH);
    printf("%s  $ ", path);
}

//Reads a single line of command from the terminal, divides it
//into tokens and returns pointers to these
void readCommand(char** parameters, int* nuberOfWord){

    //To hold input String
    char input[INPUT] = "";

    //Read input. Leave space for null-termination
    scanf("%99[^\n]s", input);

    //Clear stdin
    scanf("%*[^\n]");
    scanf("%*c");

    //Split string into tokens (like "ls", "-l" etc.)
    char* token = strtok(input, " ");

    *nuberOfWord = 0;

    //Go through tokens
    while(token != NULL && *nuberOfWord < PARAMS){

        //Add the tokens to parameters (our array of char*).
        //By using strdup space is automatically allocated on the heap using Malloc (later to be freed)
        *(parameters + *nuberOfWord) = strdup(token);

        //Interate through tokens
        token = strtok(NULL, " ");

        //Remember to count number of words, as it is to be used elsewhere in the program
        (*nuberOfWord)++;
    }

    //Remember to null-terminate our char* array, as it is required when using exec later
    *(parameters + *nuberOfWord) = NULL;

}

//Used to identify what type of command is being called.
//main() switches based on this information
void interpretCommand(char** parameters, int* numberOfWords, int* typeOfCommand, int* pipeIndex){

    //Default - none of the below
    *typeOfCommand = 0;

    //Exit
    if(strcmp(*parameters, "exit") == 0){
        *typeOfCommand = 1;
        return;
    }

    //cd
    if(strcmp(*parameters, "cd") == 0){
        *typeOfCommand = 2;
        return;
    }

    //Pipe
    //Loop through tokens
    for (int i = 0; i < *numberOfWords; ++i) {
        if(strcmp(*(parameters+i), "|") == 0){
            *typeOfCommand = 3;

            //Also return the index of pipe character "|", as it is to be used elsewhere in the program
            *pipeIndex = i;
            return;
        }
    }
}

//Execute a simple command line
void executeCommand(char** parameters){

    /* When calling fork() (which is a system call) a new process is created (forked) from the current.
    An identical copy is made, including all references. The only difference is
    their respective pid (program id). In the parent, fork() returns the pid of the child
    but in the child, fork() returns 0. We use this to make the two processes deviate from
    each other and execute different code */
    if (fork() != 0){

        //Parent code
        //The wait() call makes the parent process wait for child process to terminate before proceeding
        wait(NULL);
    } else {

        //Child code
        /* exec is also a system call, which is used to locate and execute the target file with the given parameters.
        The call completely replaces the current process image. The process therefor never returns from the call
        but is automatically terminated */
        execvp(*parameters, parameters);
    }
}

//Execute a command containing a pipe
void executePipeCommand(char** parameters, int numberOfWords, int pipeIndex){

    //"Spilt" the input by inserting a NULL in place of of "|"
    *(parameters+pipeIndex) = NULL;

    //Create pointers to the two different programs that need to execute.
    //The output of program one is going to be the input of program two
    char** programOne = parameters;
    char** programTwo = parameters + pipeIndex + 1;

    //Create an array needed to hold the two file descriptors of our pipe
    //pipe[0] for reading and pipe[1] for writing
    int pipefd[2];

    /* The following system call actually creates the pipe we need for the interprocess communication
    Whatever is written to the write end of the pipe, is buffered by the kernel until is it read from the read end
    We give pipefd as parameter for the method to return the two file descriptors */
    pipe(pipefd);

    //As explained in executeCommand() we fork
    if ((fork()) != 0){

        //Parent code
        //File descriptors not in use are closed
        close(pipefd[0]);
        close(pipefd[1]);

        //Wait for all descendants to finish before the program continues
        wait(NULL);

    } else {

        //In order to execute both programs we need to fork once more, effectively giving us three processes to manage
        if (fork() != 0){

            //Child - executes programTwo
            //First wait for program one to finish, as we need the output in order to continue
            wait(NULL);

            //This process needs to read the output of its child, therefor we close the file descriptor for writing to the pipe
            close(pipefd[1]);

            //The dup2 system call is used to change stdin to the reading end of the pipe. This is also called I/O redirection
            dup2(pipefd[0], STDIN_FILENO);

            //As the program executes, it therefor now gets its input from the pipe instead of stdin, completing the interprocess communication
            execvp(*programTwo, programTwo);

        } else {

            //Grand child - executes programOne
            //This process needs to write its output to its parent, therefor we close the file descriptor for reading from the pipe
            close(pipefd[0]);

            //The dup2 system call is used to change stdout to the writing end of the pipe. This is also called I/O redirection
            dup2(pipefd[1], STDOUT_FILENO);

            //As the program executes, its output is therefor now written to the pipe
            execvp(*programOne, programOne);
        }
    }
}

void changeDirectory(char** parameters, int numberOfWords){

    //cd
    if(numberOfWords == 1){
        chdir(getenv("HOME"));
        return;
    }

    //cd PATH
    char* path = *(parameters+1);
    chdir(path);

}

int main() {

    //To hold all input tokens
    char* parameters[PARAMS];

    //To hold information about input string
    int numberOfWords, pipeIndex;

    //Begin infinite loop
    while(TRUE){

        //Print current working directory
        typePrompt();

        //Read and organize input
        readCommand(parameters, &numberOfWords);

        int typeOfCommand;

        //Analyze what type of command was given
        interpretCommand(parameters, &numberOfWords, &typeOfCommand, &pipeIndex);

        switch(typeOfCommand){

            //exit
            case 1 :
                exit(0);

            //cd
            case 2 :
                changeDirectory(parameters, numberOfWords);
                break;

            //pipe
            case 3:
                executePipeCommand(parameters, numberOfWords, pipeIndex);
                break;

            default:
                executeCommand(parameters);
        }

        //Clean up (from strdup())
        for (int i = 0; i < numberOfWords; ++i)
            free(parameters[i]);

    }
}

