#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// list of usable characters for the created key (includes a space)
char cryptArray[27] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};

int main (int argc, char* argv[]){
    // seed for random output
    srand(time(NULL));

    // check arguments
    if(argc < 2){
        fprintf(stderr,"%s\n", argv[0]); 
        exit(0); 
    }

    // use valid argument to create key string of appropriate length
    int numChar = atoi(argv[1]);
    char* key = malloc(sizeof(char) * (numChar + 1));

    // generate key of length numChar
    for(int i = 0; i < numChar; i++){
        key[i] = cryptArray[rand() % 27];
    }
    key[numChar] = '\0'; // terminate with null character

    // write key\n to stdout
    printf("%s\n", key);

    // free allocated memory
    free(key);

    return 0;
}
