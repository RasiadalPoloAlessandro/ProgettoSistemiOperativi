#include <stdio.h>
#include <stdlib.h>
#include "headers/utility.h"
#include <string.h>
#define PAGE_DIM 4096
#define PATH "inputs/"
int fileCounter = 0;


FILE *open_file(char *file)
{

    FILE *fp = fopen(file, "r");
    if (!fp)
    {
        perror("Errore nell'apertura del file");
        return NULL;
    }
    else
        return fp;
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

void process_file(char *path, page_frame *frames, int *bufferIndex, int numElements, int algorithm, pthread_mutex_t *mutex)
{
    //printf("Processando file: %s\n\n", path);
    // printf("File da aprire: %s\n", path);
    FILE *fp = open_file(path);
    if (fp == NULL)
    {
        perror("Errore durante l'elaborazione");
        return;
    }

    char *line = NULL;
    size_t len = 0;

    //printf("Algoritmo Scelto: %s", (algorithm == 1) ? "SecondChance\n\n" : "LRU\n\n");

    pthread_mutex_lock(mutex);
    printf("\n[THREAD %lu] Inizio esecuzione su file: %s\n", pthread_self(), path);    pthread_mutex_unlock(mutex);
    while (getline(&line, &len, fp) != -1)
    {
        int address = atoi(line);
        if (algorithm == 1)
        {
            secondChance(address, frames, bufferIndex, numElements);
        }
        else
            LRU(address, frames, bufferIndex, numElements);
    }

    free(line);
    fclose(fp);
}

/*
 * PRE: Ho indirizzi non nulli
 * POST: converto indirizzo di input alla pagina virtuale corrispondente
 */
int convert_AddressToPage(int address)
{
    return address / PAGE_DIM;
}

/*
 * Leggo la cartella contentente i file di input
 */

void pre_insert(Lista **ptr, char *val)
{
    Lista *tmpPtr = *ptr;
    *ptr = malloc(sizeof(Lista));
    if (*ptr == NULL)
    {
        perror("Malloc fallita");
        exit(1);
    }
    (*ptr)->value = strdup(val); // copia la stringa
    (*ptr)->next_ptr = tmpPtr;
}

Lista *read_directory(char *directory)
{
    DIR *d;
    Lista *percorsi = NULL;
    struct dirent *dir;

    d = opendir(directory);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (++fileCounter > 2)
            {
                char fullPath[512];
                snprintf(fullPath, sizeof(fullPath), "%s%s", PATH, dir->d_name);
                pre_insert(&percorsi, fullPath);
            }
        }
        closedir(d);

        Lista *ptr = percorsi;
        while (ptr != NULL)
        {
            // printf("Valore memorizzato: %s\n", ptr->value);
            ptr = ptr->next_ptr;
        }
    }
    else
    {
        perror("Errore nella lettura della cartella");
    }

    return percorsi;
}

/*
* PRE: address rappresenta la pagina da indirizzare, bufferIndex è puntatore ad un buffer circolare che scorre ciclicamente i frames, 
* numElements è il numero di pagine
* POST: Ritorna 1 se c'è stato un fageFault per sistituzione pagina o spazio libero, 0 se c'è stato un pageHit
*/
int secondChance(int address, page_frame *frames, int *bufferIndex, int numElements)
{
    int pageID = convert_AddressToPage(address);

    // Controllo se c'è la pagina in RAM (pageHit)
    for (int i = 0; i < numElements; i++)
    {
        if (frames[i].pageId == pageID)
        {
            frames[i].rBit = 1; // Setta il reference bit
            printf("Page Hit\n");
            return 0;
        }
    }

    // Dato che la RAM parte vuota potrei avere ancora spazi liberi
    for (int i = 0; i < numElements; i++)
    {
        if (frames[i].pageId == -1)
        {
            frames[i].pageId = pageID;
            frames[i].rBit = 1;
            frames[i].mBit = 0;
            printf("Page Fault per spazio libero\n");
            return 1; // Page fault, ma nessuna sostituzione
        }
    }

    printf("Page Fault, non presente in RAM\n");

    //Scorro finché non trovo la pagina da sostituire, Solo se devo sostuire l'indice del buffer va avanti
    while (1)
    {
        if (frames[*bufferIndex].rBit == 0)
        {
            // Pagina trovata per la sostituzione
            int oldPageID = frames[*bufferIndex].pageId;

            // Sostituisco la pagina
            frames[*bufferIndex].pageId = pageID;
            frames[*bufferIndex].rBit = 1;
            frames[*bufferIndex].mBit = 0;

            printf("Sostituisco pagina %i con pagina %i\n", oldPageID, pageID);

            *bufferIndex = (*bufferIndex + 1) % numElements;
            return 1;
        }
        else
        {
            //Rimetto il reference bit a 0
            frames[*bufferIndex].rBit = 0;
            *bufferIndex = (*bufferIndex + 1) % numElements;
        }
    }
}

/*
* PRE: address rappresenta la pagina da indirizzare, bufferIndex è puntatore ad un buffer circolare che scorre ciclicamente i frames, 
* numElements è il numero di pagine
* POST: Ritorna 1 se c'è stato un fageFault per sistituzione pagina o spazio libero, 0 se c'è stato un pageHit
*/
int LRU(int address, page_frame *frames, int *bufferIndex, int numElements)
{
    int pageID = convert_AddressToPage(address);

    // Controllo se c'è la pagina in RAM (pageHit)
    for (int i = 0; i < numElements; i++)
    {
        if (frames[i].pageId == pageID)
        {
            printf("Page Hit\n");
            return 0;
        }
    }

    // Dato che la RAM parte vuota potrei avere ancora spazi liberi
    for (int i = 0; i < numElements; i++)
    {
        if (frames[i].pageId == -1)
        {
            frames[i].pageId = pageID;
            printf("Page Fault per spazio libero\n");
            return 1; // Page fault, ma nessuna sostituzione
        }
    }

    printf("Page Fault, non presente in RAM\n");

    //Eseguo la politica
    while (1)
    {

        // Pagina trovata per la sostituzione
        int oldPageID = frames[*bufferIndex].pageId;

        frames[*bufferIndex].pageId = pageID;

        printf("Sostituisco pagina %i con pagina %i\n", oldPageID, pageID);

        *bufferIndex = (*bufferIndex + 1) % numElements;
        return 1;
    }
}

void* thread_process_file(void* arg) {
    ThreadArgs* args = (ThreadArgs*) arg;
    process_file(args->path, args->frames, args->bufferIndex, args->numElements, args->algorithm, args->frames_mutex);
    free(arg);
    pthread_exit(NULL);
}