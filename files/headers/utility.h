#include <stdio.h>
#include <dirent.h>
#include <string.h>

int read_file(FILE *fp);
int convert_AddressToPage(int address);
FILE* open_file(char* file);
struct nodoStringa;
typedef struct nodoStringa Lista;
Lista* read_directory(char* directory);
void pre_insert(Lista **ptr, char* val);
