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

// Declare globals here
long unsigned int timer_cnt = 0;
long unsigned int leap_cnt = 0;

// Function Prototypes
void runtimerA2(void);

void swDelay(char numLoops);

void lightUpLedBuzz(int arrayVal, int delay);

void outKeyLcd(int key);

void buzzerOn(int pwmVal);

void buzzerOff(void);

int createRandomNum(void);

unsigned char getKeys(void);

void configureButtons(void);

char buttonsPressed(void);

void lightUserLED(char inbits);

void configLpadLEDs(void);

void turnOffUserLED(char inbits);


struct finalNote{
    int note; // Note Frequencies
    int duration;  //
    char buttonAssoc;
    int restFromPrevious; // the rest time between previous and present note that is played
    char ledColor; // 1 Yellow, 2 Blue, 4 Green,8 Red
};

void runtimerA2(void){

    TA2CTL = TASSEL_1 + MC_1 + ID_0;
    TA2CCR0 = 163;
    TA2CCTL0 = CCIE;

}


#pragma vector=TIMER2_A0_VECTOR
__interrupt void Timer_A2_ISR(void) {

    if(leap_cnt < 1024) {

        timer_cnt++;
        leap_cnt++;
    } else {

        timer_cnt+=2;
        leap_cnt = 0;
    }

}



// Main
void main(void)
{

    unsigned char currKey=0;
    int delayTime=(BIT5|BIT2);
    int state=0, counter=0, index=0, randomNum=0, num=0;
    struct finalNote notes[46];
    long unsigned int timer_on=0;

    // For Case 11
    int error = 0;
    int isCorrect = false;
    char button = '0';


    //For buzzerOn()

    //Note Number 1
       notes[0].note = 392;
       notes[0].duration = 300;
       notes[0].buttonAssoc = BIT2; // BIT2
       notes[0].restFromPrevious = 0;
       notes[0].ledColor = BIT1; // CHANGE TO BIT1 !!!!!!!!!!!!!!!!!!!!!!


       //Note Number 2
       notes[1].note = 440;
       notes[1].duration = 100;
       notes[1].buttonAssoc = BIT2;
       notes[1].restFromPrevious = 0;
       notes[1].ledColor = BIT1;


       //Note Number 3
       notes[2].note = 392;
       notes[2].duration = 200;
       notes[2].buttonAssoc = BIT2;
       notes[2].restFromPrevious = 0;
       notes[2].ledColor = BIT1;

       //Note Number 4
       notes[3].note = 330;
       notes[3].duration = 600;
       notes[3].buttonAssoc = BIT1;
       notes[3].restFromPrevious = 0;
       notes[3].ledColor = BIT2;

       //Note Number 5
       notes[4].note = 392;
       notes[4].duration = 300;
       notes[4].buttonAssoc = BIT2;
       notes[4].restFromPrevious = 0;
       notes[4].ledColor = BIT1;

       //Note Number 6
       notes[5].note = 440;
       notes[5].duration = 100;
       notes[5].buttonAssoc = BIT2;
       notes[5].restFromPrevious = 0;
       notes[5].ledColor = BIT1;

       //Note Number 7
       notes[6].note = 392;
       notes[6].duration = 200;
       notes[6].buttonAssoc = BIT2;
       notes[6].restFromPrevious = 0;
       notes[6].ledColor = BIT1;

       //Note Number 8
       notes[7].note = 330;
       notes[7].duration = 600;
       notes[7].buttonAssoc = BIT1;
       notes[7].restFromPrevious = 0;
       notes[7].ledColor = BIT2;

       //Note Number 9
       notes[8].note = 294;
       notes[8].duration = 400;
       notes[8].buttonAssoc = BIT0;
       notes[8].restFromPrevious = 0;
       notes[8].ledColor = BIT3;

       //Note Number 10
       notes[9].note = 294;
       notes[9].duration = 400;
       notes[9].buttonAssoc = BIT0;
       notes[9].restFromPrevious = 0;
       notes[9].ledColor = BIT3;

       //Note Number 11
       notes[10].note = 494;
       notes[10].duration = 400;
       notes[10].buttonAssoc = BIT3;
       notes[10].restFromPrevious = 0;
       notes[10].ledColor = BIT0;

       //Note Number 12
       notes[11].note = 523;
       notes[11].duration = 400;
       notes[11].buttonAssoc = BIT3;
       notes[11].restFromPrevious = 0;
       notes[11].ledColor = BIT0;

       //Note Number 13
       notes[12].note = 523;
       notes[12].duration = 200;
       notes[12].buttonAssoc = BIT3;
       notes[12].restFromPrevious = 0;
       notes[12].ledColor = BIT0;

       //Note Number 14
       notes[13].note = 392;
       notes[13].duration = 600;
       notes[13].buttonAssoc = BIT2;
       notes[13].restFromPrevious = 0;
       notes[13].ledColor = BIT1;

       //Note Number 15
       notes[14].note = 440;
       notes[14].duration = 400;
       notes[14].buttonAssoc = BIT2;
       notes[14].restFromPrevious = 0;
       notes[14].ledColor = BIT1;

       //Note Number 16
       notes[15].note = 440;
       notes[15].duration = 200;
       notes[15].buttonAssoc = BIT2;
       notes[15].restFromPrevious = 0;
       notes[15].ledColor = BIT1;

       //Note Number 17
       notes[16].note = 523;
       notes[16].duration = 300;
       notes[16].buttonAssoc = BIT3;
       notes[16].restFromPrevious = 0;
       notes[16].ledColor = BIT0;

       //Note Number 18
       notes[17].note = 494;
       notes[17].duration = 100;
       notes[17].buttonAssoc = BIT3;
       notes[17].restFromPrevious = 0;
       notes[17].ledColor = BIT0;

       //Note Number 19
       notes[18].note = 440;
       notes[18].duration = 200;
       notes[18].buttonAssoc = BIT2;
       notes[18].restFromPrevious = 0;
       notes[18].ledColor = BIT1;

       //Note Number 20
       notes[19].note = 392;
       notes[19].duration = 300;
       notes[19].buttonAssoc = BIT2;
       notes[19].restFromPrevious = 0;
       notes[19].ledColor = BIT1;

       //Note Number 21
       notes[20].note = 440;
       notes[20].duration = 100;
       notes[20].buttonAssoc = BIT2;
       notes[20].restFromPrevious = 0;
       notes[20].ledColor = BIT1;

       //Note Number 22
       notes[21].note = 392;
       notes[21].duration = 200;
       notes[21].buttonAssoc = BIT2;
       notes[21].restFromPrevious = 0;
       notes[21].ledColor = BIT1;

       //Note Number 23
       notes[22].note = 330;
       notes[22].duration = 600;
       notes[22].buttonAssoc = BIT1;
       notes[22].restFromPrevious = 0;
       notes[22].ledColor = BIT2;

       //Note Number 24
       notes[23].note = 440;
       notes[23].duration = 400;
       notes[23].buttonAssoc = BIT2;
       notes[23].restFromPrevious = 0;
       notes[23].ledColor = BIT1;

       //Note Number 25
       notes[24].note = 440;
       notes[24].duration = 200;
       notes[24].buttonAssoc = BIT2;
       notes[24].restFromPrevious = 0;
       notes[24].ledColor = BIT1;

       //Note Number 26
       notes[25].note = 523;
       notes[25].duration = 300;
       notes[25].buttonAssoc = BIT3;
       notes[25].restFromPrevious = 0;
       notes[25].ledColor = BIT0;

       //Note Number 27
       notes[26].note = 494;
       notes[26].duration = 100;
       notes[26].buttonAssoc = BIT3;
       notes[26].restFromPrevious = 0;
       notes[26].ledColor = BIT0;

       //Note Number 28
       notes[27].note = 440;
       notes[27].duration = 200;
       notes[27].buttonAssoc = BIT2;
       notes[27].restFromPrevious = 0;
       notes[27].ledColor = BIT1;

       //Note Number 29
       notes[28].note = 392;
       notes[28].duration = 300;
       notes[28].buttonAssoc = BIT2;
       notes[28].restFromPrevious = 0;
       notes[28].ledColor = BIT1;

       //Note Number 30
       notes[29].note = 440;
       notes[29].duration = 100;
       notes[29].buttonAssoc = BIT2;
       notes[29].restFromPrevious = 0;
       notes[29].ledColor = BIT1;

       //Note Number 31
       notes[30].note = 392;
       notes[30].duration = 200;
       notes[30].buttonAssoc = BIT2;
       notes[30].restFromPrevious = 0;
       notes[30].ledColor = BIT1;

       //Note Number 32
       notes[31].note = 330;
       notes[31].duration = 600;
       notes[31].buttonAssoc = BIT1;
       notes[31].restFromPrevious = 0;
       notes[31].ledColor = BIT2;

       //Note Number 33
       notes[32].note = 294;
       notes[32].duration = 400;
       notes[32].buttonAssoc = BIT0;
       notes[32].restFromPrevious = 0;
       notes[32].ledColor = BIT3;

       //Note Number 34
       notes[33].note = 294;
       notes[33].duration = 200;
       notes[33].buttonAssoc = BIT0;
       notes[33].restFromPrevious = 0;
       notes[33].ledColor = BIT3;

       //Note Number 35
       notes[34].note = 349;
       notes[34].duration = 300;
       notes[34].buttonAssoc = BIT1;
       notes[34].restFromPrevious = 0;
       notes[34].ledColor = BIT2;

       //Note Number 36
       notes[35].note = 294;
       notes[35].duration = 100;
       notes[35].buttonAssoc = BIT0;
       notes[35].restFromPrevious = 0;
       notes[35].ledColor = BIT3;

       //Note Number 37
       notes[36].note = 494;
       notes[36].duration = 200;
       notes[36].buttonAssoc = BIT3;
       notes[36].restFromPrevious = 0;
       notes[36].ledColor = BIT0;

       //Note Number 38
       notes[37].note = 523;
       notes[37].duration = 600;
       notes[37].buttonAssoc = BIT3;
       notes[37].restFromPrevious = 0;
       notes[37].ledColor = BIT0;

       //Note Number 39
       notes[38].note = 330;
       notes[38].duration = 600;
       notes[38].buttonAssoc = BIT1;
       notes[38].restFromPrevious = 0;
       notes[38].ledColor = BIT2;

       //Note Number 40
       notes[39].note = 523;
       notes[39].duration = 300;
       notes[39].buttonAssoc = BIT3;
       notes[39].restFromPrevious = 0;
       notes[39].ledColor = BIT0;

       //Note Number 41
       notes[40].note = 392;
       notes[40].duration = 100;
       notes[40].buttonAssoc = BIT2;
       notes[40].restFromPrevious = 0;
       notes[40].ledColor = BIT1;

       //Note Number 42
       notes[41].note = 330;
       notes[41].duration = 200;
       notes[41].buttonAssoc = BIT1;
       notes[41].restFromPrevious = 0;
       notes[41].ledColor = BIT2;

       //Note Number 43
       notes[42].note = 392;
       notes[42].duration = 300;
       notes[42].buttonAssoc = BIT2;
       notes[42].restFromPrevious = 0;
       notes[42].ledColor = BIT1;

       //Note Number 44
       notes[43].note = 349;
       notes[43].duration = 100;
       notes[43].buttonAssoc = BIT1;
       notes[43].restFromPrevious = 0;
       notes[43].ledColor = BIT2;

       //Note Number 45
       notes[44].note = 294;
       notes[44].duration = 200;
       notes[44].buttonAssoc = BIT0;
       notes[44].restFromPrevious = 0;
       notes[44].ledColor = BIT3;

       //Note Number 46
       notes[45].note = 262;
       notes[45].duration = 800;
       notes[45].buttonAssoc = BIT0;
       notes[45].restFromPrevious = 0;
       notes[45].ledColor = BIT3;



    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer. Always need to stop this!!
                                 // You can then configure it properly, if desired



    initLeds();
    configLpadLEDs();
    configureButtons();
    configDisplay();
    configKeypad();

    // *** Intro Screen ***
    Graphics_clearDisplay(&g_sContext); // Clear the display


    srand(time(0));


    _BIS_SR(GIE); // Global Interrupt enable
    runtimerA2();

    while (1)    // Forever loop
    {





        // state machine for "MSP430 Hero" game
        switch (state) {

            case 0:
            {
                state=0;
                counter=0;


                Graphics_drawStringCentered(&g_sContext, "ECE 2049", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT); // Display welcome screen
                Graphics_drawStringCentered(&g_sContext, "GUITAR HERO", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT); // Display welcome screen
                Graphics_drawStringCentered(&g_sContext, "Press * to Start", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT); // Display welcome screen
                Graphics_flushBuffer(&g_sContext);
                currKey = getKey();

                if (currKey == '*') {

                    state = 1;
                    counter = 1;

                    Graphics_clearDisplay(&g_sContext); // Clear the display
                    timer_on = timer_cnt;
                    while (timer_cnt < (timer_on + 200)) {

                        Graphics_drawStringCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
                        Graphics_flushBuffer(&g_sContext);
                        lightUserLED(1);
                    }

                    Graphics_clearDisplay(&g_sContext); // Clear the display
                    turnOffUserLED(1);
                    timer_on = timer_cnt;
                    while (timer_cnt < (timer_on + 200)) {

                        Graphics_drawStringCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
                        Graphics_flushBuffer(&g_sContext);
                        lightUserLED(2);
                    }

                    Graphics_clearDisplay(&g_sContext); // Clear the display
                    turnOffUserLED(2);
                    timer_on = timer_cnt;
                    while (timer_cnt < (timer_on + 200)) {

                        Graphics_drawStringCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
                        Graphics_flushBuffer(&g_sContext);
                        lightUserLED(1);
                    }

                    Graphics_clearDisplay(&g_sContext); // Clear the display
                    turnOffUserLED(1);
                    timer_on = timer_cnt;
                    while (timer_cnt < (timer_on + 200)) {

                        Graphics_drawStringCentered(&g_sContext, "GO", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
                        Graphics_flushBuffer(&g_sContext);
                        lightUserLED(3);
                    }
                    turnOffUserLED(3);

                }

                break;
            }
            case 1:
            {
                Graphics_clearDisplay(&g_sContext); // Clear the display


                int i;
                for (i=0; i<46; i++) {


                    // case where user wants to restart the game
                    currKey = getKey();
                    if((currKey == '#') && (counter == 1)) { // CHANGED RESET BUTTON to #!!!
                        state=0;
                        counter=0;
                        index=0;
                        currKey=0;
                        randomNum=0;
                        delayTime=36;
                        num=0;
            //            int reset;
            //            for (reset = 0; reset < 32; reset++){
            //                arr[reset] = 0;
            //            }
                        Graphics_clearDisplay(&g_sContext); // Clear the display
                        break;
                    }

                    // PLAYING...

                    isCorrect = false;
                    timer_on = timer_cnt;

                    while (timer_cnt < (timer_on + notes[i].duration)) { // notes[i].duration


//                        int timer_on1 = timer_cnt;
//                        while (timer_cnt < (timer_on1 + notes[i].restFromPrevious)){ // NO NEED (?) - DONE BELOW, AFTER WHILE()...
//                               // DO NOTHING!
//                        }

                        buzzerOn(notes[i].note); // HAVE TO ALTER BUZZERON() -- 32768/notefreq
                        setLeds(notes[i].ledColor);
                        button = buttonsPressed(); //NEED FIX ???




                        // HERE LIGHT LAUNCHPAD LED -- WHEN PRESSED...



                        if(button == notes[i].buttonAssoc){
                            isCorrect = true;
                        }
//                         else {
//                            isCorrect = false;
//                        }

                    }

                    buzzerOff();
                    // Turn off peripheral LEDs
                    setLeds(0);
                    timer_on = timer_cnt;
                    while (timer_cnt < timer_on + 50) {
                        // DO NOTHING!
                    }


                    if (!isCorrect) {
                        error+=1;
                    }

                    if (error==3) { // If user makes 3 mistakes they lose!
                        state=3; // "Game Over" state
                        break;
                    }

                }

                if (state!=3){
                    state = 4; // "You WIN" state
                }


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

//                        if((arr[check] != num) && (num != '*')) {
//                            state = 3;
//                            break;
//                        }

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

                timer_on = timer_cnt;
                while (timer_cnt < (timer_on + 600)){

                    // Keep displaying "Game Over" screen for 3 seconds
                }

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

                timer_on = timer_cnt;
                while (timer_cnt < (timer_on + 600)){

                       // Keep displaying "You Won" screen for 3 seconds
                }


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

    volatile unsigned int i,j;  // volatile to prevent removal in optimization
                                // by compiler. Functionally this is useless code

    for (j=0; j<numLoops; j++)
    {
        i = 3000 ;                  // SW Delay
        while (i > 0)               // could also have used while (i)
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
void buzzerOn(int noteFreq)
{
    unsigned int ACLK = 32768;

    // Initialize PWM output on P3.5, which corresponds to TB0.5
    P3SEL |= BIT5; // Select peripheral output mode for P3.5
    P3DIR |= BIT5;

    TB0CTL  = (TBSSEL__ACLK|ID__1|MC__UP);  // Configure Timer B0 to use ACLK, divide by 1, up mode
    TB0CTL  &= ~TBIE;                       // Explicitly Disable timer interrupts for safety

    // Now configure the timer period, which controls the PWM period
    // Doing this with a hard coded values is NOT the best method
    // We do it here only as an example. You will fix this in Lab 2.
    int inputVal = (ACLK/noteFreq);
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

void lightUserLED(char inbits){

    if ((inbits & BIT0) == 0x01){
        P1OUT |= BIT0;
    }
    if ((inbits & BIT1) == 0x02){
        P4OUT |= BIT7;
    }


}

void turnOffUserLED(char inbits){

    if ((inbits & BIT0) == 0x01){
         P1OUT &= ~BIT0;
     }

     if ((inbits & BIT1) == 0x02){
         P4OUT &= ~BIT7;
     }

}


void configLpadLEDs(){

    P4SEL &= ~(BIT7);
    P1SEL &= ~(BIT0);

    P4DIR |= BIT7;
    P1DIR |= BIT0;

    // Turn Launchpad LEDs OFF
    P1OUT &= ~BIT0;
    P4OUT &= ~BIT7;


}
