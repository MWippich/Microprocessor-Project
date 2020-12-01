/* gamefuncs.h
   Header file for the functions.

   Created 2019 by Emmy Yin and Max Wippich
   Updated 2019-12-06
 */

// The Image struct definition.


struct GameObject {
    float x, y;
    char width, height;
    const int const *img;
    float speed;
    float verticalSpeed;
    int health;
    char damage;
    char boss;
};

// Declare gameObjects
extern struct GameObject ship;
extern const int const *asteroidImg;
extern const int const *starImg;
extern const int const *smallAsteroidImg;
extern const int const *alienImg;
extern const int const *UFOImg;
extern const int const *bigUFOImg;
extern const int const *smallDogeImg;
extern const int const *bigDogeImg;
extern const int const *shotImg;
extern const int const *bigShotImg;
extern const int const *bossUFOImg;
extern char meetingBoss;
extern char bossAlive;
extern int score;
extern int bossesKilled;
extern int nextLevel;
extern int level;
extern int ticks;


/* Declare display-related functions from graphics.c */
void display_image(int x, const uint8_t *data);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
void drawString(int x, int y, char *s);
void invertCanvas();
void drawCanvas();
void addToCanvas(struct GameObject *obj);
void switchStars();
void setStars();
uint8_t spi_send_recv(uint8_t data);

//game related declarations
extern char playerAlive;
void startGame();
void resetPlayer();
void setSeed(int seed);

void checkCollisions();
void updateObjects();
void addEnemy(struct GameObject enemy);
void addProj(struct GameObject proj);
int getNumEnemies();
void resetObjects();
void createAsteroid(int x, int y, float speed);
void createSmallAsteroid(int x, int y);
void createUFO(int x, int y);
void createDoge(int x, int y);
void writeMessage(char *msg);
struct GameObject *getEnemy(int index);
void increaseDiff();

//Memory
void NVMWriteWord(void* address, unsigned int data);
void NVMErasePage(void* address);

//randomizers
float getRandomFloat(float min, float max);
int getRandomInt(int min, int max);

/* Declare related functions from game.c */
char * itoaconv( int num );

/* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug( volatile int * const addr );

/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];
/* Declare bitmap array containing icon */
extern const uint8_t const icon[128];
/* Declare text buffer for display output */
extern char textbuffer[4][16];


// Player control definitions
void fire();
void checkButtons();
int btn1Pressed();
int btn2Pressed();
