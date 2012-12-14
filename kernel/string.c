#include <string.h>

int str_length(char * str) {
    int len = 0;
    while (*str++ != '\0')
        len++;
    return len;
}