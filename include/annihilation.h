#ifndef ANNIHILATION_H
#define ANNIHILATION_H

#define SCREEN_WIDTH    1920   
#define SCREEN_HEIGHT   1080

#define FPS_LIMIT       30

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef unsigned char asciiChar;

#define THREAD_OK   0

/*** MISC ***/

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif