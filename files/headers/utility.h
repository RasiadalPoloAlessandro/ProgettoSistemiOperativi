#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <pthread.h>

// Implementa la struttura della memoria fisica (suddivisa in frame di pagina)
struct page_frame
{
    int pageId; // Numero di Pagina Virtuale
    int rBit;
    int mBit;
    int lastAccessed;
};
typedef struct page_frame page_frame;
void process_file(char *path, page_frame *frames, int *bufferIndex, int numElements, int algorithm, int *pgHt, int *pgMs, pthread_mutex_t *mutex);
//int read_file(FILE *fp);
int convert_AddressToPage(int address);
FILE* open_file(char* file);
struct nodoStringa
{

    char *value;
    struct nodoStringa *next_ptr;
};
typedef struct nodoStringa Lista;
Lista* read_directory(char* directory, int* fileCounter);
void pre_insert(Lista **ptr, char* val);

int secondChance(int address, page_frame* frames, int* bufferIndex, int numElements);
int LRU(int address, page_frame* frames, int* bufferIndex, int numElements);



//Parte multithread
typedef struct {
    char* path;
    page_frame* frames;
    int* bufferIndex;
    int numElements;
    int algorithm;
    int* pFault;
    int* pHit;
    pthread_mutex_t *frames_mutex;
} ThreadArgs;
void* thread_process_file(void* arg);

/* 
*PRE: tutti valori sono gestiti correttamente in modalità mutualmente esclusiva tra tutti i thread evitando race condition
* POST: Restituisce le statistiche dei pageFault e PageHit
*/
void print_stats(int algoritmo, int *pHit, int *pFault);