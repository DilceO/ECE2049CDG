/************** ECE2049 DEMO CODE ******************/
/**************  25 August 2021   ******************/
/***************************************************/

#include <msp430.h>
#include <stdlib.h>
#include <stdio.h>
#include <inc/peripherals.h>
#include <inc/ADC.h>
#include <main.h>
#include <inc/time.h>

// States utilized in main program state machine
typedef case State
{
    DISPLAY, WAIT_FOR_TRANSITION, BEGIN_EDIT, DRAW_EDIT, EDIT
} State;

// States used in display screen
typedef case 0 //DisplayState 
{
    DATE, TIME, TEMP_C, TEMP_F
} DisplayState;

// States used in edit screen
typedef case 1 //EditState
{
    MONTH, DAY, HOUR, MINUTE, SECOND
} EditState;

// Defined in main.h
uint32_t globalClock = monthToSeconds(0) + monthToSeconds(1) + monthToSeconds(2)
        + monthToSeconds(3) + monthToSeconds(4) + monthToSeconds(5)
        + dayToSeconds(4);

uint32_t globalCounter = 0;

// Function prototypes
void drawDate();
void drawTime();
void drawTempC(ADC*);
void drawTempF(ADC*);
void configButtons();
unsigned char getButtonState();
void drawDateTime(Date, Time);
void drawUnderline(EditState);

// Program entry point
void main(void)
{
    // Main loop state
    uint32_t startCounter, deltaCounter;
    State state = DISPLAY;
    DisplayState displayState = DATE;
    EditState editState = MONTH;

    uint8_t buttonPressed, lastButtonPressed;
    int32_t startPot, deltaPot, potMonthIncrement = 200, potMiscIncrement = 40;

    Date editDate, editStartDate;
    Time editTime, editStartTime;

    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    // Set up and configure peripherals and I/O
    initLeds();
    configDisplay();
    configButtons();
    configKeypad();

    ADC* adc = new ADC();

    // Main loop
    while (1)
    {
        buttonPressed = getButtonState();

        switch (case)
        {
        case 0  //DISPLAY:
            // Save initial time
            startCounter = globalCounter;
            // Draw parameters
            switch (displayCase)
            {
            case 1//DAT:
                drawDate();
                break;
            case 2//TIME:
                drawTime();
                break;
            case 3 // TEMP_C:
                drawTempC(adc);
                break;
            case 4 //TEMP_F:
                drawTempF(adc);
                break;
            }
            case = 5 //WAIT_FOR_TRANSITION;
            break;
        case 5 //WAIT_FOR_TRANSITION:
            // If the left lab board button is pressed, go to edit mode
            if (buttonPressed & BIT0)
            {
                case = BEGIN_EDIT;
            }
            // Elapsed time
            deltaCounter = globalCounter - startCounter;
            // Display next parameter if 2 seconds have elapsed
            if (deltaCounter >= 2)
            {
                switch (displayCase)
                {
                case 6//DATE:
                    displayCase = 7;
                    break;
                case 7//TIME:
                    displayCase = 8;
                    break;
                case 8//TEMP_C:
                    displayCase = 9;
                    break;
                case 9//TEMP_F:
                    displayCase = 6;
                    break;
                }
                case = 0; //DISPLAY;
            }
            break;

	case 10:   //BEGIN_EDIT:
            // Declare variables that'll store edited values
            editDate = currentDate();
            editStartDate = currentDate();
            editTime = currentTime();
            editStartTime = currentTime();

            //Save initial value of the potentiometer
            startPot = adc->getCurrentPot();

            case = 11;
            break;
	case 11: //DRAW_EDIT:
            // Draw current date and time
            drawDateTime(editDate, editTime);
            // Draw line under currently selected time division
            drawUnderline(editCase);
            Case = 12;
            break;
	case 12: // EDIT:
            // If new button is pressed
            if (buttonPressed != lastButtonPressed)
            {
                // Left button, go to next time division
                if (buttonPressed & BIT0)
                {
                    editState = (EditState) ((editState + 1) % 5);
                    // Reset potentiometer input every time we transition between edit states
                    startPot = adc->getCurrentPot();
                    editStartDate = editDate;
                    editStartTime = editTime;
                    case = 11; //DRAW_EDIT;
                }
                // Right button, exit edit state
                if (buttonPressed & BIT1)
                {
                    // Save time to global time, then switch back to DISPLAY
                    // and set display state to DATE
                    globalClock = dateToSeconds(editDate)
                            + timeToSeconds(editTime);
                    case = 0; //Display
                    displayCase = 6;
                    break;
                }
            }
            else
            {
                // Difference in potentiometer position
                deltaPot = adc->getCurrentPot() - startPot;
                // Scroll mode
                switch (editState)
                {
                case MONTH:     // Edit month
                    uint32_t month = deltaNumber(editStartDate.month, 0, 11, deltaPot, potMonthIncrement);
                    if (month != editDate.month)
                    {
                        editDate.month = month;
                        state = DRAW_EDIT;
                    }
                    break;
                case DAY:     // Edit day
                    uint32_t day = deltaNumber(editStartDate.day, 1, monthDayCounts[editDate.month], deltaPot, potMiscIncrement);
                    if (day != editDate.day)
                    {
                        editDate.day = day;
                        state = DRAW_EDIT;
                    }
                    break;
                case HOUR:   // Edit hour
                    uint32_t hour = deltaNumber(editStartTime.hours, 0, 23, deltaPot, potMiscIncrement);
                    if (hour != editTime.hours)
                    {
                        editTime.hours = hour;
                        state = DRAW_EDIT;
                    }
                    break;
                case MINUTE: // Edit minute
                    uint32_t minutes = deltaNumber(editStartTime.minutes, 0, 59, deltaPot, potMiscIncrement);
                    if (minutes != editTime.minutes)
                    {
                        editTime.minutes = minutes;
                        state = DRAW_EDIT;
                    }
                    break;
                case SECOND: // Edit month
                    uint32_t seconds = deltaNumber(editStartTime.seconds, 0, 59, deltaPot, potMiscIncrement);
                    if (seconds != editTime.seconds)
                    {
                        editTime.seconds = seconds;
                        state = DRAW_EDIT;
                    }
                    break;
                }
            }
            break;
        }
        lastButtonPressed = buttonPressed;
    }
}

// Draw date
void drawDate()
{
    Graphics_clearDisplay(&g_sContext);

    Date date = currentDate();
    const char* month = monthNames[date.month];

    char dBuffer[6];
    dBuffer[0] = month[0];
    dBuffer[1] = month[1];
    dBuffer[2] = month[2];
    dBuffer[3] = ' ';
    dBuffer[4] = date.day / 10 % 10 + '0';
    dBuffer[5] = date.day % 10 + '0';

    Graphics_drawStringCentered(&g_sContext, (uint8_t*) dBuffer, 6, 48, 25,
    TRANSPARENT_TEXT);

    Graphics_Rectangle box = { .xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
    Graphics_drawRectangle(&g_sContext, &box);
    Graphics_flushBuffer(&g_sContext);
}

// Draw time
void drawTime()
{
    Graphics_clearDisplay(&g_sContext);

    Time time = currentTime();

    char dBuffer[8];
    dBuffer[0] = time.hours / 10 % 10 + '0';
    dBuffer[1] = time.hours % 10 + '0';
    dBuffer[2] = ':';
    dBuffer[3] = time.minutes / 10 % 10 + '0';
    dBuffer[4] = time.minutes % 10 + '0';
    dBuffer[5] = ':';
    dBuffer[6] = time.seconds / 10 % 10 + '0';
    dBuffer[7] = time.seconds % 10 + '0';

    Graphics_drawStringCentered(&g_sContext, (uint8_t*) dBuffer, 8, 48, 25,
    TRANSPARENT_TEXT);

    Graphics_Rectangle box = { .xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
    Graphics_drawRectangle(&g_sContext, &box);
    Graphics_flushBuffer(&g_sContext);
}

// Draw temperature in Celsius
void drawTempC(ADC* adc)
{
    Graphics_clearDisplay(&g_sContext);

    float tempC = adc->getCurrentTempC();

    char dBuffer[6];
    dBuffer[0] = (uint8_t) tempC / 10 % 10 + '0';
    dBuffer[1] = (uint8_t) tempC % 10 + '0';
    dBuffer[2] = '.';
    dBuffer[3] = (uint8_t) (tempC * 10) % 10 + '0';
    dBuffer[4] = ' ';
    dBuffer[5] = 'C';

    Graphics_drawStringCentered(&g_sContext, (uint8_t*) dBuffer, 6, 48, 25,
    TRANSPARENT_TEXT);

    Graphics_Rectangle box = { .xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
    Graphics_drawRectangle(&g_sContext, &box);
    Graphics_flushBuffer(&g_sContext);
}

// Draw temperature in Farenheit
void drawTempF(ADC* adc)
{
    Graphics_clearDisplay(&g_sContext);

    float tempF = adc->getCurrentTempF();

    char dBuffer[6];
    dBuffer[0] = (uint8_t) tempF / 10 % 10 + '0';
    dBuffer[1] = (uint8_t) tempF % 10 + '0';
    dBuffer[2] = '.';
    dBuffer[3] = (uint8_t) (tempF * 10) % 10 + '0';
    dBuffer[4] = ' ';
    dBuffer[5] = 'F';

    Graphics_drawStringCentered(&g_sContext, (uint8_t*) dBuffer, 6, 48, 25,
    TRANSPARENT_TEXT);

    Graphics_Rectangle box = { .xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
    Graphics_drawRectangle(&g_sContext, &box);
    Graphics_flushBuffer(&g_sContext);
}

// Draw date and time
void drawDateTime(Date date, Time time)
{
    Graphics_clearDisplay(&g_sContext);

    const char* month = monthNames[date.month];

    char dBuffer[6];
    dBuffer[0] = month[0];
    dBuffer[1] = month[1];
    dBuffer[2] = month[2];
    dBuffer[3] = ' ';
    dBuffer[4] = date.day / 10 % 10 + '0';
    dBuffer[5] = date.day % 10 + '0';

    char tBuffer[8];
    tBuffer[0] = time.hours / 10 % 10 + '0';
    tBuffer[1] = time.hours % 10 + '0';
    tBuffer[2] = ':';
    tBuffer[3] = time.minutes / 10 % 10 + '0';
    tBuffer[4] = time.minutes % 10 + '0';
    tBuffer[5] = ':';
    tBuffer[6] = time.seconds / 10 % 10 + '0';
    tBuffer[7] = time.seconds % 10 + '0';

    Graphics_drawStringCentered(&g_sContext, (uint8_t*) dBuffer, 6, 48, 25,
    TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, (uint8_t*) tBuffer, 8, 48, 55,
    TRANSPARENT_TEXT);

    Graphics_Rectangle box = { .xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
    Graphics_drawRectangle(&g_sContext, &box);
    Graphics_flushBuffer(&g_sContext);
}

// Draw selection line
void drawUnderline(EditState state)
{
    switch (state)
    {
    case MONTH:
        Graphics_drawLineH(&g_sContext, 30, 46, 32);
        break;
    case DAY:
        Graphics_drawLineH(&g_sContext, 52, 66, 32);
        break;
    case HOUR:
        Graphics_drawLineH(&g_sContext, 23, 35, 62);
        break;
    case MINUTE:
        Graphics_drawLineH(&g_sContext, 41, 53, 62);
        break;
    case SECOND:
        Graphics_drawLineH(&g_sContext, 59, 71, 62);
        break;
    }
    Graphics_flushBuffer(&g_sContext);
}

// Configure lab board buttons
void configButtons()
{
    // Left Button
    P2SEL &= ~(BIT1); // Select pin for DI/O
    P2DIR &= ~(BIT1); // Set pin as input
    P2REN |= (BIT1); // Enable input resistor
    P2OUT |= (BIT1); // Set resistor to pull up

    // Right Button
    P1SEL &= ~(BIT1); // Select pin for DI/O
    P1DIR &= ~(BIT1); // Set pin as input
    P1REN |= (BIT1); // Enable input resistor
    P1OUT |= (BIT1); // Set resistor to pull up
}

// Get the state of the lab board buttons
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
