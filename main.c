#include <stdio.h>
#include <stdlib.h>
#include "files/headers/utility.h"
/*
 * PRE: Ho indirizzi non nulli
 * POST: converto indirizzo di input alla pagina virtuale corrispondente
 */


int main(int argc, char **argv)
{

    int pages = atoi(argv[1]); //atoi serve per convertire in int
    page_frame *frames = malloc(pages * sizeof(page_frame));
    
    //Creo una lista che memorizza i percorsi dei file di input
    Lista* processes = read_directory(argv[2]);
    int m = 0;

    for (int i = 0; i < pages; i++) {
        frames[i].pageId = -1;
        frames[i].rBit = 0;
        frames[i].mBit = 0;
    }

    while(processes != NULL){

        process_file(processes->value, frames, &m, pages);
        processes = processes->next_ptr;
    }

    return 0;
}