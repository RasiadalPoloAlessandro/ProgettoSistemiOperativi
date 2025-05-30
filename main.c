#include <stdio.h>
#include <stdlib.h>
#include "files/headers/utility.h"


int main(int argc, char **argv)
{

    int pageMiss = 0, pageHit = 0;
    int* pgMs = &pageMiss;
    int* pgHt = &pageHit;
    int pages = atoi(argv[1]); //atoi serve per convertire in int
    page_frame *frames = malloc(pages * sizeof(page_frame));
    
    //Creo una lista che memorizza i percorsi dei file di input
    Lista* processes = read_directory(argv[2]);
    int m = 0;
    int algorithm = 0;
    for (int i = 0; i < pages; i++) {
        frames[i].pageId = -1;
        frames[i].rBit = 0;
        frames[i].mBit = 0;
    }
    Lista* tmp = processes;
    printf("Esecuzione di LRU: \n");

    while(tmp != NULL){

        process_file(tmp->value, frames, &m, pages, algorithm, pgHt, pgMs);
        tmp = tmp->next_ptr;
    }

    print_stats(algorithm, pgHt, pgMs);
    algorithm = 1;
    printf("Esecuzione di Second Chance: \n");
    while(processes != NULL){

        process_file(processes->value, frames, &m, pages, algorithm, pgHt, pgMs);
        processes = processes->next_ptr;
    }
    print_stats(algorithm, pgHt, pgMs);


    return 0;
}