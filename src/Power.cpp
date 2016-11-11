#include "Power.h"

Power PM;

static void PM_InterruptHandler(void)
{
    *(uint32_t*)RTC_CCR |= 0xFFFFFFFE;
    uint32_t rtc_eoi = *(uint32_t*)RTC_EOI; //clear match interrupt
    digitalWrite(12, HIGH);
    digitalWrite(12, LOW);
    Serial1.println("wake");
    PM.wakeFromSleepCallback();
}

Power::Power()
{
    
}

void Power::doze()
{
  turnOffUSB();
  
  //switch from external crystal oscillator to internal hybrid oscilator
  switchToHybridOscillator();
  
  //Set system clock to the RTC Crystal Oscillator
  uint32_t current_val = *(uint32_t*)CCU_SYS_CLK_CTL;
  *(uint32_t*)CCU_SYS_CLK_CTL = current_val & 0xFFFFFFFE;

  //Powerdown hybrid oscillator
  current_val = *(uint32_t*)OSC0_CFG1;
  *(uint32_t*)OSC0_CFG1 = current_val | 0x00000004; 
}

void Power::doze(int duration)
{
    doze();
    delayTicks(millisToRTCTicks(duration));
    wakeFromDoze();
}

void Power::wakeFromDoze()
{
  //Powerup hybrid oscillator
  uint32_t current_val = *(uint32_t*)OSC0_CFG1;
  *(uint32_t*)OSC0_CFG1 = current_val & 0xFFFFFFFB;
   
  //Set system clock to the Hybrid Oscillator
  current_val = *(uint32_t*)CCU_SYS_CLK_CTL;
  *(uint32_t*)CCU_SYS_CLK_CTL = current_val | 0x00000001;

  //switch back to the external crystal oiscillator
  void switchToCrystalOscillator();
  
  turnOnUSB();
}

void Power::sleep()
{
   turnOffUSB();
   //*(uint32_t*)SLP_CFG &= 0xFFFFFEFF;
   
   x86_C2Request();
   isSleeping = true;
   //*(uint32_t*)CCU_LP_CLK_CTL = (*(uint32_t*)CCU_LP_CLK_CTL) | 0x00000002;
   //uint32_t c2 = *(uint32_t*)P_LVL2;
   *(uint32_t*)PM1C |= 0x00002000;
}

void Power::sleep(int duration)
{
    setRTCCMR(duration);
    enableRTCInterrupt();
    sleep();
}

void Power::deepSleep()
{
   turnOffUSB();
   
   x86_C2Request();
   isSleeping = true;
   *(uint32_t*)CCU_LP_CLK_CTL  |= 0x00000001;
   //uint32_t c2 = *(uint32_t*)P_LVL2;
   *(uint32_t*)PM1C |= 0x00002000;
}

void Power::deepSleep(int duration)
{
    setRTCCMR(duration);
    enableRTCInterrupt();
    deepSleep();
}

void Power::switchToHybridOscillator()
{
    //read trim value from OTP
    uint32_t trimMask = *(uint16_t*)OSCTRIM_ADDR << 20;
    *(uint32_t*)OSC0_CFG1 = 0x00000002 | trimMask;  //switch to internal oscillator using trim value from OTP
}

void Power::switchToCrystalOscillator()
{
    *(uint32_t*)OSC0_CFG1 = 0x00070009;
}

inline void Power::wakeFromSleepCallback(void)
{
    if(pmCB != NULL)
        pmCB();
}

void Power::attachWakeInterrupt(void (*userCallBack)())
{
    pmCB = userCallBack;
}

//Privates

void Power::turnOffUSB()
{
    *(uint32_t*)USB_PHY_CFG0 |= 0x00000001; 
}

void Power::turnOnUSB()
{
    *(uint32_t*)USB_PHY_CFG0 &= 0xFFFFFFFE;
}

void Power::setRTCCMR(int milliseconds)
{
    *(uint32_t*)RTC_CMR = readRTC_CCVR() + millisToRTCTicks(milliseconds);
    //*(uint32_t*)RTC_CMR = readRTC_CCVR() + milliseconds;
}

uint32_t Power::readRTC_CCVR()
{
    return *(uint32_t*)RTC_CCVR;
}

uint32_t Power::millisToRTCTicks(int milliseconds)
{
    return (uint32_t)((double)milliseconds*32.768);
}

void Power::enableRTCInterrupt()
{
    *(uint32_t*)RTC_MASK_INT &= 0xFFFFFFFF;
    *(uint32_t*)RTC_CCR |= 0x00000001;
    *(uint32_t*)RTC_CCR &= 0xFFFFFFFD;
    interrupt_disable(IRQ_RTC_INTR);
    interrupt_connect(IRQ_RTC_INTR , &PM_InterruptHandler);
    interrupt_enable(IRQ_RTC_INTR);
}

void Power::x86_C2Request()
{
    switchToHybridOscillator();
    //set the CCU_C2_LP_EN bit
    *(uint32_t*)CCU_LP_CLK_CTL = (*(uint32_t*)CCU_LP_CLK_CTL) | 0x00000002;
    //request for the x86 core go into C2 sleep
    volatile uint32_t c2 = *(volatile uint32_t*)P_LVL2;
 
}
