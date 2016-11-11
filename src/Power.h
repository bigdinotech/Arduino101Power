#define OSC0_CFG1       0xB0800008
#define CCU_SS_PERIPH_CLK_GATE_CTL  0xB0800028
#define CCU_SYS_CLK_CTL 0xB0800038
#define CCU_LP_CLK_CTL  0xB080002C
#define P_LVL2          0xB0800504
#define PM1C            0xB0800518
#define SLP_CFG         0xB0800550

#define USB_PLL_CFG0    0xB0800014
#define USB_PHY_CFG0    0xB0800800

#define RTC_CCVR        0xB0000400
#define RTC_CMR         0xB0000404
#define RTC_CCR         0xB000040C
#define RTC_EOI         0xB0000418
#define RTC_MASK_INT    0xB0800478

#define OSCTRIM_ADDR    0xffffe1f8

#define SLEEP_HOST_C0       0
#define SLEEP_HOST_C1       1
#define SLEEP_HOST_C2       2
#define SLEEP_HOST_C2_LP    3
#define SLEEP_SS_SS0        4
#define SLEEP_SS_SS1        5
#define SLEEP_SS_SS2        6
#define SLEEP_LPSS          7


#include <Arduino.h>
#include <stdint.h>
#include <interrupt.h>
#include <board.h>

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
        
        void switchToHybridOscillator();
        
        void switchToCrystalOscillator();
        
        void wakeFromSleepCallback(void);

        void attachWakeInterrupt(void (*userCallBack)());
        
        void detachWakeInterrupt(void) { return attachWakeInterrupt(NULL); };

    private:
        void turnOffUSB();
        
        void turnOnUSB();
        
        void setRTCCMR(int milliseconds);
        
        uint32_t readRTC_CCVR();
        
        bool isSleeping = false;
        
        uint32_t millisToRTCTicks(int milliseconds);
        
        void enableRTCInterrupt();
    
        void x86_C2Request();
        
        void (*pmCB)();
};

extern Power PM;
