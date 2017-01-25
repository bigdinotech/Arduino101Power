# CuriePower
CuriePower is a Power Management library for Curie based boards such as the Arduino101/Genuino101 and tinyTILE

#CuriePower API reference

##Functions

### ``CuriePower.doze()``

```
void CuriePower.doze()
```

Places the SoC in "doze" mode which simply switches the system clock to the internal 32.768 kHz RTC oscillator.

*Parameters*

none

*Return value*

none

### ``CuriePower.doze()``

```
void CuriePower.doze(int duration)
```


Places the SoC in "doze" mode which simply switches the system clock to the internal 32Khz RTC oscillator for `duration` milliseconds


*Parameters*

1. `int duration` : amount in milliseconds to place the SoC in "doze" mode
none

*Return value*

none

### ``CuriePower.idle()``

```
void CuriePower.idle()
```

Places the SoC into "doze" mode then enters an infinite loop, effectively stopping all operations until a wake interrupts is generated.

*Parameters*

none

*Return value*

none

### ``CuriePower.idle()``

```
void CuriePower.idle(int duration)
```


Places the SoC in "doze" mode and enters an infinite loop for `duration` milliseconds


*Parameters*

1. `int duration` : amount in milliseconds to place the SoC in an idle state
none

*Return value*

none

### ``CuriePower.sleep()``

```
void CuriePower.sleep()
```

Places the SoC into a sleep state, stopping all operations, until a wake interrupt is generated

*Parameters*

none

*Return value*

none

### ``CuriePower.sleep()``

```
void CuriePower.sleep(int duration)
```


Places the SoC into a sleep state for `duration` milliseconds


*Parameters*

1. `int duration` : amount in milliseconds to place the SoC into sleep
none

*Return value*

none

### ``CuriePower.deepSleep()``

```
void CuriePower.deepSleep()
```

Places the SoC into a deep sleep state, stopping all operations, until a wake interrupt is generated

*Parameters*

none

*Return value*

none

### ``CuriePower.deepSleep()``

```
void CuriePower.deepSleep(int duration)
```


Places the SoC into a deep sleep state for `duration` milliseconds


*Parameters*

1. `int duration` : amount in milliseconds to place the SoC into deep sleep
none

*Return value*

none

### ``CuriePower.attachInterruptWakeup()``

```
void CuriePower.attachInterruptWakeup(uint32_t pin, voidFuncPtr callback, uint32_t mode)
```

Registers a wake source into the wakesource table with the call back function `callback` with `pin` as the interrupt source. 

*Parameters*

none

*Return value*

none

### ``CuriePower.dettachInterruptWakeup()``

```
void CuriePower.dettachInterruptWakeup(uint32_t pin)
```

Removes a wake source from the wakesource table with the interrupt source of `pin`. 

*Parameters*

none

*Return value*

none

#Tutorials

##Tutorial #1: TimedWakeup Example

This sketch demonstrates the simplest way to use the CuriePower library. It blinks the LED a few times, goes to sleep for a certain amount then goes back at the start of loop()

```cpp
#include <Power.h>

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  for(int i = 0; i < 5; i++)
  { 
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
  PM.sleep(1000);
}
```

The line of code that is most interesting here is:
```cpp
PM.sleep(1000);
```
This puts the SoC into sleep, drawing significantly less power, for 1000ms or 1s.

In what situations is this most useful?

Lets says you have a battery powered project that reads a sensor value once every second and saves it to an SD card.
Simply using delay() will work but you will notice that you will run out of battery pretty fast. That is becasue even though the code is not doing much insude delay, it is still running everything at full clock speed and all peripeherals are on.
When we put the SoC to sleep, several things are turned off like most of the peripherals. voltage rails, and clocks. Basically, it drwas much less power and no code is running until a wake interrupt is generated.

