#include <stdio.h>
#include <stdlib.h>
#include "files/headers/utility.h"

int main(int argc, char **argv)
{

    int pages = atoi(argv[1]); // atoi serve per convertire in int
    int pageMiss = 0, pageHit = 0;
    int *pgMs = &pageMiss;
    int *pgHt = &pageHit;
    pthread_mutex_t frames_mutex = PTHREAD_MUTEX_INITIALIZER;
    page_frame *frames = malloc(pages * sizeof(page_frame));
    int fileCounter = 0;

    // Creo una lista che memorizza i percorsi dei file di input
    Lista *processes = read_directory(argv[2], &fileCounter);
    int m = 0;
    for (int round = 0; round < 2; round++)
    {
        int algorithm = round; // 0: LRU, 1: SecondChance

        // Reset RAM
        for (int i = 0; i < pages; i++)
        {
            frames[i].pageId = -1;
            frames[i].rBit = 0;
            frames[i].mBit = 0;
        }
        pthread_t *tId = malloc(fileCounter * sizeof(pthread_t));
        int bufferIndex = 0;
        pageMiss = 0;
        pageHit = 0;

        Lista *current = processes;
        int i = 0;

        // Avvia nuovi thread per ogni file
        while (current != NULL)
        {
            ThreadArgs *args = malloc(sizeof(ThreadArgs));
            args->path = strdup(current->value);
            args->frames = frames;
            args->bufferIndex = &bufferIndex;
            args->numElements = pages;
            args->algorithm = algorithm;
            args->pFault = &pageMiss;
            args->pHit = &pageHit;
            args->frames_mutex = &frames_mutex;

            pthread_create(&tId[i++], NULL, thread_process_file, (void *)args);
            current = current->next_ptr;
        }

        for (int j = 0; j < i; j++)
        {
            pthread_join(tId[j], NULL);
        }

        printf("\n--- RISULTATI PER %s ---\n", (algorithm == 0) ? "LRU" : "Second Chance");
        print_stats(algorithm, &pageHit, &pageMiss);
    }

    return 0;
}