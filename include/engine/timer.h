#ifndef TIMER_H
#define TIMER_H

#include <stdbool.h>

#include "annihilation.h"

typedef struct Timer {

    u32 startTicks;     // the clock time when the timer started
    u32 pausedTicks;    // the ticks stored when the timer was paused

    bool isPaused;
    bool started;

} Timer;

extern Timer *timer_new (void);
extern void timer_destroy (Timer *timer);

extern void timer_start (Timer *timer);
extern void timer_stop (Timer *timer);
extern void timer_pause (Timer *timer);
extern void timer_unpause (Timer *timer);

extern u32 timer_getTicks (Timer *timer);

#endif