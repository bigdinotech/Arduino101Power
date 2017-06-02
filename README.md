# CuriePower
CuriePower is a Power Management library for Curie based boards such as the Arduino101/Genuino101 and tinyTILE

# CuriePower API reference

## Functions

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

# Tutorials

## Tutorial #1: TimedWakeup Example

This sketch demonstrates the simplest way to use the Arduino101Power library. It blinks the LED a few times, goes to sleep for a certain amount then goes back at the start of loop()

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
Simply using delay() will work but you will notice that you will run out of battery pretty fast. That is becasue even though the code is not doing much inside delay, it is still running everything at full clock speed and all peripeherals are turned on.
When we put the SoC to sleep, several things are turned off. This includes most of the peripherals. voltage rails, and some clocks. Basically, it draws much less power and no code is running until a wake interrupt is generated.

Many Arduino projects typically have a loop where you read a sensor, do something with that reading, and then delaying for a set amount of time.
In most cases, reading the sensor and doing something with that reading takes very little time, much smaller than the delay duration. This means that we are wasting a lot of power inside the delay doing nothing.
By placing the SoC to sleep instead of just waiting inside the delay, we can save a considerable amount of power in most applications.


## Tutorial #2: WakeFromIMU Example

This sketch uses Arduino101Power library and the CurieIMU library together and demonstrates the use of an interrupt to wake the Curie SoC from sleep.

```cpp
#include <Power.h>
#include "CurieIMU.h"


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  CurieIMU.begin();
  CurieIMU.attachInterrupt(wakeup);
  CurieIMU.setDetectionThreshold(CURIE_IMU_MOTION, 20);      // 100mg
  CurieIMU.setDetectionDuration(CURIE_IMU_MOTION, 10);       // trigger times of consecutive slope data points
  CurieIMU.interrupts(CURIE_IMU_MOTION);
  
}

void loop() {
  PM.sleep();
  digitalWrite(LED_BUILTIN, HIGH);
  delay(3000);
  digitalWrite(LED_BUILTIN, LOW);
}

void wakeup()
{
  PM.wakeFromDoze();
  // This function will be called once on device wakeup
  // You can do some little operations here (like changing variables which will be used in the loop)
  // Remember to avoid calling delay() and long running functions since this functions executes in interrupt context
}
```

If you look at the beginning of loop() you will see:
```cpp
PM.sleep();
```
This line of code simple puts the SoC to sleep and stays in that state until it is waken up by an interrupt.
If no interrupt triggers the SoC to wakeup, it will simply stay in a low power sleeps state forever(or until the battery runs out).
This is specifically useful in applications that are not periodic, like in this example sketch where the SoC stays at a sleep state until the Arduino101 detects motion, or more precisely the Bosch BMI160 sensor detects the motion and triggers an interrupt to wake the Curie Soc from sleep.

Inside setup() we have:
```cpp
CurieIMU.attachInterrupt(wakeup);
CurieIMU.setDetectionThreshold(CURIE_IMU_MOTION, 20);      // 100mg
CurieIMU.setDetectionDuration(CURIE_IMU_MOTION, 10);       // trigger times of consecutive slope data points
CurieIMU.interrupts(CURIE_IMU_MOTION);
```
This lines of code simply attaches a method called wakeup() which is called whenever motion is detected. Since the interrupt signal generated but Bosch BMI160 sensor is already internally connected to AON(Always On) interrupt of the SoC it automatically wakes the SoC from sleep. 
However, since we are using the attachInterrupt() method of the CurieIMU Library instead of the one from the Arduino101Power Library we still need to do one more thing after the SoC is taken out of a sleep state.

Inside the wakeup() method:
```cpp
PM.wakeFromDoze();
```
This line of code simply takes the SoC out of the Doze state, switching it from using the internal RTC 32.768 KHz as the main clock, back to the 16Mhz oscillator.
At this point the SoC runs back at full speed ready to do stuff quickly and then go back to sleep to save power.
