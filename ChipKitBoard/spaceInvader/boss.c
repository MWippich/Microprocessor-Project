/* boss.c

  Meeting the boss before leveling up

  Created 2019 by Emmy Yin and Max Wippich
  Updated 2019-12-06

*/
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "gamefuncs.h"  /* Declatations for these labs */

int level = 0;
char meetingBoss = 0;
int nextLevel = 600;
int bossesKilled = 0;

char bossAlive = 0;
int currentBoss = 0;

void createBoss(){
    currentBoss = getRandomInt(0, 3);
    if(currentBoss == 0) {
        //UFO Swarm boss
        bossAlive = 1;
        struct GameObject bigUFO = {151, 0, 32, 32, bossUFOImg, 0.075, 0, 35 + 2 * bossesKilled, 8, 1};
        addEnemy(bigUFO);
        int i;
        for(i = 0; i < 10; i++) {
            createUFO(140, getRandomInt(2, 22));
        }
    } else if(currentBoss == 1){
        //Doge boss
        bossAlive = 1;
        struct GameObject newBoss = {152, 0, 32, 32, bigDogeImg, 0.075, 0, 55 + 2 * bossesKilled, 8, 1};
        addEnemy(newBoss);
    } else if(currentBoss == 2){
        //Asteroid boss
        int i;
        for(i = 0; i < 30 + bossesKilled*3; i++){
            createAsteroid(getRandomInt(140, 180 + ((int) bossesKilled*4.2)), getRandomInt(-4, 28), getRandomFloat(0.2, 0.7));
        }
    } else if(currentBoss == 3){
        //Single UFO boss
        bossAlive = 1;
        struct GameObject newBoss = {152, 10, 16, 13, bigUFOImg, 0.2, 0.8, 70 + 5 * bossesKilled, 8, 1};
        addEnemy(newBoss);
    }
}

void checkBoss(){
  if(getNumEnemies() <= 0){
    meetingBoss = 0;
    bossesKilled++;
    score += 1000*bossesKilled;
    increaseDiff();
    writeMessage("//Next Sector//");
    switchStars();
    return;
  }

  if(currentBoss == 0 && bossAlive){
      //UFO Swarm Boss
      int modifier = bossesKilled * (1.0/3);
      if(modifier > 4){
          modifier = 4;
      }
      if(getRandomInt(0,9 - modifier) == 0)
          createUFO(140, getRandomInt(2, 22));
  } else if(currentBoss == 1 && bossAlive){
        //Doge Boss
      int modifier = bossesKilled * 0.5;
      if(modifier > 3){
          modifier = 3;
      }
      if(getRandomInt(0, 17 - modifier) == 0)
          createDoge(140, getRandomInt(-4, 22));
  } else if(currentBoss == 3 && bossAlive) {
      //Single UFO boss
      if (ticks % 15 == 0) {
          int i;
          for (i = 0; i < getNumEnemies(); ++i) {
              struct GameObject *enem = getEnemy(i);
              if (enem->boss == 1) {
                  struct GameObject proj = {enem->x, enem->y + enem->height/2, 2, 2, shotImg, 4, 0, 1, 1};
                  addEnemy(proj);
                  if(ticks % 30 == 0){
                      //change movement
                      if(enem->x < 80 && enem->x < 115){
                          float speedDir;
                          if(getRandomInt(0, 1) == 0){
                              speedDir = 1;
                          } else {
                              speedDir = -1;
                          }
                          enem->speed = getRandomFloat(0.15, 0.4) * speedDir;
                      }
                      if(enem->y > 0 && enem->y + enem->height < 32 && getRandomInt(0, 1) == 0){
                          enem->verticalSpeed *= -1;
                      }
                      float vertSpeedDir;
                      if(enem->verticalSpeed >= 0){
                          vertSpeedDir = 1;
                      } else {
                          vertSpeedDir = -1;
                      }
                      enem->verticalSpeed = getRandomFloat(0.4, 1.2) * vertSpeedDir;
                  }
              }
          }
      }
  }
}

void meetBoss(){
    writeMessage("//Boss Incoming//");
    meetingBoss = 1;
    createBoss();
}

int levelUp(){
  level++;
  if (level >= nextLevel){
    meetBoss();
    level = 0;
  }
}
