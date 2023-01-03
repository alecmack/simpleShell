//  myshell
//
//  Created by Alexander Mackanic on 9/14/22.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/fcntl.h>




int main(int argc, char** argv) {
    
    int STDIN = dup(0);
    int STDOUT = dup(1);
    
    char input[512];
    char** charArrays;
    char* lim;
    char* line;
    char* old_lim = NULL;
    int i;
    char** iterator;
    char isAPipe[2] = "|";
    char inRedir[2] = "<";
    char outRedir[2] = ">";
    char background[2] = "&";
    char blank[2] = " ";
    int numCommands;
    int runBackground;
    
    const char delims[8]={' ', '\r', '\n', '|', '>', '<', '&'}; // delimiter separating words
    
    //const char delims[3]={' ', '\r', '\n'};
    
    char* firstArg = "";
    
    if (argc > 1)           // assigns first argument to variable "firstArg"
    {
        firstArg = argv[1];
    }
    
    
    while(1)
    {
        if (0 != strcmp(firstArg, "-n"))            // only prints myshell if firstArg is not '-n'
        {
            printf("my_shell$ "); // prompt input
        }
        
        //fflush(stdin);
        
        line = fgets(input, 512, stdin); // get max 512 input chars from stdin
        
        charArrays = malloc(32*sizeof(char *));
        
        *charArrays = line; // point 0th pointer to the input
        
        i = 1; // some looping variables...
        old_lim = NULL;
        
        
        
        lim = strpbrk(line, delims); // pointer to first occurence of our delimiter
        
        numCommands = 0;  // counts number of pipes
        runBackground = 0; // variable keeping track of &
        
        while (lim != NULL)     // Code provided by instructor to store words
        {
            
            
            if (*lim == '|')        // counts and separates pipes
            {
                *(charArrays+i) = isAPipe;
                old_lim = lim;
                
                numCommands++;
                
                i++;
                lim++;
                *old_lim = '\0';
                
                while(0 != isspace(*lim))
                {
                    lim++;
                }
                *(charArrays+i) = (lim);
                i++;
                lim = strpbrk(lim+1, delims);
            }
            
            if (*lim == '<')                // tokenizes <
            {
                *(charArrays+i) = inRedir;
                old_lim = lim;
                                
                i++;
                lim++;
                *old_lim = '\0';
                
                while(0 != isspace(*lim))
                {
                    lim++;
                }
                *(charArrays+i) = (lim);
                i++;
                lim = strpbrk(lim+1, delims);
            }
            
            if (*lim == '>')                // tokenizes >
            {
                *(charArrays+i) = outRedir;
                old_lim = lim;
                                
                i++;
                lim++;
                *old_lim = '\0';
                
                while(0 != isspace(*lim))
                {
                    lim++;
                }
                *(charArrays+i) = (lim);
                i++;
                lim = strpbrk(lim+1, delims);
            }
            
            if (*lim == '&')                        // tokenizes &
            {
                *(charArrays+i) = background;
                old_lim = lim;
                runBackground = 1;
                
                
                i++;
                lim++;
                *old_lim = '\0';
                
                if(*lim == '\n')
                {
                    break;
                }
                
                while(0 != isspace(*lim))
                {
                    lim++;
                }
                *(charArrays+i) = (lim);
                i++;
                lim = strpbrk(lim+1, delims);
            }
            
            
            *(charArrays+i) = (lim+1); // + spaces
            old_lim = lim; // so we don't lose access to old lim
            i++;
            lim = strpbrk(lim+1, delims); // find pointer to next occurence of delimiter
            *old_lim = '\0'; // indicate end of the word
            
            
            
            
            
        }
    
        
        
        *(charArrays+i) = NULL; // sets the end of the string array
        
        
        
        /*                          FOR PRINTING TOKENS LINE BY LINE
        
        iterator = charArrays;  // Reset iterator to beginning
        
        while(*iterator != NULL)
        {
            printf("%s\n", *iterator);
            iterator++;
        }
        
        */
        
        
        
        iterator = charArrays;  // Reset iterator to beginning
        
        int afterFirstCommand = 0;
        char* inputFileName, *outputFileName;
        int inputRedir = 0;
        int outputRedir = 0;
        
        while(*iterator != NULL)        // until reaches end of string array
        {
            if (**iterator == '|')      // replaces '|' with NULL to mark end of command, then newline
            {
                *iterator = NULL;
                // printf("NULL");
                // printf("\n");
                afterFirstCommand++;
                
            }
            else if (**iterator == '<' && afterFirstCommand < 1)        // takes in input file name and removes token and name
            {
                inputRedir = 1;
                *iterator = blank;
                iterator++;
                while(**iterator == ' ')
                {
                    iterator++;
                }
                inputFileName = *iterator;
                **iterator = ' ';
                
            }
            
            else if (**iterator == '>' && afterFirstCommand == numCommands)     // takes in output file name and removes token and name
            {
                outputRedir = 1;
                *iterator = blank;
                while(**iterator == ' ')
                {
                    iterator++;
                }
                outputFileName = *iterator;
                **iterator = ' ';
                
            }
            else if (**iterator == '&')     // removes & token
            {
                *iterator = blank;
                iterator++;
            }
            else
            {
                if(*(iterator+1) == NULL || **(iterator+1) == '|' || (**(iterator+1) == '\0'))        // otherwise prints word on same line, if next "word" is
                    ; // printf("%s", *iterator);     // NULL, no space is printed
                else if (**(iterator+1) == ' ')
                {
                    ;
                }
                else
                    ; //printf("%s ", *iterator);
            }
            
            iterator++;
        }
        
       // printf("\n");
        
        
        fflush(stdout);
        
        iterator = charArrays;  // Resets iterator pointer to beginning of strings
        
        int fildes[2]; // 0 is reading, 1 is writing
        int status;
        
        int input = 0;      // first input is stdin
        
        if (inputRedir == 1)      // If first command, takes input from given file
        {
            int fdOpen = open(inputFileName, O_RDONLY);
            input = fdOpen;
            
        }
        
    
        int k;  // numCommands counter
        
        pid_t pid;
        
        for (k = 0; k < numCommands; k++)    // loops through each piped command except the last
        {
            pipe(fildes);  // creates pipe between parent and child proccesses
            
            pid = fork();  // Duplicates process
            
            if (pid < 0)
            {
                printf("ERROR: fork error");
            }
            
            if (pid == 0)       // child process
            {
                if (input != 0)      // assigns read to read from special pipe file if not reading from stdin
                {
                    dup2(input, 0);
                    close(input);   // close special file we no longer need access to
                }
                
                if (fildes[1] != 1)     // assigns write to special file of pipe if not writing to stdout
                {
                    dup2(fildes[1], 1);
                    close(fildes[1]);       // close special file we no longer need access to
                }
                
                status = execvp(iterator[0], iterator);  // Executes the command inputted into shell
                
                if (status < 0)
                {
                    printf("ERROR: shell command error\n");
                }
                
            }
            
            waitpid(pid, &status, 0);       // waits for child process to complete
            
            close(fildes[1]);       // closes the write end of the pipe
            input = fildes[0];      // new input will be read from special pipe file
            
            while(*iterator != NULL)        // Moves iterator to next command
            {
                iterator++;
            }
            iterator++;
            
            
        }
        
        if(input != 0)  // Read from special pipe file for final command
        {
            dup2(input, 0);
        }
        
         
        dup2(STDOUT, 1);   // changes output back to stdout
        
        if (outputRedir == 1)      // If last command has redirection, outputs to file
        {
            int fdOpen = open(outputFileName, O_WRONLY | O_CREAT);
            dup2(fdOpen, 1);
            close(fdOpen);
        }
            
        
        pid = fork();       // fork for last command
        
        if (pid < 0)
        {
            printf("ERROR: fork error");
        }
        
        if (pid == 0)
        {
            status = execvp(iterator[0], iterator);
            
            if(status < 0)
            {
                printf("ERROR: shell command error\n");
            }
        }
        if (runBackground == 0)             // only waits for child process if no &
            waitpid(pid, &status, 0);
        
        
        int g;
        
        
        for (g = 0; g < 2; g++) // close any open pipe files
        {
            //printf("Fildes: %d", fildes[g]);
            close(fildes[g]);
        }
        
        dup2(STDIN, 0);     // Resets input and ouput back to STDIN and STDOUT
        dup2(STDOUT, 1);
        
        
        
        free(charArrays);
        
    }
    
    return 0;
}


