#include <stdio.h>
#include <time.h>

int my_time(void) {
    time_t now = time(NULL);
    struct tm *ptm = localtime(&now);   // returns pointer to static storage
    if (!ptm) return -1;

    struct tm lt;                       // own copy
    lt = *ptm;                          // struct assignment (shallow copy)
                                        // struct copy with =
                                        // Be careful if pointer exists.

    printf("Local time: %04d-%02d-%02d %02d:%02d:%02d\n",
           lt.tm_year + 1900,   // Year starts from 1900
           lt.tm_mon + 1,       // Month starts from 0
           lt.tm_mday,          // Day of month
           lt.tm_hour,          // Hour
           lt.tm_min,           // Minute
           lt.tm_sec);          // Second
    return 0;
}

