    #include <stdio.h>
    #include <stdlib.h>
    #include "headers/utility.h"
    #include <string.h>
    #define PAGE_DIM 4096
    #define PATH "inputs/"
    int fileCounter = 0;

    // Implementa la struttura della memoria fisica (suddivisa in frame di pagina)


    FILE *open_file(char *file)
    {

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

    void process_file(char* path, page_frame *frames, int *bufferIndex, int numElements, int algorithm)
    {
        printf("Processando file: %s\n", path);
        //printf("File da aprire: %s\n", path);
        FILE *fp = open_file(path);
        if (fp == NULL){
            perror("Errore durante l'elaborazione");
            return;
        } 

        char *line = NULL;
        size_t len = 0;

        while (getline(&line, &len, fp) != -1)
        {
            int address = atoi(line);
            if(algorithm){
                secondChance(address, frames, bufferIndex, numElements);
            }
            else
                LRU(address, frames, bufferIndex, numElements);

        }

        free(line);
        fclose(fp);
    }


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
                //printf("Valore memorizzato: %s\n", ptr->value);
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
    * PRE: Stream di lettura su un file non nullo, memoria che implementa i bit necessari per applicare l'algoritmo
    * POST: Ritorna 1 se c'è stato un PageFault, 0 se invece pageHit
    */
    int secondChance(int address, page_frame *frames, int *bufferIndex, int numElements)
    {
        int pageID = convert_AddressToPage(address);
        
        // Controllo se c'è la pagina in RAM (pageHit)
        for (int i = 0; i < numElements; i++)
        {
            if (frames[i].pageId == pageID)
            {
                frames[i].rBit = 1;  // Setta il reference bit
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
        
        // Implementazione corretta dell'algoritmo Second Chance
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
                
                // Aggiorna il puntatore per la prossima volta
                *bufferIndex = (*bufferIndex + 1) % numElements;
                return 1;
            }
            else
            {
                // Dai una seconda possibilità: resetta R-bit e vai avanti
                frames[*bufferIndex].rBit = 0;
                *bufferIndex = (*bufferIndex + 1) % numElements;
            }
        }
    }

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
        
        // Implementazione corretta dell'algoritmo Second Chance
        while (1)
        {

                // Pagina trovata per la sostituzione
                int oldPageID = frames[*bufferIndex].pageId;
                
                // Sostituisco la pagina
                frames[*bufferIndex].pageId = pageID;
                
                printf("Sostituisco pagina %i con pagina %i\n", oldPageID, pageID);
                
                // Aggiorna il puntatore per la prossima volta
                *bufferIndex = (*bufferIndex + 1) % numElements;
                return 1;
            

        }
    }