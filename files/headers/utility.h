#include <stdio.h>
#include <dirent.h>
#include <string.h>

struct page_frame
{
    int pageId; // Numero di Pagina Virtuale
    int rBit;
    int mBit;
};
typedef struct page_frame page_frame;
void process_file(char* path, page_frame *frames, int *bufferIndex, int numElements);
//int read_file(FILE *fp);
int convert_AddressToPage(int address);
FILE* open_file(char* file);
struct nodoStringa
{

    char *value;
    struct nodoStringa *next_ptr;
};
typedef struct nodoStringa Lista;
Lista* read_directory(char* directory);
void pre_insert(Lista **ptr, char* val);

int secondChance(int address, page_frame* frames, int* bufferIndex, int numElements);
