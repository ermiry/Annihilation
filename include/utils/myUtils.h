#ifndef MY_UTILS_H
#define MY_UTILS_H

extern void random_set_seed (unsigned int seed);
extern int random_int_in_range (int min, int max);

extern int clamp_int (int val, int min, int max);
extern int abs_int (int value);

extern char *createString (const char *stringWithFormat, ...);

#endif