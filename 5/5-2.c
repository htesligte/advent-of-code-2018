#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_LENGTH 50000

void processData(char*);
void refreshData(char*);
void removeChar(char, char*);

int main()
{
    char data[FILE_LENGTH+1] = {'\0'}, c = '\0';
    int i = 0;
    size_t shortest = 999999999;
    char shortestChar = '\0';
    refreshData(data);

    for (i = 0; i < 26; i++) {
        refreshData(data);
        c = 97+i;

        removeChar(c, data);
        processData(data);
        
        if (strlen(data) < shortest) {
            shortest = strlen(data);
            shortestChar = c;
        }
        
    }
    printf("Shortest: %d, char: %c\r\n", shortest, shortestChar);
    return 0;
}

void refreshData(char data[])
{
    FILE* fp;
    
    size_t len = 0;
    size_t read;

    fp = fopen("data.txt", "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }
    fread(data, sizeof(char), FILE_LENGTH, fp);
    fclose(fp);
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

void removeChar(char c, char data[])
{
    int p = 0;
    char capital = c - 32;
    char curChar;
    while (data[p] != '\0') {
        curChar = data[p]; 
        if (curChar == c || curChar == capital) {
            // move everything after current char one place back
            memmove(data + p, data + p + 1, FILE_LENGTH - p);
            p--;
        }
        p++;
    }
}
