#include <stdio.h>
#include <stdlib.h>

#define INLINE static inline

#include "random.h"
#include "random_data.h"

int results[1000];

int main() {
    sqran(11211432);
    const int min = -1;
    const int max = 2;
    for (int i = 0; i < 1000; i++) {
        results[i] = qran_range(min, max);
    }

    for (int i = 0; i < 1000; i++) {
        if (results[i] < min || results[i] >= max) {
            printf("Oops: %d out of range.\n", results[i]);
            return 1;
        }
    }

    printf("looks good.\n");
    return 0;
}
