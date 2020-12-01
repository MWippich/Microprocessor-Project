//
// Created by Max Wippich on 2019-12-05.
//
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "gamefuncs.h"  /* Declatations for these labs */

int ticksSinceFire = 0;
int ticksSinceStyleChange = 0;
int firingRateStyle1 = 8;
int firingRateStyle2 = 3;
int firingRateStyle3 = 10;

char firingStyle = 0;
char numFiringStyles = 3;

//Get the current status of the push-buttons BTN1, BTN2, BTN3 and BTN4
int getbtns(){
    unsigned int bits = PORTD;
    bits = bits >> 4;
    bits &= (0x0000000f & (PORTF >> 1)); //Read from button 1
    return bits;
}

void moveShipUp(){
    if (ship.y == 0){
        //Ship at edge
        return;
    }
    else{
        ship.y -= ship.verticalSpeed;
    }
}

void moveShipDown(){
    if (ship.y + ship.height >= 32){
        //Ship at edge
        return;
    }
    else{
        ship.y += ship.verticalSpeed;
    }
}

void fire() {
    if (firingStyle == 0) {
        if (ticksSinceFire > firingRateStyle1) {
            struct GameObject proj = {ship.x + ship.width, ship.y + ship.height / 2 - 2, 4, 4, bigShotImg, 5, 0, 0, 3};
            addProj(proj);
            ticksSinceFire = 0;
        }
    } else if (firingStyle == 1) {
        if (ticksSinceFire > firingRateStyle2) {
            struct GameObject proj = {ship.x + ship.width, ship.y + ship.height / 2 - 1, 2, 2, shotImg, 6, 0, 0, 1};
            addProj(proj);
            ticksSinceFire = 0;
        }
    } else {
        if (ticksSinceFire > firingRateStyle3) {
            struct GameObject proj1 = {ship.x + ship.width, ship.y + ship.height / 2 - 1, 2, 2, shotImg, 6, 0.7, 0, 2};
            addProj(proj1);
            struct GameObject proj2 = {ship.x + ship.width, ship.y + ship.height / 2 - 1, 2, 2, shotImg, 6, -0.7, 0, 2};
            addProj(proj2);
            ticksSinceFire = 0;
        }
    }
}

void checkBtns(){

    int buttonBits = getbtns();
    if(buttonBits & 0x01){
        //BTN 1
        if(ticksSinceStyleChange > 12) {
            firingStyle--;
            if (firingStyle < 0) {
                firingStyle = numFiringStyles - 1;
            }
            ticksSinceStyleChange = 0;
        }
    } else if(buttonBits & 0x02){
        //BTN 2
        if(ticksSinceStyleChange > 12) {
            firingStyle++;
            firingStyle %= numFiringStyles;
            ticksSinceStyleChange = 0;
        }
    }
    ticksSinceStyleChange++;
    fire();
    if(buttonBits & 0x04){
        //BTN 3
        moveShipDown();
    }
    if(buttonBits & 0x08){
        //BTN 4
        moveShipUp();
    }
    ticksSinceFire++;
}

int btn1Pressed(){
    int buttonBits = getbtns();
    return buttonBits & 0x01;
}

int btn2Pressed(){
    int buttonBits = getbtns();
    return buttonBits & 0x02;
}
