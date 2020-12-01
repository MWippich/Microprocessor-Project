//
// Created by Max Wippich on 2019-12-05.
//

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include <math.h>
#include "gamefuncs.h"

struct GameObject enemies[100];
char numEnemies = 0;

struct GameObject projectiles[40];
char numProjs = 0;

/**
 * Adds the passed enemy to the game at the last empty space of the enemies array and increments the numEnemies counter.
 */
void addEnemy(struct GameObject enemy){
    if (numEnemies == 100){
        return;
    }
    enemies[numEnemies] = enemy;
    numEnemies++;
}

/**
 * Adds the passed projectile to the game at the last empty space of the projectiles array and increments the numProjs counter.
 */
void addProj(struct GameObject proj){
    if(numProjs == 40){
        return;
    }
    projectiles[numProjs] = proj;
    numProjs++;
}

/**
 * Removes the enemy at the current index by shifting all enemies after it in the enemies array one position to the left
 * and decrements the numEnemies counter.
 */
void removeEnemy(int index){
    int i;
    if(enemies[index].boss == 1){
        bossAlive = 0;
    }
    for (i = index + 1; i < numEnemies; i++){
        enemies[i - 1] = enemies[i];
    }
    numEnemies--;
}

/**
 * Removes the projectile at the current index by shifting all enemies after it in the projectiles array one position to the left
 * and decrements the numProjs counter.
 */
void removeProj(int index){
    int i;
    for (i = index + 1; i < numProjs; i++){
        projectiles[i - 1] = projectiles[i];
    }
    numProjs--;
}


/**
 * Ticks all enemies. Moves them to the left on the screen and removes them if they move outside the screen.
 * Also draws the enemies to the canvas.
 */
void updateObjects(){
    int i;
    for(i = 0; i < numEnemies; i++){
        struct GameObject enemy = enemies[i];
        enemies[i].x -= enemies[i].speed;             //move horizontally
        if(enemies[i].y + enemies[i].height + enemies[i].verticalSpeed >= 36){    //determine if object should move up or down
          enemies[i].verticalSpeed *= -1;
        } else if (enemies[i].y + enemies[i].verticalSpeed <= -4 ){
          enemies[i].verticalSpeed *= -1;
        }
        enemies[i].y += enemies[i].verticalSpeed; //move vertically
        if(enemy.x + enemy.width <= 0) {
            removeEnemy(i);
            i--;
        } else {
            addToCanvas(&enemy);
        }
    }
    for(i = 0; i < numProjs; i++){
        struct GameObject proj = projectiles[i];
        projectiles[i].x += projectiles[i].speed;
        projectiles[i].y += projectiles[i].verticalSpeed;
        if(proj.x > 128 || proj.y + proj.height >= 32 || proj.y <= 0) {
            removeProj(i);
            i--;
        } else {
            addToCanvas(&proj);
        }
    }
}

//Checks if two gameObjects's rectangles intersect. Returns 1 if true and 0 otherwise
char rectIntersect(struct GameObject *obj1, struct GameObject *obj2){
    return ! ( obj1->x > obj2->x + obj2->width
               || obj1->x + obj1->width < obj2->x
               || obj1->y > obj2->y + obj2->height
               || obj1->y + obj1->height < obj2->y
    );
}

/**
 * Checks if the player intersects with an enemy and removes the enemey and decrements player health if that is the case
 */
void checkCollisions(){
    int i;
    for(i = 0; i < numEnemies; i++){
        struct GameObject enemy = enemies[i];

        int j;
        for(j = 0; j < numProjs; j++){
            struct GameObject proj = projectiles[j];
            if(rectIntersect(&enemy, &proj)) {
                score += 50*(bossesKilled + 1);      // Extra points for hitting an enemy
                enemies[i].health -= proj.damage;
                removeProj(j);
                j--;// Decrement to avoid bug where one projectile's tick is skipped
                if(enemies[i].health <= 0){
                    removeEnemy(i);
                    i--; // Decrement to avoid bug where one enemy's tick is skipped
                    continue;
                }
            }
        }
    }
    for(i = 0; i < numEnemies; i++){
        if(rectIntersect(&ship, &enemies[i])){
            playerHit(enemies[i].damage);
            removeEnemy(i);
        }
    }
}

int getNumEnemies(){
    return numEnemies;
}

struct GameObject *getEnemy(int index){
    return &enemies[index];
}

void resetObjects(){
    numEnemies = 0;
    numProjs = 0;
}
