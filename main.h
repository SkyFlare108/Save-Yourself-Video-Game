#ifndef MAIN_H
#define MAIN_H

#include "gba.h"

typedef struct astronaut {
    int x;
    int y;
    int width;
    int height;
    int dx;
    int dy;
    int prevX;
    int prevY;
    
}Astronaut;

typedef struct heart {
    int x;
    int y;
    int collideState;
}Heart;

int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
void displayAnimatedText(char* text, int *x, int *y, int *dy);
void initAstronaut(Astronaut *astronautObject);
void initHearts(Heart hearts[]);
void drawHearts(Heart hearts[]);
void showTimer(int time);
void update(Astronaut *astronautObject, u32 currentButtons);

#endif
