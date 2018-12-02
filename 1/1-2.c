#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// ooh I should have done something dynamic but I am tired :(
int history_positive[100000000] = {0};
int history_negative[100000000] = {0};
int pos = 1;
bool in_history(int);

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

    // starting position = 0
    history_positive[0] = 1;

    int iteration_count = 1;

    while(true) {
        while ((read = getline(&line, &len, fp)) != -1) {
            cur_value += atoi(line);
            if (in_history(cur_value)) {
                printf("Found duplicate value %d\r\n", cur_value);
                exit(EXIT_SUCCESS);
                break;
            }

            if (cur_value < 0) {
                history_negative[cur_value] = 1;
            } else {
                history_positive[cur_value] = 1;
            }
            pos++;
        }
        printf("Iteration %d\r\n", iteration_count);
        iteration_count++;
        rewind(fp);
    }

    fclose(fp);
    if (line) {
        free(line);
    }
    exit(EXIT_SUCCESS);
}

bool in_history(int cur_value)
{
    // this is what you get for not doing something dynamic
    if (cur_value > 100000000) {
        printf("Error: cur_value = %d\r\n", cur_value);
        exit(EXIT_FAILURE);
    } 

    if (cur_value < 0) {
        return history_negative[cur_value];
    } else {
        return history_positive[cur_value];
    }
    
}