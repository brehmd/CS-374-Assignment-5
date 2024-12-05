#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

static char cryptArray[27] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};

int find_char_index(char target) {
    for (int i = 0; cryptArray[i] != '\0'; i++) {
        if (cryptArray[i] == target) {
            return i; // Found the character at index i
        }
    }
    return -1; // Character not found                   // ERROR
}

void decipher_text(char* ciphertext, char* key){
    // create a ciphertext buffer equal in length to the plaintext
    int text_length = strlen(ciphertext);
    char* plaintext = malloc(sizeof(char) * (text_length + 1));

    /*
    for each character in plaintext,
        add the int values of the plaintext character and corresponding key character
        generate ciphertext using this encrypted int as the index
    */ 
    for(int i = 0; i < text_length; i++){
        int decrypt_int = find_char_index(ciphertext[i]) - find_char_index(key[i]) + 27;

        plaintext[i] = cryptArray[decrypt_int % 27];
    }
    plaintext[text_length] = '\0';

    // write ciphertext\n to stdout
    printf("%s\n", plaintext);

    // free allocated memory
    free(plaintext);
}

int main(int argc, char* argv[]){

    // // shortcut for testing purposes
    // decipher_text(argv[1], argv[2]);

    return 0;
}