#include <stdio.h>
#include <stdlib.h>
#define PAGE_DIM 4096

int page_fault = 0, page_hit = 0;

//Implementa la struttura della memoria fisica (suddivisa in frame di pagina)
typedef struct page_frame page_frame;
struct page_frame
{
    int pageId;
    int rBit;
    int mBit;
};

/*
 * PRE: Ho indirizzi non nulli
 * POST: converto indirizzo di input alla pagina virtuale corrispondente
 */

int convert_Address(int address);

int main(int argc, char *argv)
{

    int pages = argv[0];
    page_frame *frames = malloc(pages * sizeof(page_frame));

    printf("%li\n", sizeof(frames));

    return 0;
}
