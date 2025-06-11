#include <stdio.h>
#include <stdlib.h>
#include "headers/utility.h"
#include <string.h>
#define PAGE_DIM 4096
#define PATH "inputs/"
int fileCounter = 0;
//Uso una variabile intera per rappresentare il tempo di accesso, uso static per utilizzare la variabile SOLO in questo file (una specie di private)
static int global_time = 0;

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


int check_raw(char** raw) {
    char *end;
    strtod(*raw, &end);

    //Salto spazi bianchi o altre condizioni non valide
    while (*end == ' ' || *end == '\t' || *end == '\n')
        end++;

    if (*end == '\0') {
        return 0;
    } else {
        fprintf(stderr, "Warning: riga saltata perché non valida: %s\n", *raw);
        return -1;
    }
}

void process_file(char *path, page_frame *frames, int *bufferIndex, int numElements, int algorithm, int *pgHt, int *pgMs, pthread_mutex_t *mutex)
{
    // printf("Processando file: %s\n\n", path);
    //  printf("File da aprire: %s\n", path);
    FILE *fp = open_file(path);
    if (fp == NULL)
    {
        perror("Errore durante l'elaborazione");
        return;
    }

    char *line = NULL;
    size_t len = 0;
    int fault = 0;

    // printf("Algoritmo Scelto: %s", (algorithm == 1) ? "SecondChance\n\n" : "LRU\n\n");

    while (getline(&line, &len, fp) != -1)
    {
        if (check_raw(&line) == -1)
            continue;

        int address = atoi(line);
        pthread_mutex_lock(mutex);
        if (algorithm == 1)
        {
            fault = secondChance(address, frames, bufferIndex, numElements);
        }
        else
            fault = LRU(address, frames, bufferIndex, numElements);

        if (fault == 0)
            (*pgHt)++;
        else
            (*pgMs)++;
        pthread_mutex_unlock(mutex);
    }

    free(line);
    fclose(fp);
}

int print_stats(int algoritmo, int *pHit, int *pFault)
{
    FILE *fp = fopen("output.txt", "a");


    if (algoritmo == 1)
    {
        fprintf(fp,"== SECOND CHANCE TERMINATO ==\n");
        fprintf(fp, "Page hit totali : %d_secondChance\n", *pHit);
        fprintf(fp, "Page fault totali : %d_secondChance\n", *pFault);
        fprintf(fp, "Fault rate complessivo : %d%c_secondChance\n", (*pFault * 100) / (*pHit + (*pFault)), '%');
    }
    else
    {
        fprintf(fp, "== LRU TERMINATO ==\n");
        fprintf(fp, "Page hit totali : %d_LRU\n", *pHit);
        fprintf(fp, "Page fault totali : %d_LRU\n", *pFault);
        fprintf(fp, "Fault rate complessivo : %d%c_LRU\n", (*pFault * 100) / (*pHit + (*pFault)), '%');
    }

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
    (*ptr)->value = strdup(val); //strdup serve per copiare la stringa
    (*ptr)->next_ptr = tmpPtr;
}
/*
* Leggo il contenuto di una cartella
*/
Lista *read_directory(char *directory, int *fileCounter)
{
    DIR *d;
    Lista *percorsi = NULL;
    struct dirent *dir;

    d = opendir(directory);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            //non tengo conto di . e .. per il conteggio
            if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
            {
                char fullPath[512];
                snprintf(fullPath, sizeof(fullPath), "%s%s", PATH, dir->d_name);
                pre_insert(&percorsi, fullPath);
                (*fileCounter)++;
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
            //printf("Page Hit\n");
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
            //printf("Page Fault per spazio libero\n");
            return 1; // Page fault, ma nessuna sostituzione
        }
    }

    //printf("Page Fault, non presente in RAM\n");

    // Scorro finché non trovo la pagina da sostituire, Solo se devo sostuire l'indice del buffer va avanti
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

            //printf("Sostituisco pagina %i con pagina %i\n", oldPageID, pageID);

            *bufferIndex = (*bufferIndex + 1) % numElements;
            return 1;
        }
        else
        {
            // Rimetto il reference bit a 0
            frames[*bufferIndex].rBit = 0;
            *bufferIndex = (*bufferIndex + 1) % numElements;
        }
    }
}

/*
 * PRE: address rappresenta la pagina da indirizzare, bufferIndex è puntatore ad un buffer circolare che scorre ciclicamente i frames,
 * numElements è il numero di pagine. Per applicare l'algoritmo utilizzo una variabile globale intera che rappresenta il tempo di accesso
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
            frames[i].lastAccessed = ++global_time;
            return 0;
        }
    }
    
    // Cerco se c'è spazio in RAM
    for (int i = 0; i < numElements; i++)
    {
        if (frames[i].pageId == -1)
        {
            frames[i].pageId = pageID;
            frames[i].lastAccessed = ++global_time;
            return 1;
        }
    }
    
    // Trovo la pagina usata da meno tempo
    int lru_index = 0;
    int min_time = frames[0].lastAccessed;
    
    for (int i = 1; i < numElements; i++)
    {
        if (frames[i].lastAccessed < min_time)
        {
            min_time = frames[i].lastAccessed;
            lru_index = i;
        }
    }
    
    // Sostituisco la pagina
    frames[lru_index].pageId = pageID;
    frames[lru_index].lastAccessed = ++global_time;
    
    return 1;
}

/*
* I prendono come parametro un puntatore a funzione, ne creo una che richiami la funzione di mio interesse passando tutti i parametri necessari
*/
void *thread_process_file(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    process_file(args->path, args->frames, args->bufferIndex, args->numElements, args->algorithm, args->pHit, args->pFault, args->frames_mutex);
    free(arg);
    pthread_exit(NULL);
}

int write_file(char* path){

    FILE *fp = open_file(path);

    
}