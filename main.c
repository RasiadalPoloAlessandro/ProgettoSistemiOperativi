#include <stdio.h>
#include <stdlib.h>
#include "files/headers/utility.h"

int main(int argc, char **argv)
{

    int pages = atoi(argv[1]); // atoi serve per convertire in int
    pthread_mutex_t frames_mutex = PTHREAD_MUTEX_INITIALIZER;
    page_frame *frames = malloc(pages * sizeof(page_frame));

    // Creo una lista che memorizza i percorsi dei file di input
    Lista *processes = read_directory(argv[2]);
    int m = 0;
    int algorithm = atoi(argv[3]);
    for (int i = 0; i < pages; i++)
    {
        frames[i].pageId = -1;
        frames[i].rBit = 0;
        frames[i].mBit = 0;
    }

    /*int tId = 0;
    while(processes != NULL){

        process_file(processes->value, frames, &m, pages, algorithm);
        processes = processes->next_ptr;
    }*/

    Lista *temp = processes;
    int fileCounter = 0;
    pthread_t tId[fileCounter]; // o malloc se dinamico
    Lista *current = processes;
    int i = 0;
    while (current != NULL)
    {
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        args->path = strdup(current->value);
        args->frames = frames;
        args->bufferIndex = &m;
        args->numElements = pages;
        args->algorithm = algorithm;
        args->frames_mutex = &frames_mutex;

        pthread_create(&tId[i++], NULL, thread_process_file, (void *)args);

        current = current->next_ptr;
    }

    // Aspetta la fine dei thread
    for (int j = 0; j < i; j++)
    {
        pthread_join(tId[j], NULL);
    }

    return 0;
}