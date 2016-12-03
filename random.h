// Credit: coranc/tonc
// from tonc_core.h/.c
// A Quick (and dirty) random number generator and its seeder

extern int sqran(int seed);
extern int qran();
INLINE int qran_range(int min, int max)
{    return (qran()*(max-min)>>13)+min;     } // I guess we shift by the range?? idk coranc didn't explain but his random sucked so I wrote my own.

