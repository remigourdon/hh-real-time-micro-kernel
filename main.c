/**
 * @file kernel.h
 * @brief Kernel definitions
 */

#include "kernel.h"

int main(void) {
    init_kernel();

    // Free memory allocated for the lists
    // Will not be reached normally
    destroy_list(readyList);
    destroy_list(timerList);
    destroy_list(waitingList);

    return 0;
}