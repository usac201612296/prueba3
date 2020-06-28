#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/pwm.h"


int nota_v[7] = {4520,4520,4520,4520,4520,4520,4520};
int señal1[8]={4520,19500,4520,19500,4520,19500,4520,19500};
int time_v[1] = {19400};
int soundRed[2]= {200, 1000};
int melodiaerror[ ] = {262, 196, 196, 220, 196, 0, 247, 262};
int duracionNotas[] = {4, 8, 8, 4, 4, 4, 4, 4};

int nota=1;
int status,i,j;
int A=10;
int Push;
float time;

void GPIO(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY; // Desbloquear PF0
    GPIO_PORTF_CR_R = 0x0f;
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

  }

void interrupcion(void){
    GPIOIntEnable(INT_GPIOF, GPIO_INT_PIN_4|GPIO_INT_PIN_0);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_4|GPIO_INT_PIN_0);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_INT_PIN_4|GPIO_INT_PIN_0, GPIO_BOTH_EDGES);
    IntEnable(INT_GPIOF);
    IntMasterEnable();

}

void PWM(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
    GPIOPinTypePWM(GPIO_PORTE_BASE, GPIO_PIN_5);
    GPIOPinConfigure(GPIO_PE5_M0PWM5);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
    PWMGenEnable(PWM0_BASE, PWM_GEN_2);
    PWMOutputState(PWM0_BASE, PWM_OUT_5_BIT, true);
}

void melodia (void){
         for (i=0; i<=8; i++){
             PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, melodiaerror[i]);
             PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5,(melodiaerror[i])/2);
             SysCtlDelay(SysCtlClockGet()/16);
         }
}

void melodiaRojo(void){
    for (i=0; i<=2; i++){
        PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, soundRed[i]);
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5,(soundRed[i])/2);
        SysCtlDelay(SysCtlClockGet()/16);
    }
}


void pin1_interruption (void){
    GPIOIntClear(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4);
    Push = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);

        switch(Push){
            case 1:
                melodia();
                  break;
            case 16:
                melodiaRojo();
            default:
                GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0);
                break;
             }
        SysCtlDelay(20000000/3);
        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0);
}

int main(void){

    SysCtlClockSet(SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_5|SYSCTL_USE_PLL);
    //time=SysCtlClockGet();
    PWM();
    GPIO();
    interrupcion();

    while(1);

}
