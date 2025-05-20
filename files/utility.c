#include <stdio.h>
#include <stdlib.h>
#include "headers/utility.h"

int read_file(FILE *fp)
{
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, fp) != -1)
    {
        printf("%s", line);
    }
    free(line);

    return 0;
}