/* game.c

  The game is started and finished here.

  Created 2019 by Emmy Yin and Max Wippich
  Updated 2019-12-08

*/
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "gamefuncs.h"  /* Declatations for these labs */

//To work around errors
void *stdin, *stdout, *stderr;

char playerAlive = 1; // Boolean value for if game is over or not.
int score = 0;
int ticks = 0;

int ticksSinceScreenWrite = 0;
char screenText[20];

void gameLoop();

void highScoreMenu(){
    volatile int* ptr = (volatile int*) 0xBD000000;
    int highScore = *ptr;
    display_string(0, itoaconv(highScore));

    ptr = (volatile int*) 0xBD000004;
    highScore = *ptr;
    display_string(1, itoaconv(highScore));

    ptr = (volatile int*) 0xBD000008;
    highScore = *ptr;
    display_string(2, itoaconv(highScore));


    display_string(3, "Return() - BTN1");
    display_update();
    while(!btn1Pressed()){ }
    while(btn1Pressed()){ }
    startGame();
}

void instructions(){
    display_string(0, "BTN4/BTN3:UP/DWN");
    display_string(1, "BTN2/BTN1:");
    display_string(2, "CYCLE WEAP0N");
    display_string(3, "Continue()- BTN1");
    display_update();
    while(!btn1Pressed()){}
    while(btn1Pressed()){}
    playerAlive = 1;
    gameLoop();
}

void startGame(){

    display_string(0, "//Welcome//");
    display_string(1, "");
    display_string(2, "BTN1:Start()");
    display_string(3, "BTN2:HighScore()");
    display_update();

    int seed = 0;
    while(!btn1Pressed() && !btn2Pressed()){ seed++; }
    setSeed(seed);
    int nextScreen = 0;
    if(btn2Pressed()){
        nextScreen = 2;
    } else if (btn1Pressed()){
        nextScreen = 1;
    }
    while(btn1Pressed() || btn2Pressed()){ }

    if(nextScreen == 1){
        instructions();
    } else if(nextScreen == 2){
        highScoreMenu();
    } else {
        startGame();
    }

}

void resetGame(){
    resetObjects();
    resetPlayer();
    clearCanvas();
    level = 0;
    meetingBoss = 0;
    bossAlive = 0;
    bossesKilled = 0;
    score = 0;
    ticks = 0;
}

int calcHighScore(){
    volatile int* ptr = (volatile int*) 0xBD000000;
    int highScore = *ptr;
    ptr = (volatile int*) 0xBD000004;
    int highScore2 = *ptr;
    ptr = (volatile int*) 0xBD000008;
    int highScore3 = *ptr;

    if(score > highScore){
        NVMErasePage((void*) 0xBD000000);
        NVMWriteWord((void*) 0xBD000000, score);
        NVMWriteWord((void*) 0xBD000004, highScore);
        NVMWriteWord((void*) 0xBD000008, highScore2);
        return 1;
    } else if(score > highScore2){
        NVMErasePage((void*) 0xBD000000);
        NVMWriteWord((void*) 0xBD000000, (unsigned int) highScore);
        NVMWriteWord((void*) 0xBD000004, (unsigned int) score);
        NVMWriteWord((void*) 0xBD000008, (unsigned int) highScore2);
        return 2;
    } else if(score > highScore3){
        NVMErasePage((void*) 0xBD000000);
        NVMWriteWord((void*) 0xBD000000, (unsigned int) highScore);
        NVMWriteWord((void*) 0xBD000004, (unsigned int) highScore2);
        NVMWriteWord((void*) 0xBD000008, (unsigned int) score);
        return 3;
    }
    return 0;
}

void gameOver(){
    int res = calcHighScore();

    char* s = itoaconv(score);

    char * scoreString;
    if(res == 0){
        scoreString = "[Score]";
    }  else if(res == 1){
        scoreString = "#New High Score#";
    } else if(res == 2){
        scoreString = "#2nd Best Score#";
    } else if(res == 3){
        scoreString = "#3rd Best Score#";
    }

    display_string(0, "//Game Over//");
    display_string(1, scoreString);
    display_string(2, s);
    display_string(3, "Return() - BTN1");

    display_update();
    resetGame();
    while(!btn1Pressed()){ }
    while(btn1Pressed()){ }
    startGame();
}

// Show the nr of lives the player has left using the LEDs
void displayHealth(){
    uint8_t healthLamp = 0xFF;
    PORTE = healthLamp >> 8 - ship.health;
}

// Decrease health if player is hit
void playerHit(int dmg){
    ship.health -= dmg;
    if(ship.health <= 0){
      playerAlive = 0;
    }
    displayHealth();
}

void writeMessage(char *msg){
    int i = 0;
    while(*msg != 0x00 && i < 19){
        screenText[i] = *msg;
        msg++;
        i++;
    }
    screenText[i] = 0x00;
    ticksSinceScreenWrite = 0;
}

/**
 * Main Game Loop.
*/
void gameLoop(){
    displayHealth();
    while(playerAlive){

        while(!(IFS(0) & 0x0100)){ } //Wait for timer timeout every 100ms
        IFS(0) &= 0xfeff; // reset timer timeout flag
        clearCanvas();
        if(ticks % 3 == 0) {
            score += bossesKilled + 1;
        }

        if(!meetingBoss){
          createEnemy();
          levelUp();
        } else {
          checkBoss();
        }

        //Write score
        char* s = itoaconv(score);
        drawString(0, 0, s);

        if(ticksSinceScreenWrite <= 10 || (ticksSinceScreenWrite >= 20 && ticksSinceScreenWrite <= 30) || (ticksSinceScreenWrite >= 40 && ticksSinceScreenWrite <= 50   )) {
            drawString(20, 12, screenText);
        }
        ticksSinceScreenWrite++;

        checkBtns();
        addToCanvas(&ship);
        updateObjects();
        checkCollisions();
        drawCanvas();
        ticks++;
    }

    int blackoutTimer = 20;
    while(blackoutTimer > 0) {
        while (!(IFS(0) & 0x0100)) {} //Wait for timer timeout every 100ms
        IFS(0) &= 0xfeff; // reset timer timeout flag
        if(blackoutTimer % 4 == 0){
            invertCanvas();
        }
        drawCanvas();
        blackoutTimer--;
    }
    gameOver();
}
