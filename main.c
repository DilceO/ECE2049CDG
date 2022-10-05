/************** ECE2049 DEMO CODE ******************/
/**************  25 August 2021   ******************/
/***************************************************/

#include <msp430.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "peripherals.h"




#define CALADC12_15V_30C *((unsigned int *)0x1A1A) // For Vref+=2.5V
#define CALADC12_15V_85C *((unsigned int *)0x1A1C) // For Vref+=2.5V


long unsigned int timer_cnt = 4919167; // initialized with last 7 digits of WPI ID
int adc_inTemp;
unsigned int adc_scrollWheel = 0;

long unsigned int timerCopy = 4919167; // for bonus question 2
long unsigned int timer_cnt_cpy;
long unsigned int timer_cnt_cpy_prev;
long unsigned int counterHour = 1;

volatile float temperatureDegC_cpy;
volatile char secPassed = 0;

float tempC[256];
long unsigned int times[256];
char index = 0;

//Function Prototypess:
void displayTime(long unsigned int time_cnt_cpy);
void displayTemp(float tempC_cpy);
void configureButtons();
char buttonsPressed();
void buzzerOn(float potVal);
void buzzerOff();

void runtimerA2(void){

    TA2CTL = TASSEL_1 + MC_1 + ID_0;

    TA2CCR0 = 32767;

    TA2CCTL0 = CCIE;

}


#pragma vector = TIMER2_A0_VECTOR
__interrupt void Timer_A2_ISR(void) {

    timer_cnt++;
    secPassed = 1;

    if((counterHour%3600)==0){
        counterHour = 0;
    } else {
        counterHour++;
    }

}


#pragma vector = ADC12_VECTOR
__interrupt void ADC12ISR(void){

    adc_inTemp = ADC12MEM1 & 0x0FFF;
    adc_scrollWheel = ADC12MEM0 & 0x0FFF;

}

/**
 * main.c
 */
int main(void)
{


    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    configDisplay();
    configureButtons();


    volatile float temperatureDegC, temperatureDegF, degC_per_bit;
    volatile unsigned int bits30, bits85;
    float potVolts;
    char button = '0';

    bits30 = CALADC12_15V_30C;
    bits85 = CALADC12_15V_85C;

    float x =  ((float)(85.0 - 30.0)); // 85.0 - 30.0
    float y = ((float)(bits85 - bits30));

    degC_per_bit = x/y;

    REFCTL0 &= ~REFMSTR;


    // Initialize control register ADC12CTL0 = 0000 1001 0111 0000
    // SHT0x=9h (384 clk cycles), MCS=1=burst thru selected chans.,
    // REF2_5V = 0 (1.5V), REFON = 1 = use internal reference volts
    // and ADC12ON = 1 = turn ADC on
    ADC12CTL0 = ADC12SHT0_9|ADC12REFON|ADC12ON|ADC12MSC;

     // Initialize control register ADC12CTL1 = 0000 0010 0000 0010
     // ADC12CSTART ADDx = 0000 = start conversion with ADC12MEM0,
     // ADC12SHSx = 00 = use SW conversion trigger, ADC12SC bits
     // ADC12SHP = 1 = SAMPCON signal sourced from sampling timer,
     // ADC12ISSH = 0 = sample input signal not inverted,
     // ADC12DIVx = 000= divide ADC12CLK by 1,
     // ADC12SSEL=00= ADC clock ADC12OSC (~5 MHz),
     // ADC12CONSEQx = 01 = sequence of channels converted once
     // ADC12BUSY = 0 = no ADC operation active
      ADC12CTL1 = ADC12SHP + ADC12CONSEQ_1;

      ADC12MCTL0 = ADC12SREF_0 + ADC12INCH_0;
      ADC12MCTL1 = ADC12SREF_1 + ADC12INCH_10 + ADC12EOS; // Set to Vcc due to scroll wheel
      __delay_cycles(100); // delay to allow Ref to settle

      P6SEL = P6SEL | BIT0;


      ADC12IE = BIT1;
      _BIS_SR(GIE);

      runtimerA2();


      while (1) { //Enable and start single burst conversion
           ADC12CTL0 &= ~ADC12SC;
           ADC12CTL0 |= ADC12SC + ADC12ENC;


           temperatureDegC=((float)((((long) (adc_inTemp)) - bits30 ) * degC_per_bit + 30.0));


           // Temperature in Fahrenheit = (9/5)*Tc + 32
           temperatureDegF = temperatureDegC * 9.0/5.0 + 32.0;


           timer_cnt_cpy = timer_cnt;
           temperatureDegC_cpy = temperatureDegC;

           potVolts = (float)adc_scrollWheel * 3.3/4096; // Volts from scroll wheel

           button = buttonsPressed();

           if(button == 1){
               buzzerOn(potVolts);
           }

           if (button == 2){
              buzzerOff();
           }

           // BONUS QUESTION 2
           if(counterHour==0){

              timerCopy = timer_cnt;
              while ((timer_cnt < (timerCopy + 1))){
                  buzzerOn(potVolts);
              }

              buzzerOff();
           }


           if (secPassed) {

               if (((timer_cnt_cpy%4)==0)  && (timer_cnt_cpy_prev!=timer_cnt_cpy) ) {

                   timer_cnt_cpy_prev = timer_cnt_cpy;
                   Graphics_clearDisplay(&g_sContext); // Clear the display

                   // USE 2 DISPLAY FUNCTIONS FROM PRELAB
                   displayTime(timer_cnt_cpy);
                   displayTemp(temperatureDegC_cpy);




                   tempC[index] = temperatureDegC_cpy; // store value on tempC[] array
                   times[index] = timer_cnt_cpy; // store value on times[] array
                   index++; // index is of type char so it has a range [0,255]




                   secPassed = 0;

               }

           }

       }

    return 0;
}





void displayTime(long unsigned int time_cnt_cpy){


    long unsigned int days = (time_cnt_cpy / (long)86400);

    long unsigned int hours = (time_cnt_cpy - days * (long)86400) / (long)3600;

    time_cnt_cpy = time_cnt_cpy - days * ((long)86400);

    long unsigned int minutes = (time_cnt_cpy - ((long)3600) * hours) / ((long)60);

    time_cnt_cpy = time_cnt_cpy - ((long) 3600) * hours;

    long unsigned seconds = time_cnt_cpy - ((long)60) * minutes;


    long unsigned int day = days % 365;

    counterHour = 3600 - (minutes * 60) - seconds;

    char output[7];
    char out[9];
    int dd;

    if(day <= 30){

        output[0] = 'J';
        output[1] = 'A';
        output[2] = 'N';
        output[3] = '-';
        dd = day + 1;
        output[4] = (char)(dd/10) + 0x30;
        output[5] = (char)(dd) % 10 + 0x30;



    }else if((day >= 31) && (day <= 58)){
        output[0] = 'F';
        output[1] = 'E';
        output[2] = 'B';
        output[3] = '-';
        dd = (day - 30);
        output[4] = (char)((dd/10) + 0x30);
        output[5] = (char)((dd % 10) + 0x30);




    }else if((day >=59) && (day <=89)){
        output[0] ='M';
       output[1] = 'A';
       output[2] = 'R';
       output[3] = '-';
       dd = (day - 58);
       output[4] = (char)((dd/10) + 0x30);
       output[5] = (char)((dd) % 10 + 0x30);


    }else if((day >= 90) && (day <= 119)){
        output[0] = 'A';
       output[1] = 'P';
       output[2] = 'R';
       output[3] = '-';
       dd = (day - 89);
       output[4] = (char)((dd/10) + 0x30);
       output[5] = (char)((dd) % 10 + 0x30);


    }else if((day >= 120) && (day <= 150)){
        output[0] = 'M';
       output[1] = 'A';
       output[2] = 'Y';
       output[3] = '-';
       dd = (day - 119);
       output[4] = (char)((dd/10) + 0x30);
       output[5] = (char)((dd) % 10 + 0x30);


    }else if((day >= 151) && (day <= 180)){
        output[0] = 'J';
       output[1] = 'U';
       output[2] = 'N';
       output[3] = '-';
       dd = (day - 150);
       output[4] = (char)((dd/10) + 0x30);
       output[5] = (char)((dd) % 10 + 0x30);


    }else if((day >= 181) && (day <= 211)){
        output[0] = 'J';
       output[1] = 'U';
       output[2] = 'L';
       output[3] = '-';
       dd = (day - 180);
       output[4] = (char)((dd/10) + 0x30);
       output[5] = (char)((dd) % 10 + 0x30);


    }else if((day >= 212) && (day <= 242)){
        output[0] = 'A';
       output[1] = 'U';
       output[2] = 'G';
       output[3] = '-';
       dd = (day - 211);
       output[4] = (char)((dd/10) + 0x30);
       output[5] = (char)((dd) % 10 + 0x30);

    }else if((day >= 243) && (day <= 272)){
        output[0] = 'S';
       output[1] = 'E';
       output[2] = 'P';
       output[3] = '-';
       dd = (day - 242);
       output[4] = (char)((dd/10) + 0x30);
       output[5] = (char)((dd) % 10 + 0x30);


    }else if((day >= 273) && (day <= 303)){
        output[0] = 'O';
       output[1] = 'C';
       output[2] = 'T';
       output[3] = '-';
       dd = (day - 272);
       output[4] = (char)((dd/10) + 0x30);
       output[5] = (char)((dd) % 10 + 0x30);


    }else if((day >= 304) && (day <= 333)){
        output[0] = 'N';
          output[1] = 'O';
          output[2] = 'V';
          output[3] = '-';
          dd = (day - 303);
          output[4] = (char)((dd/10) + 0x30);
          output[5] = (char)((dd) % 10 + 0x30);


    }else if((day >= 334) && (day <= 364)){
       output[0] = 'D';
       output[1] = 'E';
       output[2] = 'C';
       output[3] = '-';
       dd = (day - 333);
       output[4] = (char)((dd/10) + 0x30);
       output[5] = (char)((dd) % 10 + 0x30);


      }

        output[6] = '\0';

        out[0] = (char)(hours/10) + 0x30;
        out[1] = (char)(hours) % 10 + 0x30;
        out[2] = ':';
        out[3] = (char)(minutes/10) + 0x30;
        out[4] = (char)(minutes) % 10 + 0x30;
        out[5] = ':';
        out[6] = (char)(seconds/10) + 0x30;
        out[7] = (char)(seconds) % 10 + 0x30;
        out[8] = '\0';


        Graphics_drawStringCentered(&g_sContext, output, AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
        Graphics_drawStringCentered(&g_sContext, out, AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
}


void displayTemp(float tempC_cpy) {

    char out[14];

    float tempF = tempC_cpy * (9/5) + 32;
    char decC, decF;

    out[0] = (char)( ((int) floor(tempC_cpy/100))  + 48);
    if (out[0] == '0') {
        out[0] = ' ';
    }
    out[1] = (char)(( (int)  floor(tempC_cpy/10)       ) + 48);
    out[2] = (char)(( (int) fmodf(tempC_cpy, 10.0)  ) + 48);

    out[3] = '.';
    decC = (int) (fmodf(tempC_cpy, 1.0) * 10);
    out[4] = ((char) (decC)) + 0x30;
    out[5] = 'C';
    out[6] = '/';

    out[7] = (char)((tempF/100) + 0x30);
    if (out[7] == '0') {
           out[7] = ' ';
       }
    out[8] = (char)((tempF/10) + 0x30);
    out[9] = (char)(fmodf(tempF, 10.0) + 0x30);
    out[10] = '.';
    decF = (char) (fmodf(tempF, 1.0) * 10);
    out[11] =  decF + 0x30;
    out[12] = 'F';
    out[13] = '\0';

   Graphics_drawStringCentered(&g_sContext, out, AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);
   Graphics_flushBuffer(&g_sContext);


}



void configureButtons()
{
    P7SEL &= ~(BIT0);
    P3SEL &= ~(BIT6);
    P2SEL &= ~(BIT2);
    P7SEL &= ~(BIT4);

    P7DIR &= ~(BIT0);
    P3DIR &= ~(BIT6);
    P2DIR &= ~(BIT2);
    P7DIR &= ~(BIT4);

    P7REN |= (BIT0);
    P3REN |= (BIT6);
    P2REN |= (BIT2);
    P7REN |= (BIT4);

    P7OUT |= (BIT0);
    P3OUT |= (BIT6);
    P2OUT |= (BIT2);
    P7OUT |= (BIT4);

}


char buttonsPressed(){
    char holderS1 = 0x00;
    char holderS2 = 0x00;
    char holderS3 = 0x00;
    char holderS4 = 0x00;
    char outBits = 0x00;

    holderS1 |= (P7IN & BIT0);

    holderS2 |= (P3IN & BIT6);
    holderS2 = holderS2 >> 5;

    holderS3 |= (P2IN & BIT2);

    holderS4 |= (P7IN & BIT4);
    holderS4 = holderS4 >> 1;


    outBits = holderS1 | holderS2 | holderS3 | holderS4;


    switch (outBits) {
       case 0: {
           return 15;
           break;
       }
       case 1: {
           return 14;
           break;
       }
       case 2: {
          return 13;
          break;
       }
       case 3: {
           return 12;
         break;
       }

       case 4:{
          return 11;
          break;
       }
       case 5:{
          return 10;
          break;
       }
       case 6:{
          return 9;
          break;
       }
       case 7:{
          return 8;
          break;
       }
       case 8:{
          return 7;
          break;
       }
       case 9:{
          return 6;
          break;
       }
       case 10:{
          return 5;
          break;
       }
       case 11:{
          return 4;
          break;
       }
       case 12:{
          return 3;
          break;
       }
       case 13:{
          return 2;
          break;
       }
       case 14:{
          return 1;
          break;
       }
       case 15:{
          return 0;
          break;
       }

    }

    return outBits;
}

/*
 * Enable a PWM-controlled buzzer on P3.5
 * This function makes use of TimerB0.
 */
void buzzerOn(float potVal)
{
    unsigned int ACLK = 32768;

    // Initialize PWM output on P3.5, which corresponds to TB0.5
    P3SEL |= BIT5; // Select peripheral output mode for P3.5
    P3DIR |= BIT5;

    TB0CTL  = (TBSSEL__ACLK|ID__1|MC__UP);  // Configure Timer B0 to use ACLK, divide by 1, up mode
    TB0CTL  &= ~TBIE;                       // Explicitly Disable timer interrupts for safety


    int inputVal = (int) (((int) floor(255/3.3)) * potVal);
    TB0CCR0   = inputVal;                    // Set the PWM period in ACLK ticks
    TB0CCTL0 &= ~CCIE;                  // Disable timer interrupts

    // Configure CC register 5, which is connected to our PWM pin TB0.5
    TB0CCTL5  = OUTMOD_7;                   // Set/reset mode for PWM
    TB0CCTL5 &= ~CCIE;                      // Disable capture/compare interrupts
    TB0CCR5   = TB0CCR0/2;                  // Configure a 50% duty cycle
}


/*
 * Disable the buzzer on P7.5
 */
void buzzerOff()
{
    // Disable both capture/compare periods
    TB0CCTL0 = 0;
    TB0CCTL5 = 0;
}

unsigned char getButtonState()
{
    unsigned char ret = 0x00;
    // P2.1
    if (~P2IN & BIT1)
        ret |= BIT0; // Left button (S1)
    // P1.1
    if (~P1IN & BIT1)
        ret |= BIT1; // Right button (S2)
    return ret;
}
