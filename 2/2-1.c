#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

bool has_char_amount(char*, int);

int main(void)
{
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    size_t read;
    int count2 = 0;
    int count3 = 0;

    fp = fopen("data.txt", "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        if (has_char_amount(line, 2)) {
            count2++;
        }
        if (has_char_amount(line, 3)) {
            count3++;
        }
    }

    printf("%d * %d = %d\r\n", count2, count3, count2 * count3);

    fclose(fp);
    if (line) {
        free(line);
    }
    exit(EXIT_SUCCESS);
}

bool has_char_amount(char* str, int amount)
{
    // isn't it lovely to just keep messing with arrays, chars and ints in c
    // i feel one day i will be punished for this :-o
    int chars[255] = {0};
    int i;
    char* c;

    for (c = str; *c != '\0'; c++) {
        if (*c == '\r' || *c == '\n') {
            continue;
        }

        chars[(int)*c]++;
    }
    
    // now loop through the chars and return if one is exactly the specified amount
    for (i = 0; i < 255; i++) {
        if(chars[i] == amount) {
            return true;
        }
    }

    return false;
}

