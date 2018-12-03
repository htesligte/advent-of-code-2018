#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NUM_LINES 1381
#define MAX_WIDTH 1000
#define MAX_POS 1000000

struct LineItem {
    int i;
    int x;
    int y;
    int w;
    int h;
};

struct LineItem* parseLineItem(char*);
int charpos(char *, char);

struct LineItem* parseLineItem(char* line) {
    int atPos, commaPos, colonPos, xPos, eofStr;
    struct LineItem* lineItem = (struct LineItem*) malloc(sizeof(struct LineItem));
    char i[4] = {'\0'}, x[4] = {'\0'}, y[4] = {'\0'}, w[4] = {'\0'}, h[4] = {'\0'};

    atPos = charpos(line, '@');
    commaPos = charpos(line, ',');
    colonPos = charpos(line, ':');
    xPos = charpos(line, 'x');
    eofStr = (int)strlen(line);

    // we now know where every char in this string is
    // so time to start filling in the lineItem fields
    // the key starts on index 1, and runs until atPos-3
    strncpy(i, line+1, atPos-3);
    lineItem->i = atoi(i);

    // x is on atPos+1 until commaPos-2
    strncpy(x, line+atPos+1, ((commaPos-1) - (atPos+1)));
    lineItem->x = atoi(x);

    // y is on commaPos until colon-2
    strncpy(y, line+commaPos, (colonPos-1 - commaPos));
    lineItem->y = atoi(y);

    // w is on colonPos+1 until xPos-2
    strncpy(w, line+colonPos+1, ((xPos-1) - (colonPos+1)));
    lineItem->w = atoi(w);

    // h is on xPos until eofStr-2
    strncpy(h, line+xPos, (eofStr - xPos));
    lineItem->h = atoi(h);

    return lineItem;
}

int charpos(char *haystack, char needle)
{
   return strchr(haystack, needle)-haystack+1;
}

int main()
{
    struct LineItem* lineItems[NUM_LINES] = {NULL};
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    size_t read;
    int startPos = 0, y = 0, x = 0;
    int positions[MAX_POS] = {0};
    int pos = 0;
    int duplicates = 0;

    fp = fopen("data.txt", "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    int i = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        line[strcspn(line, "\r\n")] = '\0'; // remove line ending from string
        lineItems[i] = parseLineItem(line);
        i++;
    }

    fclose(fp);

    for (i = 0; i < NUM_LINES; i++) {
        startPos = (lineItems[i]->y * MAX_WIDTH) + lineItems[i]->x+1;
        for (y = 0; y < lineItems[i]->h; y++) {
            for (x = 0; x < lineItems[i]->w; x++) {
                pos = startPos + ((y * MAX_WIDTH) + x);
                positions[pos]++;
                if (positions[pos] == 2) {
                    duplicates = duplicates + 1;
                }
            }
        }
    }

    printf("Duplicates: %d.\r\n", duplicates);

    for (i = 0; i < NUM_LINES; i++) {
        if (lineItems[i] == NULL) {
            continue;
        }
        // printf("Freeing up: %d\r\n", lineItems[i]->i);
        free(lineItems[i]);
    }

    return 0;
}