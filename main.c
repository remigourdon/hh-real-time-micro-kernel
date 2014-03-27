/**
 * @file kernel.h
 * @brief Kernel definitions
 */

#include "kernel.h"

int main(void) {
    init_kernel();

    // Free memory allocated for the lists
    // Will not be reached normally
    free(readyList->pHead);
    free(readyList->pTail);
    free(readyList);

    free(timerList->pHead);
    free(timerList->pTail);
    free(timerList);

    free(waitingList->pHead);
    free(waitingList->pTail);
    free(waitingList);

    return 0;
}