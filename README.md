# CuriePower
CuriePower is a Power Management library for Curie based boards such as the Arduino101/Genuino101 and tinyTILE

#CuriePower API reference

##Functions

### ``CuriePower.doze()``

```
void CuriePower.doze()
```

Places the SoC in "doze" mode which simply switches the system clock to the internal 32Khz RTC oscillator

*Parameters*

none

*Return value*

none

### ``CuriePower.doze(int duration)``

```
void CuriePower.doze(int duration)
```


Places the SoC in "doze" mode which simply switches the system clock to the internal 32Khz RTC oscillator for `duration` in milliseconds


*Parameters*

1. `int duration` : amount in milliseconds to place the SoC in "doze" mode
none

*Return value*

none


#Tutorials
