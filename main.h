#define TRUE 1

#define PARAMS 11
#define INPUT 100
#define PATH 1000

int main();
void typePrompt();
void readCommand(char** parameters, int* nuberOfWord);
void interpretCommand(char** parameters, int* numberOfWords, int* typeOfCommand, int* pipeIndex);
void executeCommand(char** parameters);
void executePipeCommand(char** parameters, int numberOfWords, int pipeIndex);
void changeDirectory(char** parameters, int numberOfWords);
