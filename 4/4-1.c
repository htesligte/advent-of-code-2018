#define __USE_XOPEN 1
#define _GNU_SOURCE 1
#define __USE_GNU

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dlfcn.h>

#define NUM_LINES 1134
#define TIMESTAMP_SIZE 16
#define STR_NOT_FOUND -1
#define GUARDID_LENGTH 4
#define TYPE_GUARD_STARTS 1
#define TYPE_SLEEPING 2
#define TYPE_WAKEUP 3
#define MAX_GUARD 9999
#define MAX_LOG_ITEMS 100

struct LogItem {
    int type;
    int guardId;
    struct tm* tm;
};

struct Guard {
    int guardId;
    int numLogItems;
    struct LogItem* logItems[MAX_LOG_ITEMS];
    int sleepingMinutes[60];
    int totalSleepingMinutes;
};


void parseLogItems(struct LogItem**);
void sortLogItems(struct LogItem**);
struct LogItem* parseLogItem(char*);
int compareLogItems(struct LogItem*, struct LogItem*);
int strpos(char*, char*);
time_t time_to_epoch (const struct tm*, int);
void fillGuards(struct LogItem**);
void fillSleepingMinutes(struct Guard*);

struct Guard* guards[MAX_GUARD] = {NULL};

int main()
{
    struct LogItem* logItems[NUM_LINES] = {NULL};
    int i;
    struct Guard* sleepiestGuard = NULL;    
    int sleepiestMinute = 0;


    parseLogItems(logItems);
    sortLogItems(logItems);
    fillGuards(logItems);

    for (i = 0; i < MAX_GUARD; i++) {
        if (guards[i] != NULL) {
            fillSleepingMinutes(guards[i]);
            if (sleepiestGuard == NULL || sleepiestGuard->totalSleepingMinutes < guards[i]->totalSleepingMinutes) {
                sleepiestGuard = guards[i];
            }
        }
    }

    for (i = 0; i < 60; i++) {
        if (sleepiestGuard->sleepingMinutes[i] > sleepiestGuard->sleepingMinutes[sleepiestMinute]) {
            sleepiestMinute = i;
        }
    }

    printf("Sleepiest guard: %d\r\n", sleepiestGuard->guardId);
    printf("Most asleep at minute: %d\r\n", sleepiestMinute);

    return 0;
}

void parseLogItems(struct LogItem* logItems[])
{
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    size_t read;

    fp = fopen("data.txt", "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    int i = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        line[strcspn(line, "\r\n")] = '\0'; // remove line ending from string
        logItems[i] = parseLogItem(line);
        i++;
    }

    fclose(fp);
}

struct LogItem* parseLogItem(char* line)
{
    int pos = STR_NOT_FOUND, endPos = STR_NOT_FOUND;
    char guardStr[GUARDID_LENGTH+1] = {'\0'};
    struct LogItem* logItem = (struct LogItem*) malloc(sizeof(struct LogItem));
    struct tm* tm = (struct tm*) malloc(sizeof(struct tm));

    // copy timestamp to struct tm and assign to LogItem
    char timestamp[TIMESTAMP_SIZE+1] = {'\0'};
    strncpy(timestamp, line+1, TIMESTAMP_SIZE);
    strptime(timestamp, "%Y-%m-%d %H:%M", tm);
    logItem->tm = tm;

    // does line contain "Guard #"?
    if ((pos = strpos(line, "Guard #")) != STR_NOT_FOUND) {
        pos += strlen("Guard #");
        // retrieve the guard id
        endPos = strpos(line, " begins shift");
        strncpy(guardStr, line+pos, endPos - pos); // 7 = length of Guard #
        
        //printf("Found guard : %s with length %d\r\n", guardStr, endPos - pos); // 7 = length of Guard #
        logItem->type = TYPE_GUARD_STARTS;
        logItem->guardId = atoi(guardStr);

        // reserve a guard with that id already in the list of guards
        if (guards[logItem->guardId] == NULL) {
            guards[logItem->guardId] = (struct Guard*) malloc(sizeof(struct Guard));
            guards[logItem->guardId]->numLogItems = 0;
            guards[logItem->guardId]->guardId = logItem->guardId;
        }
    } else if ((pos = strpos(line, "falls asleep")) != STR_NOT_FOUND) {
        logItem->type = TYPE_SLEEPING;
    } else if ((pos = strpos(line, "wakes up")) != STR_NOT_FOUND) {
        logItem->type = TYPE_WAKEUP;
    }

    return logItem;
}

void sortLogItems(struct LogItem* logItems[])
{
    int i = 0, j = 0;
    struct LogItem* tmp;
    // Yes, I know I should have done something with qsort, but I got afraid of that at day 2 :(
    for(i = 0; i < NUM_LINES; i++) {
        for(j = 0; j < NUM_LINES; j++) {
            if(compareLogItems(logItems[i], logItems[j]) < 0) {
                tmp = logItems[i];
                logItems[i] = logItems[j];
                logItems[j] = tmp;
            }
        }
    }
}

int compareLogItems(struct LogItem* logItem1, struct LogItem* logItem2)
{
    time_t timestamp1, timestamp2;
    double seconds;

    timestamp1 = time_to_epoch(logItem1->tm, 0);
    timestamp2 = time_to_epoch(logItem2->tm, 0);

    seconds = difftime(timestamp1, timestamp2);

    if (seconds < 0) {
        return -1;
    } else {
        return 1;
    }
}

int strpos(char* haystack, char* needle)
{
    char *p = strstr(haystack, needle);
    if (p) {
        return p - haystack;
    }
    return STR_NOT_FOUND;
}


// from: https://gmbabar.wordpress.com/2010/12/01/mktime-slow-use-custom-function/
time_t time_to_epoch (const struct tm *ltm, int utcdiff ) {
   const int mon_days [] =
      {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
   long tyears, tdays, leaps, utc_hrs;
   int i;

   tyears = ltm->tm_year - 70 ; // tm->tm_year is from 1900.
   leaps = (tyears + 2) / 4; // no of next two lines until year 2100.
   //i = (ltm->tm_year – 100) / 100;
   //leaps -= ( (i/4)*3 + i%4 );
   tdays = 0;
   for (i=0; i < ltm->tm_mon; i++) tdays += mon_days[i];

   tdays += ltm->tm_mday-1; // days of month passed.
   tdays = tdays + (tyears * 365) + leaps;

   utc_hrs = ltm->tm_hour + utcdiff; // for your time zone.
   return (tdays * 86400) + (utc_hrs * 3600) + (ltm->tm_min * 60) + ltm->tm_sec;
}

void fillGuards(struct LogItem* logItems[])
{
    int i;
    struct Guard* currentGuard = NULL;    
    
    for (i = 0; i < NUM_LINES; i++) {
        if (logItems[i]->type == 1) {
            currentGuard = guards[logItems[i]->guardId];
        } else {
            currentGuard->logItems[currentGuard->numLogItems] = logItems[i];
            currentGuard->numLogItems++;
        }
    }
}

void fillSleepingMinutes(struct Guard* guard)
{
    int i = 0, j = 0;
    double currentSleepingMinutes;
    long timestamp1, timestamp2;
    struct LogItem* lastSleeping = NULL;

    for (i = 0; i < guard->numLogItems; i++) {
        if (guard->logItems[i]->type == TYPE_SLEEPING) {
            lastSleeping = guard->logItems[i];
        } else if(guard->logItems[i]->type == TYPE_WAKEUP) {
            timestamp1 = time_to_epoch(lastSleeping->tm, 0);
            timestamp2 = time_to_epoch(guard->logItems[i]->tm, 0);
            
            // difference in minutes
            currentSleepingMinutes = abs(difftime(timestamp1, timestamp2) / 60);
            
            guard->totalSleepingMinutes += currentSleepingMinutes;
            
            // for all the minutes that the guy was asleep, increment the counter of that minute
            for (j = 0; j < currentSleepingMinutes; j++) {
                guard->sleepingMinutes[(lastSleeping->tm->tm_min + j) % 60]++;
            }
        }
    }
}