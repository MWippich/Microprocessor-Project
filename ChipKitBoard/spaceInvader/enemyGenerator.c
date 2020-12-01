/* enemyGenerator.c

  Randomly generate a new enemy

  Created 2019 by Emmy Yin and Max Wippich
  Updated 2019-12-06

*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "gamefuncs.h"
#include <stdlib.h>

int alienRate = 3;
int asteroidRate = 4;
int smallAstRate = 4;
float alienProbability = 0.2;
float asteroidProbability = 0.7;
float smallAstProbability = 0.5;

void generateAlien(){
  if(getRandomFloat(0, 100) <= alienProbability){
    int x = 128, y = getRandomInt(0, 22);
    float speed = getRandomFloat(0.5 + 0.05 * bossesKilled, 1.2 + 0.1 * bossesKilled), verticalSpeed = getRandomFloat(0.1, 0.5 + 0.05*bossesKilled);
    struct GameObject alien = {x, y, 10, 10, alienImg, speed, verticalSpeed, 5 + 0.2*bossesKilled, 4};
    addEnemy(alien);
  }
}

void generateSmallAsteroid(){
  if(getRandomFloat(0, 100) <= smallAstProbability) {
    int x = 128, y = getRandomInt(-2, 28);
    createSmallAsteroid(x, y);
  }
}

void generateAsteroid(){
  if(getRandomFloat(0, 100) <= asteroidProbability) {
    int x = 128, y = getRandomInt(-4, 28);
    float speed = getRandomFloat(0.5 + 0.1 * bossesKilled, 1.4 + 0.3 * bossesKilled);
    createAsteroid(x, y, speed);
  }
}

void createUFO(int x, int y){
    addEnemy((struct GameObject) {x, y, 11, 8, UFOImg, getRandomFloat(0.5, 1.5), getRandomFloat(0.4, 1), 2, 2});
}

void createDoge(int x, int y){
    addEnemy((struct GameObject) {x, y, 12, 14, smallDogeImg, getRandomFloat(1.5, 2.5 + bossesKilled*0.05), 0, 5, 2});
}

void createAsteroid(int x, int y, float speed){
    struct GameObject asteroid = {x, y, 8, 8, asteroidImg, speed, 0, 4, 2};
    addEnemy(asteroid);
}

void createSmallAsteroid(int x, int y){
    float speed = getRandomFloat(0.6 + 0.1 * bossesKilled, 1.6 + 0.3 * bossesKilled);
    struct GameObject asteroid = {x, y, 6, 6, smallAsteroidImg, speed, 0, 1, 1};
    addEnemy(asteroid);
}

void createEnemy(){
    int i;
    //generate alien
    for(i = 0; i < alienRate; i++){
      generateAlien();
    }

    //generate small asteroid
    for(i = 0; i < smallAstRate; i++){
      generateSmallAsteroid();
    }

    //generate asteroid
    for(i = 0; i < asteroidRate; i++){
      generateAsteroid();
    }
}


float getRandomFloat(float min, float max){
  return (((float)rand()/(float)RAND_MAX) * (max - min)) + min;
}

int getRandomInt(int min, int max){
  return (rand() % (max - min + 1)) + min;
}

void increaseDiff(){
    alienProbability += 0.04;
    asteroidProbability += 0.03;
    smallAstProbability += 0.03;

    if (bossesKilled == 3) {
        alienRate += 1;
        smallAstRate--;
    }
    if (bossesKilled == 7) {
        alienRate += 3;
        asteroidRate--;
        smallAstRate--;
    }

}