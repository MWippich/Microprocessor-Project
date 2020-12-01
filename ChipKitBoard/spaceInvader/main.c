/* mipslabmain.c

   This file written 2015 by Axel Isaksson,
   modified 2015, 2017 by F Lundevall

   Latest update 2019-12-06 by Emmy Yin and Max Wippich

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <stdlib.h>
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "gamefuncs.h"  /* Declatations for these labs */

void initMicrocontroller();
void enable_interrupt();

char seeded = 0;

// For the rand() function
void setSeed(int seed){
  if(seeded){
      return;
  }
  srand(seed);
  seeded = 1;
}

int main(void) {
    // Run All initializations
    initMicrocontroller();
  	display_init();
  	display_update();
    initGameObjects();

    //Start game
    setStars();
    startGame();

    return 0;
}

//Initialize Microcontroller
void initMicrocontroller()
{
    /*
  This will set the peripheral bus clock to the same frequency
  as the sysclock. That means 80 MHz, when the microcontroller
  is running at 80 MHz. Changed 2017, as recommended by Axel.
*/
    SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
    SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
    while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
    OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
    while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
    SYSKEY = 0x0;  /* Lock OSCCON */

    /* Set up output pins */
    AD1PCFG = 0xFFFF;
    ODCE = 0x0;
    TRISECLR = 0xFF;
    PORTE = 0x0;

    /* Output pins for display signals */
    PORTF = 0xFFFF;
    PORTG = (1 << 9);
    ODCF = 0x0;
    ODCG = 0x0;
    TRISFCLR = 0x70;
    TRISGCLR = 0x200;

    /* Set up input pins */
    TRISDSET = (1 << 8);
    TRISFSET = (1 << 1);

    /* Set up SPI as master */
    SPI2CON = 0;
    SPI2BRG = 4;
    /* SPI2STAT bit SPIROV = 0; */
    SPI2STATCLR = 0x40;
    /* SPI2CON bit CKP = 1; */
    SPI2CONSET = 0x40;
    /* SPI2CON bit MSTEN = 1; */
    SPI2CONSET = 0x20;
    /* SPI2CON bit ON = 1; */
    SPI2CONSET = 0x8000;


    TRISD &= 0x00000fe0;        //set port D as input
    TRISFSET = 0x02;            //set port F as input

    TRISECLR = 0xFF;            //set port E as output (for LED)


    //initialize timer 2
    PR2 = 15625;            //Set the period register to 80.000.000/256/10 = 31250 (timeout every 100ms)
    T2CON = 0x8070;         //set bit 15 to 1; start timer 2. Prescale to 256:1

    return;
}
