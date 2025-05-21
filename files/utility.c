#include <stdio.h>
#include <stdlib.h>
#include "headers/utility.h"
#define PAGE_DIM 4096

int read_file(FILE *fp)
{
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, fp) != -1)
    {
        printf("%s", line);
    }
    
    return 0;
}

int convert_AddressToPage(int address)
{
    return address / PAGE_DIM;
}