//https://www.geeksforgeeks.org/chdir-in-c-language-with-examples/
//https://www.geeksforgeeks.org/pipe-system-call/
//https://pubs.opengroup.org/onlinepubs/009695399/functions/getcwd.html
//https://stackoverflow.com/questions/9493234/chdir-to-home-directory
//https://fresh2refresh.com/c-programming/c-strings/c-strrchr/
//https://www.geeksforgeeks.org/pipe-system-call/
//https://stackoverflow.com/questions/21923982/how-to-create-a-linux-pipeline-example-in-c

//todo beskriv hvor store input der tages
//bemmærk kun et mellemrum mellem ord
//Man kan skrive cd, cd /path eller cd .. Format for path is movies/jamesbond/...
//Huske kilde bogen

//Max 10 params since it has to be null terminated?
//Paths can be up to 500
//Max input size = 99 characters since input[100] +  null-termination

#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define PARAMS 11
#define INPUT 100
#define PATH 500

void typePrompt(){
    char path[PATH] = "";
    getcwd(path, sizeof(char)*PATH);
    printf("%s  %% ", path);
}

void readCommand(char** parameters, int* nuberOfWord){

    char input[INPUT] = "";

    //Read input
    scanf("%99[^\n]s", input);

    //Clear stdin
    scanf("%*[^\n]");
    scanf("%*c");

    //Split string into tokens
    char* token = strtok(input, " ");

    *nuberOfWord = 0;
    while(token != NULL && *nuberOfWord < PARAMS){
        parameters[*nuberOfWord] = strdup(token);
        token = strtok(NULL, " ");
        (*nuberOfWord)++;
    }

    parameters[*nuberOfWord] = NULL;

}

void interpretCommand(char** parameters, int* numberOfWords, int* typeOfCommand, int* pipeIndex){

    //Default
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
    for (int i = 0; i < *numberOfWords; ++i) {
        if(strcmp(*(parameters+i), "|") == 0){
            *typeOfCommand = 3;
            *pipeIndex = i;
            return;
        }
    }

}

void executeCommand(char** parameters){

    if (fork() != 0){
        //Parent code
        waitpid(-1, NULL, 0);
    } else {
        //Child code
        execvp(*parameters, parameters);
    }

}

void executePipeCommand(char** parameters, int numberOfWords, int pipeIndex){

    //Spilt input
    *(parameters+pipeIndex) = NULL;

    char** programOne = parameters;
    char** programTwo = parameters + pipeIndex + 1;

    int pipefd[2];
    pipe(pipefd);

    if ((fork()) != 0){

        //Parent code
        close(pipefd[0]);
        close(pipefd[1]);
        wait(NULL);

    } else {

        //Child code
        if (fork() != 0){

            //Executes programTwo
            wait(NULL);
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            execvp(*programTwo, programTwo);

        } else {
            //Executes programOne
            close(pipefd[0]);
            dup2(pipefd[1], STDOUT_FILENO);
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

    int numberOfWords, pipeIndex;
    char* parameters[PARAMS];

    while(TRUE){

        typePrompt();
        readCommand(parameters, &numberOfWords);

        int typeOfCommand;
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

        //Clean up
        for (int i = 0; i < numberOfWords; ++i)
            free(parameters[i]);

    }
}