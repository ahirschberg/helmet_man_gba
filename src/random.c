#include "myLib.h"
#include "random.h"
#include "random_data.h"

// Credit: corcranc
// from tonc_core.h/.c
// A Quick (and dirty) random number generator and its seeder
// Seed routine
int __qran_seed= 42;     // Seed / rnd holder
int sqran(int seed)
{
    int old= __qran_seed;
    __qran_seed= seed;
    return old;
}

int qran() {
    __qran_seed = 1664525*__qran_seed+1013904223;
    return rand_nums[(__qran_seed & RAND_NUMS_MASK)];
}

// this random generator sucks.
/* int qran() */
/* { */
/*     __qran_seed= 1664525*__qran_seed+1013904223; */
/*     return (__qran_seed>>16) & 0x7FFF; */
/* } */

