#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_LENGTH 50000

void processData(char*);

int main()
{
    FILE* fp;
    char data[FILE_LENGTH+1] = {'\0'};
    size_t len = 0;
    size_t read;

    fp = fopen("data.txt", "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }
    fread(data, sizeof(char), FILE_LENGTH, fp);
    fclose(fp);

    processData(data);

    printf("New length: %d\r\n", strlen(data));
}

void processData(char data[])
{
    int c = 0;
    char prevChar = '\0';
    while (data[c] != '\0') {
        if (data[c] + 32 == prevChar || data[c] - 32 == prevChar) {
            // move everything after current char two places back
            memmove(data + (c-1), data + c + 1, FILE_LENGTH - c);
            // move c one back so we are sure we have the correct prevChar
            c -= 2;
            
            // plz don't crash if we remove the first char
            if (c < 0) {
                c = 0;
                prevChar = '\0';
                continue;
            }
        }
        prevChar = data[c];
        c++;
    }
}
