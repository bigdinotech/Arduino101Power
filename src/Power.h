#define OSC0_CFG1       0xB0800008
#define CCU_SS_PERIPH_CLK_GATE_CTL  0xB0800028
#define CCU_LP_CLK_CTL  0xB080002C
#define CCU_SYS_CLK_CTL 0xB0800038
#define P_LVL2          0xB0800504
#define PM1C            0xB0800518
#define SLP_CFG         0xB0800550
#define SS_STS          0xB0800604

#define AONC_CNT        0xB0800700
#define AONC_CFG        0xB0800704
#define AONPT_CNT       0xB0800708
#define AONPT_STAT      0xB080070C
#define AONPT_CTRL      0xB0800710
#define AONPT_CFG       0xB0800714

#define USB_PLL_CFG0    0xB0800014
#define USB_PHY_CFG0    0xB0800800

#define RTC_CCVR        0xB0000400
#define RTC_CMR         0xB0000404
#define RTC_CCR         0xB000040C
#define RTC_EOI         0xB0000418
#define RTC_MASK_INT    0xB0800478

#define AON_TIMER_MASK_INT      0xB08004C8
#define AON_GPIO_MASK_INT       0xB08004D4

#define AON_GPIO_SWPORTA_DR         0xB0800B00
#define AON_GPIO_SWPORTA_DDR        0xB0800B04
#define AON_GPIO_SWPORTA_CTL        0xB0800B08
#define AON_GPIO_INTEN              0xB0800B30
#define AON_GPIO_INTMASK            0xB0800B34
#define AON_GPIO_INTTYPE_LEVEL      0xB0800B38
#define AON_GPIO_INT_POL            0xB0800B3C
#define AON_GPIO_DEBOUNCE           0xB0888B48
#define AON_GPIO_PORTA_EOI           0xB0800B4C


#define OSCTRIM_ADDR    0xffffe1f8

enum wakeSource{
    AON_GPIO0 = 100,
    AON_GPIO1 = 101,
    AON_GPIO2 = 102,
    AON_GPIO3 = 103,
    AON_GPIO4 = 104,
    AON_GPIO5 = 105,
    INT_BMI160 = 104,
    INT_BLE = 105,
    INT_RTC = 106,
    AON_TIMER = 107,
    INT_COMP = 108,
};


#include <Arduino.h>
#include <stdint.h>
#include <interrupt.h>
#include <board.h>
//#include "qm_sensor_regs.h"
//#include "ss_power_states.h"

class Power
{
    public:
        Power();
        
        //puts the SoC into "doze" mode which lowers the system clock speed to 32k
        void doze();
        
        void doze(int duration);
        
        void wakeFromDoze();
        
        void sleep();
        
        void sleep(int duration);
        
        void deepSleep();
        
        void deepSleep(int duration);
        
        void wakeFromSleepCallback(void);
        
        void attachInterruptWakeup(uint32_t pin, voidFuncPtr callback, uint32_t mode);

        void detachInterruptWakeup(uint32_t pin);
        
        uint32_t arc_restore_addr;  

    private:
        void turnOffUSB();
        
        void turnOnUSB();
        
        void switchToHybridOscillator();
        
        void switchToCrystalOscillator();
        
        void setRTCCMR(int milliseconds);
        
        uint32_t readRTC_CCVR();
        
        bool isSleeping = false;
        
        volatile bool dozing = false;
        
        uint32_t millisToRTCTicks(int milliseconds);
        
        void enableRTCInterrupt();
        
        void enableAONGPIOInterrupt(int aon_gpio, int mode);

        void x86_C2Request();
        
        void (*pmCB)();
};

extern Power PM;
