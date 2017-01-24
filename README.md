# CuriePower
CuriePower is a Power Management library for Curie based boards such as the Arduino101/Genuino101 and tinyTILE

#CuriePower API reference

##Functions

### ``CuriePower.doze()``

```
void CuriePower.doze()
```

Places the SoC in "doze" mode which simply switches the system clock to the internal 32Khz RTC oscillator.

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

Work In Progress
