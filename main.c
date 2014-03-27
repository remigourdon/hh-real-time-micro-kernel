/**
 * @file kernel.h
 * @brief Kernel definitions
 */

#include "kernel.h"

int main(void) {
    init_kernel();

    // Free memory allocated for the lists
    // Will not be reached normally
    free(readyList->pHead->pTask);
    free(readyList->pHead);
    free(readyList->pTail->pTask);
    free(readyList->pTail);
    free(readyList);

    free(timerList->pHead->pTask);
    free(timerList->pHead);
    free(timerList->pTail->pTask);
    free(timerList->pTail);
    free(timerList);

    free(waitingList->pHead->pTask);
    free(waitingList->pHead);
    free(waitingList->pTail->pTask);
    free(waitingList->pTail);
    free(waitingList);

    return 0;
}