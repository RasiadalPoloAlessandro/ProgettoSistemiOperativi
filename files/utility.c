#include <stdio.h>
#include <stdlib.h>
#include "headers/utility.h"
#define PAGE_DIM 4096


FILE* open_file(char* file){

    FILE *fp = fopen(file, "r");
    if (!fp)
    {
        perror("Errore nell'apertura del file");
        return NULL;
    }
}

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


/*
* Leggo la cartella contentente i file di input
*/
char* read_directory(char *directory)
{
    DIR *d;
    char* files;
    struct dirent *dir;
        char *path = directory;
        d = opendir(path);
        if (d)
        {
            while ((dir = readdir(d)) != NULL)
            {
                printf("%s\n", dir->d_name);
            }
            closedir(d);
        }
        else
            perror("Errore nella lettura della cartella");
}
