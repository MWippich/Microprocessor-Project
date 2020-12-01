/* gameObjects.c

  This file contains all the objects in the game
  such as the player and the enemies.

  Created 2019 by Emmy Yin and Max Wippich
  Updated 2019-12-06

*/

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "gamefuncs.h"  /* Declatations for these labs */

const int const shipImg[8] = {0x66, 0xFF, 0xDB, 0x66, 0x7E, 0x3C, 0x3C, 0x18};
const int const astImg[8] = {0x3C, 0x4A, 0x51, 0x8B, 0xA5, 0xCA, 0x62, 0x1C};
const int const smallAstImg[6] = {0x0E, 0x1F, 0x3F, 0x3F, 0x3E, 0x06};
const int const aliImg[10] = {0xC1, 0x27D, 0x2F7, 0x1F7, 0x1FE, 0xFE, 0x1EE, 0x3F7, 0x27D, 0x1E1};
const int const ufoImg[11] = {0x70, 0xD8, 0xAE, 0xD9, 0xAD, 0xDD, 0xAD, 0xD9, 0xAE, 0xD8, 0x70};
const int const bigUfoImg[16] = {0x380, 0x440, 0x820, 0x920, 0x870, 0x128C, 0x1202, 0x1071, 0x1071, 0x1202, 0x128C, 0x870, 0x920, 0x820, 0x440, 0x380};
const int const smlDoge[12] = {0x3c0, 0xff8, 0x3cfc, 0x2c49, 0x3b6b, 0x1bdc, 0x3bfc, 0x2bce, 0x1fe9, 0x15db, 0xafc, 0x150};
const uint8_t const doge[] = {
        255, 255, 255, 255, 255, 255, 127, 187, 68, 95, 170, 93, 163, 215, 175, 95,175, 95, 175, 95, 223, 111, 175, 247, 59, 237, 242, 254, 171, 254, 1, 255,
        255, 255, 15, 211, 109, 58, 253, 8, 178, 77, 58, 199, 122, 197, 242, 173, 242, 237, 186, 215, 40, 215, 41, 214, 35, 175, 91, 212, 63, 234, 149, 111,
        171, 84, 253, 252, 254, 253, 126, 184, 195, 52, 201, 22, 225, 27, 196, 19, 165, 74, 36, 146, 72, 162, 85, 8, 226, 25, 166, 80, 167, 216, 167, 88,
        106, 149, 161, 95, 135, 91, 175, 87, 142, 123, 134, 127, 134, 121, 134, 121, 132, 59, 192, 27, 164, 74, 177, 70,184, 69, 186, 69, 254, 80, 175, 217,
};
int bgDoge[32] = {};
const int const strImg[3] = {0x2, 0x5, 0x2};
const int const shtImg[2] = {0x3, 0x3};
const int const bigShtImg[4] = {0x6, 0xF, 0xF, 0x6};
const int const *asteroidImg;
const int const *starImg;
const int const *smallAsteroidImg;
const int const *alienImg;
const int const *UFOImg;
const int const *bigUFOImg;
const int const *smallDogeImg;
const int const *bigDogeImg;
const int const *shotImg;
const int const *bigShotImg;
struct GameObject ship;

const int const bossUFOIm[] = {
  0x1F80000, 0x2040000, 0x4e23fc0, 0x4E26070, 0x78E18198, 0xC801038E, 0x88010702, 0xC8E17FC3,
  0x78E1FFE1, 0x8E1FFF1, 0x7801C7F1, 0xC801C3F9, 0x88E1E7F9, 0xC8E1FFF9, 0x78E1FE39, 0x801FE39,
  0x7801FF39, 0xC8E1C7F9, 0x88E1C7F9, 0xC8E1FFF1, 0x7801FFF1, 0x801FFE1, 0x8E1FFC1, 0x78E1FF83,
  0xC8E11C02, 0x88011E02, 0xC801060E, 0x78E18018, 0x4E26070, 0x4e23fc0, 0x2040000, 0x1F80000
};
const int const *bossUFOImg;

void resetPlayer(){
    ship = (struct GameObject) {4, 12, 8, 8, shipImg, 0, 1, 8};
}

void convertDoge(){
    int i, j;
    for(i = 0; i < 4; i++) {

        for(j = 0; j < 32; j++) {
            unsigned int data = ~doge[i * 32 + j];
            data = (data << 24) >> 24;
            bgDoge[j] |= data << i*8;
        }
    }
    bigDogeImg = bgDoge;
}

void initGameObjects(){
    resetPlayer();

    asteroidImg = astImg;
    starImg = strImg;
    smallAsteroidImg = smallAstImg;
    shotImg = shtImg;
    bigShotImg = bigShtImg;
    bossUFOImg = bossUFOIm;
    alienImg = aliImg;
    UFOImg = ufoImg;
    bigUFOImg = bigUfoImg;
    smallDogeImg = smlDoge;
    convertDoge();
}

