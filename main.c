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
#include <stdio.h>
#include <stdlib.h>
#include<time.h>


// Function Prototypes
void swDelay(char numLoops);

void lightUpLedBuzz(int arrayVal, int delay);

void outKeyLcd(int key);

void buzzerOn(int pwmVal);

void buzzerOff(void);

int createRandomNum(void);

unsigned char getKeys(void);



// Declare globals here

// Main
void main(void)
{

    unsigned char currKey=0;
    int delayTime=(BIT5|BIT2);
    int state=0, counter=0, index=0, randomNum=0, num=0;
    int arr[32]={0};




    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer. Always need to stop this!!
                                 // You can then configure it properly, if desired

    initLeds();

    configDisplay();
    configKeypad();

    // *** Intro Screen ***
    Graphics_clearDisplay(&g_sContext); // Clear the display


    srand(time(0));


    while (1)    // Forever loop
    {
        currKey = getKey();
        // case where user wants to restart the game
        if((currKey == '*') && (counter == 1)) {
            state=0;
            counter=0;
            index=0;
            currKey=0;
            randomNum=0;
            delayTime=36;
            num=0;
            int reset;
            for (reset = 0; reset < 32; reset++){
                arr[reset] = 0;
            }
            Graphics_clearDisplay(&g_sContext); // Clear the display
        }

        // state machine for "Simon Says" game
        switch (state) {

            case 0:
            {
                state=0;
                 counter=0;
                 index=0;
                 currKey=0;
                 randomNum=0;
                 delayTime=36;
                 num=0;
                 int reset;
                 for (reset = 0; reset < 32; reset++){
                     arr[reset] = 0;
                 }

                Graphics_drawStringCentered(&g_sContext, "ECE 2049 SIMON", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT); // Display welcome screen
                Graphics_drawStringCentered(&g_sContext, "Press * to Start", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT); // Display welcome screen
                Graphics_flushBuffer(&g_sContext);
                currKey = getKey();

                if (currKey == '*') {

                    state = 1;
                    counter = 1;
                    Graphics_clearDisplay(&g_sContext); // Clear the display
                    Graphics_drawStringCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 32, 15, TRANSPARENT_TEXT);
                    Graphics_flushBuffer(&g_sContext);
                    swDelay(BIT2 | BIT5);
                    Graphics_drawStringCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
                    Graphics_flushBuffer(&g_sContext);
                    swDelay(BIT2 | BIT5);
                    Graphics_drawStringCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 64, 15, TRANSPARENT_TEXT);
                    Graphics_flushBuffer(&g_sContext);
                    swDelay(BIT2 | BIT5);

                }

                break;
            }
            case 1:
            {
                Graphics_clearDisplay(&g_sContext); // Clear the display


                // When you WIN (passed 32nd round)
                if(arr[31] != 0) {
                    state = 4;
                    break;
                }


                randomNum = createRandomNum();
                arr[index] = randomNum;
                index++;

                int items;
                for(items=0; items<index; items++){


                    lightUpLedBuzz(arr[items], delayTime);
                }

                delayTime--;
                state = 2;

                break;
            }

            case 2: // Check Player Input
            {
                int check=0; // !!!
                for(check=0; check<index; check++){
                        currKey = getKeys();
                        num = currKey - '0';
                        lightUpLedBuzz(num, BIT3);
                        outKeyLcd(num);

                        if((arr[check] != num) && (num != '*')) {
                            state = 3;
                            break;
                        }

                  }
                swDelay(BIT3);

                if(state == 3) break;

                state = 1; // Go back for next round
                break;
            }
            case 3: {
                Graphics_clearDisplay(&g_sContext); // Clear the display
                Graphics_drawStringCentered(&g_sContext, "Game Over!", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);

                int k=0;
                int i=16;
                while (i != 0){
                    k = (k % 4) + 1;
                    lightUpLedBuzz(k, BIT2);
                    i--;
                }

                swDelay(BIT6);
                state = 0;
                Graphics_clearDisplay(&g_sContext); // Clear the display
                break;
            }
            case 4: {
                Graphics_clearDisplay(&g_sContext); // Clear the display
                Graphics_drawStringCentered(&g_sContext, "You Won!", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);

                int k=0;
                int i=0;
                while (i != 16){
                    k = (k % 4) + 1;
                    lightUpLedBuzz(k, BIT2);
                    i++;
                }

                swDelay(BIT6);
                state = 0;
                Graphics_clearDisplay(&g_sContext); // Clear the display
                break;
            }
        }



    }  // end while (1)
}


void swDelay(char numLoops)
{
	// This function is a software delay. It performs
	// useless loops to waste a bit of time
	//
	// Input: numLoops = number of delay loops to execute
	// Output: none
	//
	// smj, ECE2049, 25 Aug 2013

	volatile unsigned int i,j;	// volatile to prevent removal in optimization
			                    // by compiler. Functionally this is useless code

	for (j=0; j<numLoops; j++)
    {
    	i = 3000 ;					// SW Delay
   	    while (i > 0)				// could also have used while (i)
	       i--;
    }
}


void lightUpLedBuzz(int arrayVal, int delay) // Have a delay of 0 in user input and a "delayTime" delay when outputting the next sequence
{
    switch(arrayVal){

        case 1:
            setLeds(BIT3);
            buzzerOn(BIT2|BIT3|BIT4|BIT5|BIT6|BIT7);
            swDelay(delay);
            setLeds(0);
            buzzerOff();
            swDelay(delay);
            break;
        case 2:
            setLeds(BIT2);
            buzzerOn(BIT0|BIT2|BIT3|BIT4|BIT5|BIT7);
            swDelay(delay);
            setLeds(0);
            buzzerOff();
            swDelay(delay);
            break;
        case 3:
            setLeds(BIT1);
            buzzerOn(BIT1|BIT2|BIT3|BIT4|BIT5|BIT6);
            swDelay(delay);
            setLeds(0);
            buzzerOff();
            swDelay(delay);
            break;
        case 4:
            setLeds(BIT0);
            buzzerOn(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);
            swDelay(delay);
            setLeds(0);
            buzzerOff();
            swDelay(delay);
            break;
    }
}



void outKeyLcd(int key)
{
    switch(key){

        case 1:
            Graphics_clearDisplay(&g_sContext); // Clear the display
            Graphics_drawStringCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 20, 48, TRANSPARENT_TEXT);
            Graphics_flushBuffer(&g_sContext);
            break;

        case 2:
                Graphics_clearDisplay(&g_sContext); // Clear the display
                Graphics_drawStringCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 40, 48, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                break;
        case 3:
                Graphics_clearDisplay(&g_sContext); // Clear the display
                Graphics_drawStringCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 60, 48, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                break;
        case 4:
                Graphics_clearDisplay(&g_sContext); // Clear the display
                Graphics_drawStringCentered(&g_sContext, "4", AUTO_STRING_LENGTH, 80, 48, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                break;
    }
}

/*
 * Enable a PWM-controlled buzzer on P3.5
 * This function makes use of TimerB0.
 */
void buzzerOn(int pwmVal)
{
    // Initialize PWM output on P3.5, which corresponds to TB0.5
    P3SEL |= BIT5; // Select peripheral output mode for P3.5
    P3DIR |= BIT5;

    TB0CTL  = (TBSSEL__ACLK|ID__1|MC__UP);  // Configure Timer B0 to use ACLK, divide by 1, up mode
    TB0CTL  &= ~TBIE;                       // Explicitly Disable timer interrupts for safety

    // Now configure the timer period, which controls the PWM period
    // Doing this with a hard coded values is NOT the best method
    // We do it here only as an example. You will fix this in Lab 2.
    TB0CCR0   = pwmVal;                    // Set the PWM period in ACLK ticks
    TB0CCTL0 &= ~CCIE;                  // Disable timer interrupts

    // Configure CC register 5, which is connected to our PWM pin TB0.5
    TB0CCTL5  = OUTMOD_7;                   // Set/reset mode for PWM
    TB0CCTL5 &= ~CCIE;                      // Disable capture/compare interrupts
    TB0CCR5   = TB0CCR0/2;                  // Configure a 50% duty cycle
}

/*
 * Disable the buzzer on P7.5
 */
void buzzerOff(void)
{
    // Disable both capture/compare periods
    TB0CCTL0 = 0;
    TB0CCTL5 = 0;
}

/*
 * Creates random number
 *
 */
int createRandomNum(void)
{
    int num = (rand() % 4) + 1;
    return num;
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

