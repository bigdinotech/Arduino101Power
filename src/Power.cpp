#include "Power.h"

Power PM;
uint32_t arc_restore_addr;
uint32_t cpu_context[33];

static void PM_InterruptHandler(void)
{
    unsigned int flags = interrupt_lock();
    PM.wakeFromDoze();
    PM.wakeFromSleepCallback();
    interrupt_unlock(flags);
}

Power::Power()
{
    
}

void Power::doze()
{
  turnOffUSB();
  dozing = true;
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

  //switch back to the external crystal oscillator
  void switchToCrystalOscillator();
  
  turnOnUSB();
  
  dozing = false;
}

void Power::sleep()
{
   uint32_t creg_mst0_ctrl = 0;
   creg_mst0_ctrl = __builtin_arc_lr(QM_SS_CREG_BASE);

   /*
	 * Clock gate the sensor peripherals at CREG level.
	 * This clock gating is independent of the peripheral-specific clock
	 * gating provided in ss_clk.h .
   */
   creg_mst0_ctrl |= (QM_SS_IO_CREG_MST0_CTRL_ADC_CLK_GATE |
    QM_SS_IO_CREG_MST0_CTRL_I2C1_CLK_GATE |
	QM_SS_IO_CREG_MST0_CTRL_I2C0_CLK_GATE |
	QM_SS_IO_CREG_MST0_CTRL_SPI1_CLK_GATE |
	QM_SS_IO_CREG_MST0_CTRL_SPI0_CLK_GATE);
    
    __builtin_arc_sr(creg_mst0_ctrl, QM_SS_CREG_BASE);
    
   //#if 0
   x86_C2Request();
   doze();
   
      __asm__ __volatile__(
		    "sleep %0"
		    :
		    : "i"(QM_SS_SLEEP_MODE_CORE_OFF));
   
   /**
   while(dozing)
   {
    digitalWrite(11, HIGH);
    digitalWrite(11, LOW);
   }
   **/
   
   //#endif 
   
   #if 0
   //FIXME:  Actually go to sleep instead of just dozing
   
   Serial1.println(" start of sleep");
   if((QM_SCSS_GP->gps0)&0x00000004)
   {
       Serial1.println("waking up");
       //goto wake_from_sleep;
   }
   turnOffUSB();
   //*(uint32_t*)SLP_CFG &= 0xFFFFFEFF;
   
   Serial1.print("arc restore addr0: ");
   Serial1.println(arc_restore_addr);
   
   Serial1.print("arc restore addr1: ");
   Serial1.println((uint32_t)&arc_restore_addr, HEX);
   Serial1.println((uint32_t)&&wake_from_sleep, HEX);
   
   qm_ss_set_resume_vector(sleep_restore_trap, arc_restore_addr);
   shared_data->arc_restore_addr = (&&wake_from_sleep);
   
   Serial1.print("arc restore addr1b: ");
   Serial1.println(arc_restore_addr, HEX);
   Serial1.println((uint32_t)&&wake_from_sleep, HEX);
   /**
   for(int i = 0 ; i < 32; i++)
   {
       cpu_context[i] = 0;
       Serial1.print("reg");
       Serial1.print(i);
       Serial1.print(": ");
       Serial1.println(cpu_context[i], HEX); 
   }
   **/
   qm_ss_save_context(cpu_context);
   
   for(int i = 0 ; i < 32; i++)
   {
       Serial1.print("reg");
       Serial1.print(i);
       Serial1.print(": ");
       Serial1.println(cpu_context[i], HEX);
       shared_data->arc_cpu_context[i] = cpu_context[i];      
   }
   
   //Serial1.print("arc restore addr2: ");
   //Serial1.println(arc_restore_addr);

   shared_data->pm_status |= 0x00000001;
   QM_SCSS_GP->gps0 |= BIT(QM_GPS0_BIT_SENSOR_WAKEUP);
   
   for(int i = 0 ; i < 32; i++)
   {
       cpu_context[i] = shared_data->arc_cpu_context[i];
       Serial1.print("reg");
       Serial1.print(i);
       Serial1.print(": ");
       Serial1.println(cpu_context[i], HEX);
   }

   Serial1.println("really sleeping");
   
   x86_C2Request();
   isSleeping = true;
   
   /**
   __asm__ __volatile__(
		    "sleep %0"
		    :
		    : "i"(QM_SS_SLEEP_MODE_CORE_OFF_TIMER_OFF));
   **/
   
   *(uint32_t*)CCU_LP_CLK_CTL = (*(uint32_t*)CCU_LP_CLK_CTL) | 0x00000002;
   uint32_t c2 = *(uint32_t*)P_LVL2;
   Serial1.println("PM1C");
   *(uint32_t*)PM1C |= 0x00002000;
   
   wake_from_sleep:
   Serial1.println("restoring context");
   for(int i = 0 ; i < 32; i++)
   {
       cpu_context[i] = shared_data->arc_cpu_context[i];
       Serial1.print("reg");
       Serial1.print(i);
       Serial1.print(": ");
       Serial1.println(cpu_context[i], HEX);
   }
   qm_ss_restore_context(sleep_restore_trap, cpu_context);
   Serial1.println("context restored");
   
   digitalWrite(12, HIGH);
   digitalWrite(12, LOW);
   digitalWrite(12, HIGH);
   digitalWrite(12, LOW);
   
   #endif
   
   creg_mst0_ctrl &= ~(QM_SS_IO_CREG_MST0_CTRL_ADC_CLK_GATE |
			    QM_SS_IO_CREG_MST0_CTRL_I2C1_CLK_GATE |
			    QM_SS_IO_CREG_MST0_CTRL_I2C0_CLK_GATE |
			    QM_SS_IO_CREG_MST0_CTRL_SPI1_CLK_GATE |
			    QM_SS_IO_CREG_MST0_CTRL_SPI0_CLK_GATE);
                
   __builtin_arc_sr(creg_mst0_ctrl, QM_SS_CREG_BASE);
}

void Power::sleep(int duration)
{
    //setRTCCMR(duration);
    //enableRTCInterrupt();
    enableAONPTimerInterrrupt(duration);
    sleep();
}

void Power::deepSleep()
{
   sleep();
}

void Power::deepSleep(int duration)
{
    sleep(duration);
}

inline void Power::wakeFromSleepCallback(void)
{
    if(pmCB != NULL)
        pmCB();
}

void Power::attachInterruptWakeup(uint32_t pin, voidFuncPtr callback, uint32_t mode)
{
    if( pin >= NUM_DIGITAL_PINS  )
    {
        pmCB = callback;
        switch (pin)
        {
            case AON_GPIO0:
                enableAONGPIOInterrupt(0, mode);
                break;
            case AON_GPIO1:
                enableAONGPIOInterrupt(1, mode);
                break;
            case AON_GPIO2:
                enableAONGPIOInterrupt(2, mode);
                break;
            case AON_GPIO3:
                enableAONGPIOInterrupt(3, mode);
                break;
            case INT_BMI160:
                enableAONGPIOInterrupt(4, mode);
                break;
            case INT_BLE:
                enableAONGPIOInterrupt(5, mode);
                break;
            default:
                break;
        };
    }
    else
    {
        //regular gpio interrupt
        attachInterrupt(pin, callback, mode);
    }
}

void Power::detachInterruptWakeup(uint32_t pin)
{
    pmCB = NULL;
    if( pin >= NUM_DIGITAL_PINS  )
    {
        if(pin == INT_RTC)
        {
            interrupt_disable(IRQ_RTC_INTR);
        }
        else if (pin == INT_COMP)
        {
            interrupt_disable(IRQ_ALWAYS_ON_TMR);
        }
        else if (pin == AON_TIMER)
        {
            interrupt_disable(IRQ_COMPARATORS_INTR);
        }
        else
        {
           interrupt_disable(IRQ_ALWAYS_ON_GPIO);
        }
    }
    else
    {
        detachInterrupt(pin);
    }
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

void Power::switchToHybridOscillator()
{
    //read trim value from OTP
    uint32_t trimMask = *(uint16_t*)OSCTRIM_ADDR << 20;
    *(uint32_t*)OSC0_CFG1 = 0x00000002 | trimMask;  //switch to internal oscillator using trim value from OTP
}

void Power::switchToCrystalOscillator()
{
    *(uint32_t*)OSC0_CFG1 = 0x00070009;
    while(!(*(uint32_t*)OSC0_STAT & 0x00000002));   //wait till crystal oscillator is stable
}

void Power::setRTCCMR(int milliseconds)
{
    *(uint32_t*)RTC_CMR = readRTC_CCVR() + millisToRTCTicks(milliseconds);
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
    *(uint32_t*)RTC_MASK_INT &= 0xFFFFFEFE;
    *(uint32_t*)RTC_CCR |= 0x00000001;
    *(uint32_t*)RTC_CCR &= 0xFFFFFFFD;
    volatile uint32_t read = *(uint32_t*)RTC_EOI;
    
    pmCB = &wakeFromRTC;
    interrupt_disable(IRQ_RTC_INTR);
    interrupt_connect(IRQ_RTC_INTR , &PM_InterruptHandler);
    delayTicks(6400);   //2ms
    interrupt_enable(IRQ_RTC_INTR);
}

void Power::enableAONGPIOInterrupt(int aon_gpio, int mode)
{
    switch(mode)
    {
        case CHANGE:    //not supported just do the same as FALLING
            *(uint32_t*)AON_GPIO_INTTYPE_LEVEL |= 1 << aon_gpio;
            *(uint32_t*)AON_GPIO_INT_POL &= ~(1 << aon_gpio);
            break;
        case RISING:
            *(uint32_t*)AON_GPIO_INTTYPE_LEVEL |= 1 << aon_gpio;
            *(uint32_t*)AON_GPIO_INT_POL |= 1 << aon_gpio;
            break;
        case FALLING:
            *(uint32_t*)AON_GPIO_INTTYPE_LEVEL |= 1 << aon_gpio;
            *(uint32_t*)AON_GPIO_INT_POL &= ~(1 << aon_gpio);
            break;
        case HIGH:
            *(uint32_t*)AON_GPIO_INTTYPE_LEVEL &= ~(1 << aon_gpio);
            *(uint32_t*)AON_GPIO_INT_POL |= 1 << aon_gpio;
            break;
        case LOW:
            *(uint32_t*)AON_GPIO_INTTYPE_LEVEL &= ~(1 << aon_gpio);
            *(uint32_t*)AON_GPIO_INT_POL &= ~(1 << aon_gpio);
            break;
        default:
            *(uint32_t*)AON_GPIO_INTTYPE_LEVEL &= ~(1 << aon_gpio);
            *(uint32_t*)AON_GPIO_INT_POL &= ~(1 << aon_gpio);
            break;
    };
    
    *(uint32_t*)AON_GPIO_SWPORTA_DDR &= ~(1 << aon_gpio);
    *(uint32_t*)AON_GPIO_INTMASK &= ~(1 << aon_gpio);
    *(uint32_t*)AON_GPIO_INTEN |= 1 << aon_gpio;
    
    
    *(uint32_t*)AON_GPIO_MASK_INT &= 0xFFFFFEFE;
    interrupt_disable(IRQ_ALWAYS_ON_GPIO);
    interrupt_connect(IRQ_ALWAYS_ON_GPIO , &PM_InterruptHandler);
    interrupt_enable(IRQ_ALWAYS_ON_GPIO);
}

void Power::enableAONPTimerInterrrupt(int millis)
{
    pmCB = resetAONPTimer;
    *(uint32_t*)AONPT_CFG = millisToRTCTicks(millis);
    *(uint32_t*)AONPT_CTRL |= 0x00000003;
    
    *(uint32_t*)AON_TIMER_MASK_INT &= 0xFFFFFEFE;
    interrupt_disable(IRQ_ALWAYS_ON_TMR);
    interrupt_connect(IRQ_ALWAYS_ON_TMR , &PM_InterruptHandler);
    //delayTicks(6400);   //2ms
    interrupt_enable(IRQ_ALWAYS_ON_TMR);
}

void Power::resetAONPTimer()
{
    *(uint32_t*)AONPT_CFG = 0;
    *(uint32_t*)AONPT_CTRL |= 0x00000001;
    delayTicks(6400);   //2ms
    //Serial1.println("resetting timer");
}

void Power::wakeFromRTC()
{
    *(uint32_t*)RTC_MASK_INT |= 0x00000101;
    interrupt_disable(IRQ_RTC_INTR);
    volatile uint32_t read = *(uint32_t*)RTC_EOI;
}

void Power::x86_C2Request()
{
    switchToHybridOscillator();
    //set the CCU_C2_LP_EN bit
    *(uint32_t*)CCU_LP_CLK_CTL = (*(uint32_t*)CCU_LP_CLK_CTL) | 0x00000002;
    //request for the x86 core go into C2 sleep
    volatile uint32_t c2 = *(volatile uint32_t*)P_LVL2;
}
