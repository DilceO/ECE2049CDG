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

// Function Prototypes
void swDelay(char numLoops);

// Enumerations and Class Objects
enum GAME_STATE
  {
    START_SCREEN,
    COUNTDOWN,
    LEVEL_1,
    LEVEL_2,
    LEVEL_3,
    LEVEL_4,
    LEVEL_5,
    WIN_SCREEN,
    DEATH_SCREEN
  };
//
enum GAME_STATE game_state = START_SCREEN; // set initial switch case to the start screen
//
// Declare globals here

// Main
void main(void)

{
    unsigned char currKey=0;
    unsigned char dispThree[3];
    // int32_t textSize = 40;


    // Define some local variables

    long int n = 0;
    long int incr = 0;
    volatile unsigned int i;
//    float a_flt = 190.68;
//    float  test = 0x0600, i=0;     // In C prefix 0x means the number that follows is in hex
//    long unsigned X= 123456;    // No prefix so number is assumed to be in decimal
//    unsigned char myGrade='A';
//    unsigned char initial='S';
    //unsigned char your_name[14] = "Your Name Here";
                                    // What happens when you change the array length?
                                    // What should it be? Do you need null terminator /n ?


    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer. Always need to stop this!!
                                 // You can then configure it properly, if desired

    // Some utterly useless instructions -- Step through them
    // What size does the Code Composer MSP430 Compiler use for the
    // following variable types? A float, an int, a long integer and a char?

//    a_flt = a_flt*test;
//    X = test+X;
//    test = test-myGrade;    // A number minus a letter?? What's actually going on here?
                            // What value stored in myGrade (i.e. what's the ASCII code for "A")?
                            // Thus, what is the new value of test? Explain?

    // Useful code starts here
    initLeds();
    configDisplay();
    configKeypad();

    // *** Screen ***
    Graphics_clearDisplay(&g_sContext); // Clear the display

    switch(game_state) {

    case START_SCREEN: // Display welcome screen
    
        Graphics_drawStringCentered(&g_sContext, "SPACE INVADERS", AUTO_STRING_LENGTH, 48, 40, TRANSPARENT_TEXT);
        Graphics_drawStringCentered(&g_sContext, "Press * to Start", AUTO_STRING_LENGTH, 48, 56, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext);
        swDelay(5);
        // Graphics_clearDisplay(&g_sContext); // Clear the display
        game_state = COUNTDOWN;


        // Check if any keys have been pressed on the 3x4 keypad
        currKey = getKey();
           if (currKey == '*') {
              game_state = COUNTDOWN;
              Graphics_clearDisplay(&g_sContext); // Clear the display
              Graphics_drawStringCentered(&g_sContext, "Going", AUTO_STRING_LENGTH, 48, 56, TRANSPARENT_TEXT);
              Graphics_flushBuffer(&g_sContext);
           }
    break;
    


    case COUNTDOWN: // Intermediate state before level 1
    
        Graphics_drawStringCentered(&g_sContext, "GOT HERE", AUTO_STRING_LENGTH, 48, 40, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext);
        swDelay(5);
        Graphics_clearDisplay(&g_sContext); // Clear the display

       if (n < 10) {
           Graphics_drawStringCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 48, 40, TRANSPARENT_TEXT);
           Graphics_flushBuffer(&g_sContext);

       }
       else if ((n > 10) && (n < 20)) {
           Graphics_drawStringCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 48, 40, TRANSPARENT_TEXT);
           Graphics_flushBuffer(&g_sContext);
           dispThree[0] = ' ';
           dispThree[2] = ' ';
       }
       else if (n > 20 && n < 30) {
           Graphics_drawStringCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 48, 40, TRANSPARENT_TEXT);
           Graphics_flushBuffer(&g_sContext);
           dispThree[0] = ' ';
           dispThree[2] = ' ';
       }
       else if (n > 30) {
           game_state = LEVEL_1;
       }

       n = n + 1;

       if (game_state == LEVEL_1){
           n = 0;
       }
       break;
    


    case LEVEL_1:
    

        Graphics_drawStringCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 8, 5, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext);
        dispThree[0] = ' ';
        dispThree[2] = ' ';
    


    case LEVEL_2:

    case LEVEL_3:

    case LEVEL_4:

    case LEVEL_5:

    case WIN_SCREEN:

        while (i < 15) {
        Graphics_drawStringCentered(&g_sContext, "CONGRATULATIONS!", AUTO_STRING_LENGTH, 48, 48, TRANSPARENT_TEXT);
        Graphics_drawStringCentered(&g_sContext, "YOU WON!", AUTO_STRING_LENGTH, 48, 32, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext);
        dispThree[0] = ' ';
        dispThree[2] = ' ';

        BuzzerOn();
        setLeds(currKey - 0x30);
        swDelay(1);
        BuzzerOff();
        setLeds(currKey - 0x30);
        swDelay(1);
        BuzzerOn();
        setLeds(currKey - 0x30);
        swDelay(1);
        BuzzerOff();
        setLeds(currKey - 0x30);
        swDelay(1);
        BuzzerOn();
        setLeds(currKey - 0x30);
        swDelay(1);
        BuzzerOff();
        setLeds(currKey - 0x30);
        swDelay(1);
        BuzzerOn();
        setLeds(currKey - 0x30);
        swDelay(5);
        BuzzerOff();
        setLeds(currKey - 0x30); //10 might not be allowed, maybe A? Maybe needs to be 9 or less
        swDelay(4);
        i = i + 1;
        }
        if (i == 15) {
            game_state = START_SCREEN;
            i = 0;
        }
        break;

    case DEATH_SCREEN:

        while (i < 15) {
        Graphics_drawStringCentered(&g_sContext, "GAME OVER", AUTO_STRING_LENGTH, 48, 40, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext);
        dispThree[0] = ' ';
        dispThree[2] = ' ';

        BuzzerOn();
        swDelay(3);
        BuzzerOff();
        swDelay(1);
        BuzzerOn();
        swDelay(1);
        BuzzerOff();
        swDelay(1);
        BuzzerOn();
        swDelay(1);
        BuzzerOff();
        swDelay(1);
        BuzzerOn();
        swDelay(1);
        BuzzerOff();
        swDelay(2);
        i = i + 1;
        }

        if (i == 15) {
           game_state = START_SCREEN;
           i = 0;
        }
        break;
    }



    // Write some text to the display (lab 0 and test stuff)
//    Graphics_drawStringCentered(&g_sContext, "W", AUTO_STRING_LENGTH, 88, 5, TRANSPARENT_TEXT); // X axes are from 10 to 90 (spaces we can use with this size of letter, increase left to right)
//    Graphics_drawStringCentered(&g_sContext, "W", AUTO_STRING_LENGTH, 72, 5, TRANSPARENT_TEXT); // Y axes we can use are 5 to 85 (increase top to bottom)
//    Graphics_drawStringCentered(&g_sContext, "W", AUTO_STRING_LENGTH, 56, 5, TRANSPARENT_TEXT); // Gives us 9 columns and 9 rows to work with, but need 5 columns, x amount of rows
//    Graphics_drawStringCentered(&g_sContext, "W", AUTO_STRING_LENGTH, 40, 5, TRANSPARENT_TEXT); // Have a distance of 16 between each alien
//    Graphics_drawStringCentered(&g_sContext, "W", AUTO_STRING_LENGTH, 24, 5, TRANSPARENT_TEXT); // Need to fix these x-values to have only 5 spaces
//    Graphics_drawStringCentered(&g_sContext, "W", AUTO_STRING_LENGTH, 8, 5, TRANSPARENT_TEXT);
//    Graphics_drawStringCentered(&g_sContext, "W", AUTO_STRING_LENGTH, 8, 5, TRANSPARENT_TEXT);




//    Graphics_drawStringCentered(&g_sContext, "CONGRATULATIONS!", AUTO_STRING_LENGTH, 48, 48, TRANSPARENT_TEXT);
//           Graphics_drawStringCentered(&g_sContext, "YOU WON!", AUTO_STRING_LENGTH, 48, 32, TRANSPARENT_TEXT);
//           Graphics_flushBuffer(&g_sContext);
//
//           dispThree[0] = ' ';
//           dispThree[2] = ' ';
//           BuzzerOn();
//           setLeds('5' - 0x30); // 5 - 0x30
//           swDelay(1);
//           BuzzerOff();
//           setLeds('9' - 0x30);
//           swDelay(1);
//           BuzzerOn();
//           setLeds('5' - 0x30);
//           swDelay(1);
//           BuzzerOff();
//           setLeds('9' - 0x30);
//           swDelay(1);
//           BuzzerOn();
//           setLeds('5' - 0x30);
//           swDelay(1);
//           BuzzerOff();
//           setLeds('9' - 0x30);
//           swDelay(1);
//           BuzzerOn();
//           setLeds('5' - 0x30);
//           swDelay(5);
//           BuzzerOff();
//           setLeds('9' - 0x30); //10 might not be allowed, maybe A? Maybe needs to be 9 or less
//           swDelay(4);


//    Graphics_drawStringCentered(&g_sContext, "to", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
//    Graphics_drawStringCentered(&g_sContext, "ECE2049-A21!", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);

    // Draw a box around everything because it looks nice
//    Graphics_Rectangle box = {.xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
//    Graphics_drawRectangle(&g_sContext, &box);

    // We are now done writing to the display.  However, if we stopped here, we would not
    // see any changes on the actual LCD.  This is because we need to send our changes
    // to the LCD, which then refreshes the display.
    // Since this is a slow operation, it is best to refresh (or "flush") only after
    // we are done drawing everything we need.
//    Graphics_flushBuffer(&g_sContext);
//
//    dispThree[0] = ' ';
//    dispThree[2] = ' ';

//    while (1)    // Forever loop
//    {
//        // Check if any keys have been pressed on the 3x4 keypad
//        currKey = getKey();
//        if (currKey == '*')
//            BuzzerOn();
//        if (currKey == '#')
//            BuzzerOff();
//        if ((currKey >= '0') && (currKey <= '9'))
//            setLeds(currKey - 0x30);
//
//        if (currKey)
//        {
//            dispThree[1] = currKey;
//            // Draw the new character to the display
//            Graphics_drawStringCentered(&g_sContext, dispThree, dispSz, 48, 55, OPAQUE_TEXT);
//
//            // Refresh the display so it shows the new data
//            Graphics_flushBuffer(&g_sContext);
//
//            // wait awhile before clearing LEDs
//            swDelay(1);
//            setLeds(0);
//        }

//    }  // end while (1)
}


void swDelay(char numLoops)
{
	// This function is a software delay. It performs
	// useless loops to waste a bit of time
	//
	// Input: numLoops = number of delay loops to execute
	// Output: none
	//
	// smj, ECE2049, 25 Aug 2021

	volatile unsigned int i,j;	// volatile to prevent removal in optimization
			                    // by compiler. Functionally this is useless code

	for (j=0; j<numLoops; j++)
    {
    	i = 50000 ;					// SW Delay
   	    while (i > 0)				// could also have used while (i)
	       i--;
    }
}
