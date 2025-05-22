#include <stdio.h>
#include <dirent.h>


int read_file(FILE *fp);
int convert_AddressToPage(int address);
char* read_directory(char* directory);
FILE* open_file(char* file);