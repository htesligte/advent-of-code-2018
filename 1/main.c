#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    size_t read;
    int cur_value = 0;

    fp = fopen("data.txt", "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        cur_value += atoi(line);
    }

    printf("Current value: %d\r\n", cur_value);

    fclose(fp);
    if (line) {
        free(line);
    }
    exit(EXIT_SUCCESS);
}