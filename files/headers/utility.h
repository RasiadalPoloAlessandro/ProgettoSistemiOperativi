#include <stdio.h>
#include <dirent.h>
#include <string.h>

// Implementa la struttura della memoria fisica (suddivisa in frame di pagina)
struct page_frame
{
    int pageId; // Numero di Pagina Virtuale
    int rBit;
    int mBit;
};
typedef struct page_frame page_frame;
void process_file(char* path, page_frame *frames, int *bufferIndex, int numElements, int algorithm, int* pgHt, int* pgMs);
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

int secondChance(int address, page_frame* frames, int* bufferIndex, int numElements, int* pgHt, int* pgMs);
int LRU(int address, page_frame* frames, int* bufferIndex, int numElements, int* pgHt, int* pgMs);
void print_stats(int algoritmo, int *pHit, int *pFault);
