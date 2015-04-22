/*******************************************************************************
  MPLAB Harmony Project Main Source File

  Company:
    Microchip Technology Inc.
  
  File Name:
    main.c

  Summary:
    This file contains the "main" function for an MPLAB Harmony project.

  Description:
    This file contains the "main" function for an MPLAB Harmony project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state 
    machines of all MPLAB Harmony modules in the system and it calls the 
    "SYS_Tasks" function from within a system-wide "super" loop to maintain 
    their correct operation. These two functions are implemented in 
    configuration-specific files (usually "system_init.c" and "system_tasks.c")
    in a configuration-specific folder under the "src/system_config" folder 
    within this project's top-level folder.  An MPLAB Harmony project may have
    more than one configuration, each contained within it's own folder under
    the "system_config" folder.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

//Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "system/common/sys_module.h"   // SYS function prototypes

//One of these defines the TRISBbits register. 
#include <stdio.h>
#include <stdlib.h>
#include<xc.h> // processor SFR definitions
#include<sys/attribs.h> // __ISR macro

#include "i2c_master_int.h" //use "" to search for the proper directory
#include "i2c_display.h"
#include "accel.h"


// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

void printOLED(int ascii, int startposx, int startposy);

int main ( void )
{
    /* Initialize all MPLAB Harmony modules, including application(s). */
    SYS_Initialize ( NULL );
    acc_setup();
    
    // set up B4 digital output for powering OLED screen
    TRISBbits.TRISB4 = 0; //digital output
    LATBbits.LATB4 = 1; //output on=1 initially

    int delay = 0;
    while(delay<4000) {
        delay++;
    }
    
    //ACCELEROMETER
    short accels[3]; // accelerations for the 3 axes
    short mags[3]; // magnetometer readings for the 3 axes
    short temp;

    display_init();

    while ( true )
    {
        // read the accelerometer from all three axes
        // the accelerometer and the pic32 are both little endian by default
        //(the lowest address has the LSB)
        // the accelerations are 16-bit twos compliment numbers, the same as a short
        acc_read_register(OUT_X_L_A, (unsigned char *) accels, 6);
        // need to read all 6 bytes in one transaction to get an update.
        acc_read_register(OUT_X_L_M, (unsigned char *) mags, 6);
        // read the temperature data. Its a right justified 12 bit two's compliment number
        acc_read_register(TEMP_OUT_L, (unsigned char *) &temp, 2);

        /* - for debugging
        char message[25];
        sprintf(message, "%3.2f,%3.2f,%3.2f", accels[0]/16000.0,accels[1]/16000.0,accels[2]/16000.0);
        int i=0;
        while(message[i])
        {
            printOLED(message[i],28,32+5*i);
            i++;
        }
        
        display_clear();
        int startrow = 32;
        int startcol = 64;
        int d;
        //x direction
        for (d = 0; d<1+abs(20*accels[0]/16000.0); d++) {
            if (accels[0]>0){
                display_pixel_set(startrow,startcol-d,1);
                display_pixel_set(startrow+1,startcol-d,1);
            }
            else {
                display_pixel_set(startrow,startcol+d,1);
                display_pixel_set(startrow+1,startcol+d,1);
            }
        }

        //y direction
        for (d = 0; d<1+abs(20*accels[1]/16000.0); d++) {
            if (accels[1]>0){
                display_pixel_set(startrow-d,startcol,1);
                display_pixel_set(startrow-d,startcol+1,1);
            }
            else {
                display_pixel_set(startrow+d,startcol,1);
                display_pixel_set(startrow+d,startcol+1,1);
            }
        }
        display_draw();
        */

        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks( );
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

void printOLED(int ascii, int startrow, int startcol) {
    int row, col;
    int ascii_num = ascii-0x20;
    for (col = 0; col < 5; col++){
        for (row = 0; row < 8; row++) {
            int totrow = startrow+row;
            int totcol = startcol+col;
            if(totrow<HEIGHT && totcol<WIDTH) { //
                int digit = !!((1 << row) & ASCII[ascii_num][col]); //1000 1000 = 136
                display_pixel_set(totrow,totcol,digit);
            }
         }
    }
}