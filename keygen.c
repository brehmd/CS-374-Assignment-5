#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


char cryptArray[27] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};

int main (int argc, char* argv[]){
    srand(time(NULL));

    if(argc < 2){
        fprintf(stderr,"%s\n", argv[0]); 
        exit(0); 
    }

    int numChar = atoi(argv[1]);
    char* key = malloc(sizeof(char) * (numChar + 1));

    for(int i = 0; i < numChar; i++){
        key[i] = cryptArray[rand() % 27];
    }
    key[numChar] = '\0';

    printf("%s\n", key);

    free(key);

    return 0;
}
