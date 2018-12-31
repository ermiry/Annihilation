#ifndef MY_UTILS_H
#define MY_UTILS_H

extern void random_set_seed (unsigned int seed);
extern int random_int_in_range (int min, int max);

extern char *createString (const char *stringWithFormat, ...);

#endif