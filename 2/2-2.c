#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

const int NUM_LINES = 250;

bool has_one_diff(char*, char*);

int main() 
{
    char* strings[NUM_LINES];
    char* tmp;
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    size_t read;
    int i = 0, j = 0;
    char* cur_string;
    char* prev_string;

    fp = fopen("data.txt", "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        line[strcspn(line, "\r\n")] = 0; // remove line ending from string

        strings[i] = line; // store the string pointer
        i++;
        line = NULL; // set line to NULL so the new string is stored in a new area of memory
    }

    fclose(fp);

    // this is my demonstration of failure because I couldn't get qsort working :(
    for(i = 0; i < NUM_LINES; i++) {
        for(j = 0; j < NUM_LINES; j++) {
            if(strcmp(strings[i], strings[j]) < 0) {
                tmp = strings[i];
                strings[i] = strings[j];
                strings[j] = tmp;
            }
        }
    }

    for (int it = 0; it < NUM_LINES-1; it++) {
        prev_string = strings[it];
        cur_string = strings[it+1];

        if (has_one_diff(prev_string, cur_string)) {
            printf("String1: %s, String2: %s\r\n", prev_string, cur_string);
        }
    }
    
    // cleanups
    for (i = 0; i < NUM_LINES; i++) {
        free(strings[i]);
    }

    exit(EXIT_SUCCESS);
}

bool has_one_diff(char* string1, char* string2)
{
    int i = 0, diff = 0;
    for(i = 0; i < strlen(string1); i++) {
        if (string1[i] != string2[i]) {
            diff++;
            if (diff > 1) {
                return false;
            }
        }
    }
    return true;
}