    /*
    * SimpleTimer.h
    *
    * Templated implementation of SimpleTimer library
    * For use with member-function callbacks
    *
    * Based off of:
    * SimpleTimer - A timer library for Arduino.
    * Author: mromani@ottotecnica.com
    * Copyright (c) 2010 OTTOTECNICA Italy
    *
    * Modified by:
    * William A. Clark
    * clark@fuseproject.com
    * Github: indicatesvoid
    *
    * This library is free software; you can redistribute it
    * and/or modify it under the terms of the GNU Lesser
    * General Public License as published by the Free Software
    * Foundation; either version 2.1 of the License, or (at
    * your option) any later version.
    *
    * This library is distributed in the hope that it will
    * be useful, but WITHOUT ANY WARRANTY; without even the
    * implied warranty of MERCHANTABILITY or FITNESS FOR A
    * PARTICULAR PURPOSE.  See the GNU Lesser General Public
    * License for more details.
    *
    * You should have received a copy of the GNU Lesser
    * General Public License along with this library; if not,
    * write to the Free Software Foundation, Inc.,
    * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
    *
    */


    #ifndef SIMPLETIMER_H
    #define SIMPLETIMER_H

    #if defined(ARDUINO) && ARDUINO >= 100
    #include <Arduino.h>
    #else
    #include <WProgram.h>
    #endif

    //typedef void (*timer_callback)(void);

    // deferred call constants
    const static int DEFCALL_DONTRUN = 0;       // don't call the callback function
    const static int DEFCALL_RUNONLY = 1;       // call the callback function but don't delete the timer
    const static int DEFCALL_RUNANDDEL = 2;      // call the callback function and delete the timer

template <class T> class SimpleTimer {

    public:
    typedef void(T::*methodPtr_t)(void);
    
    // maximum number of timers
    const static int MAX_TIMERS = 10;

    // setTimer() constants
    const static int RUN_FOREVER = 0;
    const static int RUN_ONCE = 1;

    // constructor
    SimpleTimer() {
        unsigned long current_millis = millis();

        for (int i = 0; i < MAX_TIMERS; i++) {
            enabled[i] = false;
            callbacks[i] = 0;                   // if the callback pointer is zero, the slot is free, i.e. doesn't "contain" any timer
            prev_millis[i] = current_millis;
            numRuns[i] = 0;
        }

        numTimers = 0;
    }

    // this function must be called inside loop()
    void run(T* instancePtr) {
        int i;
        unsigned long current_millis;

        // get current time
        current_millis = millis();

        for (i = 0; i < MAX_TIMERS; i++) {

            toBeCalled[i] = DEFCALL_DONTRUN;

            // no callback == no timer, i.e. jump over empty slots
            if (callbacks[i]) {

                // is it time to process this timer ?
                if (current_millis - prev_millis[i] >= delays[i]) {

                    // update time
                    prev_millis[i] = current_millis;

                    // check if the timer callback has to be executed
                    if (enabled[i]) {

                        // "run forever" timers must always be executed
                        if (maxNumRuns[i] == RUN_FOREVER) toBeCalled[i] = DEFCALL_RUNONLY;

                        // other timers get executed the specified number of times
                        else if (numRuns[i] < maxNumRuns[i]) {
                            toBeCalled[i] = DEFCALL_RUNONLY;
                            numRuns[i]++;

                            // after the last run, delete the timer
                            if (numRuns[i] >= maxNumRuns[i]) {
                                toBeCalled[i] = DEFCALL_RUNANDDEL;
                            }
                        }
                    }
                }
            }
        }

        for (i = 0; i < MAX_TIMERS; i++) {
            switch(toBeCalled[i]) {
                case DEFCALL_DONTRUN:
                    break;

                case DEFCALL_RUNONLY:
                    (instancePtr->*callbacks[i])();
                    break;

                case DEFCALL_RUNANDDEL:
                    (instancePtr->*callbacks[i])();
                    deleteTimer(i);
                    break;
            }
        }
    } 

    // call function f once after d milliseconds
    int setTimeout(long d, T* instancePtr, void(T::*methodPtr)()) {
        return setTimer(d, instancePtr, methodPtr, RUN_ONCE);
    }

    // call function f every d milliseconds
    int setInterval(long d, T* instancePtr, void(T::*methodPtr)()) {
        return setTimer(d, instancePtr, methodPtr, RUN_FOREVER);
    }
    
    // call function f every d milliseconds for n times
    int setTimer(long d, T* instancePtr , void(T::*methodPtr)(), int n) {
        int freeTimer;

        freeTimer = findFirstFreeSlot();
        if (freeTimer < 0) return -1;

        delays[freeTimer] = d;
        methodPtr_t method = methodPtr;
       
        callbacks[freeTimer] = methodPtr;
        maxNumRuns[freeTimer] = n;
        enabled[freeTimer] = true;
        prev_millis[freeTimer] = millis();

        numTimers++;

        return freeTimer;
    }

    // destroy the specified timer
    void deleteTimer(int numTimer) {
        if (numTimer >= MAX_TIMERS) return;

        // nothing to delete if no timers are in use
        if (numTimers == 0) return;

        callbacks[numTimer] = 0;
        enabled[numTimer] = false;
        delays[numTimer] = 0;
        numRuns[numTimer] = 0;

        // update number of timers
        numTimers--;
    }

    // restart the specified timer
    // function contributed by code@rowansimms.com
    void restartTimer(int numTimer) {
        if (numTimer >= MAX_TIMERS) return;
        prev_millis[numTimer] = millis();
    }

    // returns true if the specified timer is enabled
    boolean isEnabled(int numTimer) {
        if (numTimer >= MAX_TIMERS) return false;
        return enabled[numTimer];
    }

    // enables the specified timer
    void enable(int numTimer) {
        if (numTimer >= MAX_TIMERS) return;
        enabled[numTimer] = true;
    }

    // disables the specified timer
    void disable(int numTimer) {
        if (numTimer >= MAX_TIMERS) return;
        enabled[numTimer] = false;
    }

    // enables the specified timer if it's currently disabled,
    // and vice-versa
    void toggle(int numTimer) {
        if (numTimer >= MAX_TIMERS) return;
        enabled[numTimer] = !enabled[numTimer];
    }

    // returns the number of used timers
    int getNumTimers() {
        return numTimers;
    }

    // returns the number of available timers
    int getNumAvailableTimers() { return MAX_TIMERS - numTimers; };

    private:

    // find the first available slot
    int findFirstFreeSlot() {
        int i;

        // all slots are used
        if (numTimers >= MAX_TIMERS) return -1;

        // return the first slot with no callback (i.e. free)
        for (i = 0; i < MAX_TIMERS; i++) if (callbacks[i] == 0) return i;

        // we should never reach this point...
        return -1;
    }

    // value returned by the millis() function
    // in the previous run() call
    unsigned long prev_millis[MAX_TIMERS];

    // pointers to the callback functions
    methodPtr_t callbacks[MAX_TIMERS];

    // delay values
    long delays[MAX_TIMERS];

    // number of runs to be executed for each timer
    int maxNumRuns[MAX_TIMERS];

    // number of executed runs for each timer
    int numRuns[MAX_TIMERS];

    // which timers are enabled
    boolean enabled[MAX_TIMERS];

    // deferred function call (sort of) - N.B.: this array is only used in run()
    int toBeCalled[MAX_TIMERS];

    // actual number of timers in use
    int numTimers;
};

    #endif