/* graphics.c

  This file contains the graphics of
	the objects in the game
  such as the player and the enemies.

  Created 2019 by Emmy Yin and Max Wippich
  Updated 2019-12-06

	Some of the functions were originally created
	2015 by Axel Isaksson and F Lundevall

*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "gamefuncs.h"  /* General game declarations */

/* Declare a helper function which is local to this file */
static void num32asc( char * s, int );

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)


unsigned int canvas[128];
struct GameObject stars[40];
char starChangeTimer = 0;
char starsSwitch = 1; //boolean to know which half of stars array to fill upon level up


/* Obtained from Lab 3
	 quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}


/* display_debug

	 Obtained from Lab 3

   A function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug( volatile int * const addr )
{
  display_string( 1, "Addr" );
  display_string( 2, "Data" );
  num32asc( &textbuffer[1][6], (int) addr );
  num32asc( &textbuffer[2][6], *addr );
  display_update();
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

//Obtained from Lab 3
void display_init(void) {
  DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);

	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);

	spi_send_recv(0x8D);
	spi_send_recv(0x14);

	spi_send_recv(0xD9);
	spi_send_recv(0xF1);

	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);

	spi_send_recv(0xA1);
	spi_send_recv(0xC8);

	spi_send_recv(0xDA);
	spi_send_recv(0x20);

	spi_send_recv(0xAF);
}

//Obtained from Lab 3
void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;

	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

void drawString(int x, int y, char *s) {
	int i;
	if(x < 0 || x >= 128 || y < 0 || y > 32)
		return;
	if(!s)
		return;

	char string[25];
	for(i = 0; i < 25; i++) {
        if (*s) {
            string[i] = *s;
            s++;
        } else
            string[i] = ' ';
    }
	int j;
    for(j = 0; j < 25; j++) {
        char c = string[j];
        if(c & 0x80) {
            continue;
        } else {
            int i;
            for (i = 0; i < 8; i++){
                if(x + j*6 + i >= 128 ) continue;
                canvas[x + j*6 + i] |= (unsigned int) font[c*8+i] <<  y;
            }
        }
    }
}

void addToCanvas(struct GameObject *obj) {
    int x1;
    int i = 0;
    int w = obj->width;
    int x = obj->x;
    if(x + w < 0) return;
    for(x1 = x; x1 < x + w && x1 < 128 ; x1++, i++){
        unsigned int col = 0;
        col |= obj->img[i];
        int shamt = obj->y;
        if(shamt >= 0) {
            canvas[x1] |= col << shamt;
        } else {
            canvas[x1] |= col >> -shamt;
        }
    }
}

void invertCanvas(){
    int i;
    for (i = 0; i < 132; i++){
        canvas[i] = ~canvas[i];
    }
}

void drawCanvas() {
	int i, j;

	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(0x10);

		DISPLAY_CHANGE_TO_DATA_MODE;

		for(j = 0; j < 128; j++) {
			uint8_t u = (canvas[j] >> (8 * (i))) & 0xFF;
			spi_send_recv(u);

		}
	}
}

void drawStars(){
    if(starChangeTimer >= 0){
        starChangeTimer--;
        int i;
        for(i = 0; i < 40; i++){
            stars[i].x -= 2;

        }
    }

    int i;
    for(i = 0; i < 40; i++){
        addToCanvas(&stars[i]);
    }
}

void setStars(){
    int i;
    for(i = 0; i < 20; i++){
        stars[i] = (struct GameObject) {getRandomInt(0, 125), getRandomInt(0, 29), 3, 3, starImg, 0, 0};
    }
}

void switchStars(){
    starChangeTimer = 64;
    int i = 0;
    int max = 20;
    if(starsSwitch){
        //Fill other half of array
        i = 21;
        max = 40;
        starsSwitch = 0;
    } else {
        starsSwitch = 1;
    }
    for(; i < max; i++){
        stars[i] = (struct GameObject) {getRandomInt(128, 128 + 125), getRandomInt(0, 29), 3, 3, starImg, 0, 0};
    }
    /*
    addToCanvas(&(struct GameObject) {10, 52, 3, 3, starImg, 0, 0});
    addToCanvas(&(struct GameObject) {16, 12, 3, 3, starImg, 0, 0});
    addToCanvas(&(struct GameObject) {32, 16, 3, 3, starImg, 0, 0});
    addToCanvas(&(struct GameObject) {64, 12, 3, 3, starImg, 0, 0});
    addToCanvas(&(struct GameObject) {72, 32, 3, 3, starImg, 0, 0});
    addToCanvas(&(struct GameObject) {60, 60, 3, 3, starImg, 0, 0});
    addToCanvas(&(struct GameObject) {60, 60, 3, 3, starImg, 0, 0});
    addToCanvas(&(struct GameObject) {96, 16, 3, 3, starImg, 0, 0});
    addToCanvas(&(struct GameObject) {112, 0, 3, 3, starImg, 0, 0});
    addToCanvas(&(struct GameObject) {100, 40, 3, 3, starImg, 0, 0});
    addToCanvas(&(struct GameObject) {120, 52, 3, 3, starImg, 0, 0});
    addToCanvas(&(struct GameObject) {12, 48, 3, 3, starImg, 0, 0});
     */
}

void clearCanvas(){
    int i;
    for (i = 0; i < 128; i++){
        canvas[i] = 0;
    }
    drawStars();
}

// Obtained from Lab 3
void display_update(void) {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);

		spi_send_recv(0x0);
		spi_send_recv(0x10);

		DISPLAY_CHANGE_TO_DATA_MODE;

		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;

			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}

/* Helper function, local to this file.
	 Obtained from Lab 3
   Converts a number to hexadecimal ASCII digits. */
static void num32asc( char * s, int n )
{
  int i;
  for( i = 28; i >= 0; i -= 4 )
    *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}



/*
 * Obtained from Lab 3
 * itoa
 *
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 *
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 * The integer is considered signed, and a minus-sign
 * precedes the string of digits if the number is
 * negative.
 *
 * This routine will return a varying number of digits, from
 * one digit (for integers in the range 0 through 9) and up to
 * 10 digits and a leading minus-sign (for the largest negative
 * 32-bit integers).
 *
 * If the integer has the special value
 * 100000...0 (that's 31 zeros), the number cannot be
 * negated. We check for this, and treat this as a special case.
 * If the integer has any other value, the sign is saved separately.
 *
 * If the integer is negative, it is then converted to
 * its positive counterpart. We then use the positive
 * absolute value for conversion.
 *
 * Conversion produces the least-significant digits first,
 * which is the reverse of the order in which we wish to
 * print the digits. We therefore store all digits in a buffer,
 * in ASCII form.
 *
 * To avoid a separate step for reversing the contents of the buffer,
 * the buffer is initialized with an end-of-string marker at the
 * very end of the buffer. The digits produced by conversion are then
 * stored right-to-left in the buffer: starting with the position
 * immediately before the end-of-string marker and proceeding towards
 * the beginning of the buffer.
 *
 * For this to work, the buffer size must of course be big enough
 * to hold the decimal representation of the largest possible integer,
 * and the minus sign, and the trailing end-of-string marker.
 * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
 * 64-bit quantities; however, the size of an int on your current compiler
 * may not allow this straight away.
 */
#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";

  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i + 1 ] );
}
