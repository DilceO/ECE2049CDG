/************** ECE2049 DEMO CODE ******************/
/**************  25 August 2021   ******************/
/***************************************************/

#include <msp430.h>

/* Peripherals.c and .h are where the functions that implement
 * the LEDs and keypad, etc are. It is often useful to organize
 * your code by putting like functions together in files.
 * You include the header associated with that file(s)
 * into the main file of your project. */
#include "peripherals.h"

#include <msp430.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "peripherals.h"


#define CALADC12_15V_30C *((unsigned int *)0x1A1A) // For Vref+=2.5V
#define CALADC12_15V_85C *((unsigned int *)0x1A1C) // For Vref+=2.5V
unsigned int dac_Codes[17] = {0, 256, 512, 768, 1024, 1280, 1536, 1792, 2048, 2304, 2560, 2816, 3072, 3328, 3584, 3840, 4095}; // adc_scrollWheel
int i;
unsigned int waitTime = 0;
unsigned int tmp;
long unsigned int timer_cnt = 0;
long unsigned leap_cnt = 0;
int adc_inTemp;
unsigned int adc_scrollWheel = 0;
unsigned int codeValue = 0;
long unsigned int last_cnt = 0;
long unsigned int indexSquare = 0;
long unsigned int volts_code;
long unsigned int val = 0;
unsigned int  code = 0, step = 81;
int state=0;
char flag = 0;
long unsigned int oneInterrupt;
long double floatVoltage = 0.0;

//Function Prototypess:
void displayTime(long unsigned int time_cnt_cpy);
void displayTemp(float tempC_cpy);
void configureButtons();
char buttonsPressed();
void buzzerOn(float potVal);
void buzzerOff();
void DACInit(void);
void DACSetValue(unsigned int dac_code);
unsigned char getKeys(void);
long double ADCconfigP61andSC(void);
void runtimerA2(void){
    TA2CTL = TASSEL_1 + MC_1 + ID_0;
    TA2CCR0 = 2; // 15 for 0.0005
    TA2CCTL0 = CCIE;
}

#pragma vector = TIMER2_A0_VECTOR
__interrupt void Timer_A2_ISR(void) {

    if(leap_cnt < 12){ // 43 for 0.0005
        timer_cnt++;
        leap_cnt++;
    } else {
        leap_cnt = 0;
    }
}

#pragma vector = ADC12_VECTOR
__interrupt void ADC12ISR(void){
     codeValue = ADC12MEM1 & 0x0FFF;
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
    configKeypad();

    volatile float temperatureDegC, temperatureDegF, degC_per_bit;
    volatile unsigned int bits30, bits85;
    float potVolts;
    char button = '0';
    unsigned char currKey=0;
    bits30 = CALADC12_15V_30C;
    bits85 = CALADC12_15V_85C;
    float x =  ((float)(85.0 - 30.0)); // 85.0 - 30.0
    float y = ((float)(bits85 - bits30));
    degC_per_bit = x/y;
    DACInit(); // Here we are Initializing the DAC
    ADCconfigP61andSC();
    runtimerA2();

      while (1) { //Enable and start single burst conversion
          ADC12CTL0 &= ~ADC12SC;
          ADC12CTL0 |= ADC12SC + ADC12ENC;
          switch (state) {
                     case 0:
                     {
                         state=0;

                         Graphics_drawStringCentered(&g_sContext, "Press:", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT); // Display welcome screen
                         Graphics_drawStringCentered(&g_sContext, "1 = DC", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT); // Display welcome screen
                         Graphics_drawStringCentered(&g_sContext, "2 = Square Wave", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT); // Display welcome screen
                         Graphics_drawStringCentered(&g_sContext, "3=Sawtooth Wave", AUTO_STRING_LENGTH, 48, 45, TRANSPARENT_TEXT); // Display welcome screen
                         Graphics_drawStringCentered(&g_sContext, "4=Triangle Wave", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT); // Display welcome screen
                         Graphics_flushBuffer(&g_sContext);
                         currKey = buttonsPressed();

                         if (currKey == 1) {
                             Graphics_clearDisplay(&g_sContext); // Clear the display
                             Graphics_drawStringCentered(&g_sContext, "DC", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
                             Graphics_flushBuffer(&g_sContext);
//                             lightUserLED(1);
                             state = 1;
                         }
                         if(currKey == 2){
                             Graphics_clearDisplay(&g_sContext); // Clear the display
                             Graphics_drawStringCentered(&g_sContext, "SQUARE", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
                             Graphics_flushBuffer(&g_sContext);
//                             lightUserLED(1);
                             state = 2;
                         }
                         if(currKey == 4){
                             Graphics_clearDisplay(&g_sContext); // Clear the display
                             Graphics_drawStringCentered(&g_sContext, "SAWTOOTH", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
                             Graphics_flushBuffer(&g_sContext);
//                             lightUserLED(1);
                             state = 3;
                         }
                         if(currKey == 8){
                             Graphics_clearDisplay(&g_sContext); // Clear the display
                             Graphics_drawStringCentered(&g_sContext, "TRIANGLE", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
                             Graphics_flushBuffer(&g_sContext);
//                             lightUserLED(1);
                             state = 4;
                         }

                         // timer_on = timer_cnt;
                         break;
                     }
                     case 1:
                     {
                         //ADCconfigP61andSC();
                         DACSetValue(adc_scrollWheel); // We only need to call this once and it will constantly send of the value we want
                         ADCconfigP61andSC();
                         break;
                     }
                     case 2:
                     {
                         unsigned int dacCodes[2] = {0, adc_scrollWheel}; // adc_scrollWheel

                         last_cnt = timer_cnt;
//                         while ((timer_cnt % (last_cnt + 13)) != 0) {
//
//                             tmp = dacCodes[indexSquare%2];
//
//                             DACSetValue(dacCodes[indexSquare%2]);
//
//                         }
                         while (timer_cnt < last_cnt + 59){// 13 for 0.0005
                             __no_operation();
                         }
                         DACSetValue(dacCodes[indexSquare%2]);
                         indexSquare++;
//                           while(waitTime == 24){
//                                tmp = dacCodes[indexSquare%2];
//                                DACSetValue(dacCodes[indexSquare%2]);
//
//                                if (indexSquare == 65535) {
//                                   indexSquare = 0;
//                               } else {
//
//                                   indexSquare++;
//
//                               }
//                           }

                           last_cnt = timer_cnt;

                         break;
                     }
                     case 3:
                     {

                             volts_code = ((timer_cnt % 133)*30);
                             DACSetValue(volts_code);
//                           volts_code = ((val % 133)*30); //133, 30
//
//
//                         last_cnt = timer_cnt;
//
//                         while (timer_cnt == last_cnt){// 13 for 0.0005
//                              __no_operation();
//                          }
//
//
//                         DACSetValue(volts_code);
//                         val++;
//                         last_cnt = timer_cnt;
                         break;
                     }
                     case 4:
                     {
//                         volts_code = ((timer_cnt % 50)*81);
//                         DACSetValue(volts_code);
//                         volts_code = (4095 - ((timer_cnt % 50)*81));
//                         DACSetValue(volts_code);

//                         timer_cnt = 0;
//                         runtimerA2();
                         for (i=0; i<=16; i++) { // 50
                             DACSetValue(dac_Codes[i]);
                             oneInterrupt = timer_cnt;
                             while (timer_cnt == oneInterrupt) {
                                 __no_operation();
                             }

                         }
                         for (i=15; i>=0; i--) { // 49
                             DACSetValue(dac_Codes[i]);
                             oneInterrupt = timer_cnt;
                             while (timer_cnt == oneInterrupt) {
                                 __no_operation();
                             }

                         }

                         oneInterrupt = timer_cnt;

                         break;
                     }

                     }
               }

    return 0;
}


long double ADCconfigP61andSC(void) {
    REFCTL0 &= ~REFMSTR;

     // Initialize control register ADC12CTL0 = 0000 1001 0111 0000
     // SHT0x=9h (384 clk cycles), MCS=1=burst thru selected chans.,
     // REF2_5V = 0 (1.5V), REFON = 1 = use internal reference volts
     // and ADC12ON = 1 = turn ADC on
     ADC12CTL0 = ADC12SHT0_9|ADC12ON|ADC12MSC;
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
      ADC12MCTL1 = ADC12SREF_0 + ADC12INCH_1 + ADC12EOS;
      __delay_cycles(100); // delay to allow Ref to settle
      P6SEL = P6SEL | (BIT0);//BIT1

       ADC12IE = BIT1; //BIT1
       _BIS_SR(GIE);
       ADC12CTL0 &= ~ADC12SC;
       ADC12CTL0 |= ADC12SC + ADC12ENC;
       floatVoltage = ((double)(((long)codeValue)/4095.0));
       floatVoltage = ((double)(floatVoltage * ((long)3.3)));
       return floatVoltage;
}

void displayTime(long unsigned int time_cnt_cpy){

    long unsigned int days = (time_cnt_cpy / (long)86400);
    long unsigned int hours = (time_cnt_cpy - days * (long)86400) / (long)3600;
    time_cnt_cpy = time_cnt_cpy - days * ((long)86400);
    long unsigned int minutes = (time_cnt_cpy - ((long)3600) * hours) / ((long)60);
    time_cnt_cpy = time_cnt_cpy - ((long) 3600) * hours;
    long unsigned seconds = time_cnt_cpy - ((long)60) * minutes;

    long unsigned int day = days % 365;

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
    float tempF = tempC_cpy * 1.8 + 32.0;
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

void DACInit(void)
{
    // Configure LDAC and CS for digital IO outputs
    DAC_PORT_LDAC_SEL &= ~DAC_PIN_LDAC;
    DAC_PORT_LDAC_DIR |=  DAC_PIN_LDAC;
    DAC_PORT_LDAC_OUT |= DAC_PIN_LDAC; // Deassert LDAC
    DAC_PORT_CS_SEL   &= ~DAC_PIN_CS;
    DAC_PORT_CS_DIR   |=  DAC_PIN_CS;
    DAC_PORT_CS_OUT   |=  DAC_PIN_CS;  // Deassert CS
}

void DACSetValue(unsigned int dac_code)
{
    // Start the SPI transmission by asserting CS (active low)
    // This assumes DACInit() already called
    DAC_PORT_CS_OUT &= ~DAC_PIN_CS;
    // Write in DAC configuration bits. From DAC data sheet
    // 3h=0011 to highest nibble.
    // 0=DACA, 0=buffered, 1=Gain=1, 1=Out Enbl
    dac_code |= 0x3000;     // Add control bits to DAC word
    uint8_t lo_byte = (unsigned char)(dac_code & 0x00FF);
    uint8_t hi_byte = (unsigned char)((dac_code & 0xFF00) >> 8);
    // First, send the high byte
    DAC_SPI_REG_TXBUF = hi_byte;
    // Wait for the SPI peripheral to finish transmitting
    while(!(DAC_SPI_REG_IFG & UCTXIFG)) {
        _no_operation();
    }
    // Then send the low byte
    DAC_SPI_REG_TXBUF = lo_byte;
    // Wait for the SPI peripheral to finish transmitting
    while(!(DAC_SPI_REG_IFG & UCTXIFG)) {
        _no_operation();
    }
    // We are done transmitting, so de-assert CS (set = 1)
    DAC_PORT_CS_OUT |=  DAC_PIN_CS;
    // This DAC is designed such that the code we send does not
    // take effect on the output until we toggle the LDAC pin.
    // This is because the DAC has multiple outputs. This design
    // enables a user to send voltage codes to each output and
    // have them all take effect at the same time.
    DAC_PORT_LDAC_OUT &= ~DAC_PIN_LDAC;  // Assert LDAC
     __delay_cycles(10);                 // small delay
    DAC_PORT_LDAC_OUT |=  DAC_PIN_LDAC;  // De-assert LDAC
}

unsigned char getKeys(void)
{
    // Returns ASCII value of key pressed from keypad or 0.
    // Does not decode or detect when multiple keys pressed.
    // smj -- 27 Dec 2015
    // Updated -- 14 Jan 2018
    unsigned char ret_val = 0;
    while (ret_val == 0) {
            // Set Col1 = ?, Col2 = ? and Col3 = ?
            P1OUT &= ~BIT5;
            P2OUT |= (BIT5|BIT4);
            // Now check value from each rows
            if ((P4IN & BIT3)==0)
                ret_val = '1';
            if ((P1IN & BIT2)==0)
                ret_val = '4';
            if ((P1IN & BIT3)==0)
                ret_val = '7';
            if ((P1IN & BIT4)==0)
                ret_val = '*';
            P1OUT |= BIT5;
            // Set Col1 = ?, Col2 = ? and Col3 = ?
            P2OUT &= ~BIT4;
            // Now check value from each rows
            if ((P4IN & BIT3)==0)
                ret_val = '2';
            if ((P1IN & BIT2)==0)
                ret_val = '5';
            if ((P1IN & BIT3)==0)
                ret_val = '8';
            if ((P1IN & BIT4)==0)
                ret_val = '0';
            P2OUT |= BIT4;
            // Set Col1 = ?, Col2 = ? and Col3 = ?
            P2OUT &= ~BIT5;
            // Now check value from each rows
            if ((P4IN & BIT3)==0)
                ret_val = '3';
            if ((P1IN & BIT2)==0)
                ret_val = '6';
            if ((P1IN & BIT3)==0)
                ret_val = '9';
            if ((P1IN & BIT4)==0)
                ret_val = '#';
            P2OUT |= BIT5;
    }
    return(ret_val);
}
