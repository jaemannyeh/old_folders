
#include <stdint.h> // uint16_t
#include <stdbool.h>

// Listing 1: A simple yet effective debounce algorithm

#define CHECK_MSEC        5 // Read hardware every 5 msec
#define PRESS_MSEC       10 // Stable time before registering pressed
#define RELEASE_MSEC    100 // Stable time before registering released

// This function reads the key state from the hardware.
extern bool_t RawKeyPressed();

// This holds the debounced state of the key.
bool_t DebouncedKeyPress = false;

// Service routine called every CHECK_MSEC to
// debounce both edges
void DebounceSwitch1(bool_t *Key_changed, bool_t *Key_pressed)
{
    static uint8_t Count = RELEASE_MSEC / CHECK_MSEC;
    bool_t RawState;
    *Key_changed = false;
    *Key_pressed = DebouncedKeyPress;
    RawState = RawKeyPressed();
    if (RawState == DebouncedKeyPress) {
        // Set the timer which will allow a change from the current state.
        if (DebouncedKeyPress) Count = RELEASE_MSEC / CHECK_MSEC;
        else                 Count = PRESS_MSEC / CHECK_MSEC;
    } else {
        // Key has changed - wait for new state to become stable.
        if (--Count == 0) {
            // Timer expired - accept the change.
            DebouncedKeyPress = RawState;
            *Key_changed=true;
            *Key_pressed=DebouncedKeyPress;
            // And reset the timer.
            if (DebouncedKeyPress) Count = RELEASE_MSEC / CHECK_MSEC;
            else                 Count = PRESS_MSEC / CHECK_MSEC;
        }
    }
}

// Listing 2: An even simpler debounce routine
// Service routine called by a timer interrupt
bool_t DebounceSwitch2()
{
    static uint16_t State = 0; // Current debounce status
    
    State = (State<<1) | !RawKeyPressed() | 0xe000; // 1110 0000 0000 0000
    if (State == 0xf000)
        return TRUE; // 1111 0000 0000 0000
    return FALSE; 
}

// Listing 3: Code that debounces many switches at the same time

#define MAX_CHECKS 10	// # checks before a switch is debounced
uint8_t Debounced_State;	// Debounced state of the switches
uint8_t State[MAX_CHECKS];	// Array that maintains bounce status
uint8_t Index;	// Pointer into State
// Service routine called by a timer interrupt
void DebounceSwitch3()
{
    uint8_t i,j;
    State[Index]=RawKeyPressed();
    ++Index;
    j=0xff;
    for(i=0; i<MAX_CHECKS-1;i++)j=j & State[i];
    Debounced_State=Debounced_State ^ j;
    if(Index>=MAX_CHECKS)Index=0;
}

#if 0
Don't forget to initialize State and Index to zero.

I prefer a less computationally intensive alternative that splits
DebounceSwitch3() into these two routines: one routine driven by the timer tick
merely accumulates data into array State. Another function,
Whats_Da_Switches_Now(), ANDs and XORs as described but only when the system
needs to know the switches' status.

http://www.embedded.com/electronics-blogs/break-points/4024981/My-favorite-software-debouncers
#endif