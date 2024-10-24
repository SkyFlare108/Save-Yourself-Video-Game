#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"

#include "images/space.h"
#include "images/titlekg.h"
#include "images/astronaut.h"
#include "images/win.h"
#include "images/lose.h"
#include "images/heart.h"

enum gba_state {
  START,
  DIRECTIONS,
  PLAY,
  WIN,
  LOSE,
};


int currentTime = 0;

int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
  if(x2 >= x1 && x2 + w2 <= x1 + w1 && y2 >= y1 && y2 + h2 <= y1 + h1) {
    return 1;
  } else {
    return 0;
  }
}

void displayAnimatedText(char* text, int *x, int *y, int *dy){
    undrawImageDMA(*x, *y, 6 * 20, 8, titlebkg);
      *y += *dy;
      if(*y < 35 || *y > 80) {
        *dy = -(*dy);
      }
    drawString(*x, *y,text, WHITE);
}

void initAstronaut(Astronaut *astronautObject) {
    astronautObject->x = 0;
    astronautObject->y = 0;
    astronautObject->width = 50;
    astronautObject->height = 37;
    astronautObject->dx = 0;
    astronautObject->dy = 0;
}

void initHearts(Heart hearts[]) {
  for(int i = 0; i < 12; i++) {
    hearts[i].x = randint(5, WIDTH - 30);
    hearts[i].y = randint(40, HEIGHT - 20);
    hearts[i].collideState = 0;
  }
}

void drawHearts(Heart hearts[]){
    for(int i = 0; i < 12; i++) {
    drawImageDMA(hearts[i].y,hearts[i].x, 25, 18, heart);
  }
}

void showTimer(int time) {
  int seconds = (time / 60) + 1;
  char timerT[10];
  sprintf(timerT, "Time: %d", seconds);
  currentTime = seconds;
  drawRectDMA(HEIGHT - 10, WIDTH - 65, 65,10, BLACK);
  drawString(HEIGHT - 8, WIDTH - 57, timerT, WHITE);
}

void update(Astronaut *astronautObject, u32 currentButtons) {
  if(KEY_DOWN(BUTTON_LEFT, currentButtons)) {
    astronautObject->x += -1;
  }
  
  if(KEY_DOWN(BUTTON_RIGHT, currentButtons)) {
    astronautObject->x += 1;
  }

  if(KEY_DOWN(BUTTON_DOWN, currentButtons)) {
    astronautObject->y += 1;
  }
  
  if(KEY_DOWN(BUTTON_UP, currentButtons)) {
    astronautObject->y += -1;
  }

  // astronautObject->x += astronautObject->dx;
  // astronautObject->y += astronautObject->dy;

  if(astronautObject->x < 0) {
    astronautObject->x = 0;
  }
  if(astronautObject-> x > (WIDTH - 50)) {
    astronautObject-> x = WIDTH - 50;
  }

  if(astronautObject-> y > (HEIGHT - 37)) {
    astronautObject-> y = HEIGHT - 37;
  }
    if(astronautObject->y < 0) {
    astronautObject->y = 0;
  }

}

int main(void) {
  int x = 140;
  int y = 50;
  int dy = 1;
  Astronaut astronautObject;
  Heart hearts[12];
  int counter = 12;
  int timer = 600;
  srand(0);

  drawFullScreenImageDMA(titlebkg);
  // Manipulate REG_DISPCNT here to set Mode 3.
  REG_DISPCNT = MODE3 | BG2_ENABLE;

  // Save current and previous state of button input.
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  // Load initial application state
  enum gba_state state = START;

  while (1) {
    currentButtons = BUTTONS; // Load the current state of the buttons
    
    // Manipulate the state machine below as needed //
    waitForVBlank();

    switch (state) {
      case START:  
        counter = 12;
        timer = 600;
        initAstronaut(&astronautObject);

        drawString(35,5,"Welcome to Surviving Space", WHITE);
        waitForVBlank();
        //drawRectDMA(135, 0, 240, 16,BLACK);
        displayAnimatedText("Press Enter to begin", &x, &y, &dy);
        //drawString(100,50,"Press Enter to begin", WHITE);

        if(KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) 
        { 
            state = DIRECTIONS;   
            drawFullScreenImageDMA(titlebkg);
            drawRectDMA(80, 0, 240, 30, BLACK);
        }
        break;

      case DIRECTIONS:
          drawString(35,5,"Directions:", WHITE);
          drawString(80,5,"Your oxygen supply ran out!!", WHITE);
          drawString(100, 5, "Get the hearts in 10 sec to survive.", WHITE);

          waitForVBlank();
          //drawRectDMA(135, 0, 240, 16,BLACK);
          //displayAnimatedText("Press Enter to Start Game!", &x, &y, &dy);
          drawString(140,40,"...Press Enter When Ready", WHITE);

        if(KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons)) 
        { 
            state = PLAY;
            drawFullScreenImageDMA(space);
            initHearts(hearts);
            drawHearts(hearts);
        }
        if(KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) 
        {
            currentTime = 0;
            state = START;   
            drawFullScreenImageDMA(titlebkg);  
        }
        break;
      case PLAY:
        update(&astronautObject, currentButtons);
        waitForVBlank();
        undrawImageDMA(astronautObject.prevY, astronautObject.prevX, astronautObject.width, astronautObject.height, space);
        drawImageDMA(astronautObject.y, astronautObject.x, astronautObject.width, astronautObject.height, astronaut);
        astronautObject.prevX = astronautObject.x;
        astronautObject.prevY = astronautObject.y;

        //collision -  astronaut bumps into heart, heart disappears
        for(int i = 0; i < 12; i++) {
            int result = collision(astronautObject.x, astronautObject.y, astronautObject.width, astronautObject.height, hearts[i].x, hearts[i].y, 25, 18);
            if(result == 1 && hearts[i].collideState == 0) {
             // if((astronautObject.x == (hearts[i].x - 25) / 2 || astronautObject.x == (hearts[i].x + 25) / 2) && (astronautObject.y == (hearts[i].y - 18) / 2 || astronautObject.y == (hearts[i].y + 18) / 2)) {
                  hearts[i].collideState = 1;
                  undrawImageDMA(hearts[i].x, hearts[i].y, 25, 18, space);
                  counter--;
                  if(counter <= 0) {
                    state = WIN;
                    drawFullScreenImageDMA(win);
                    drawRectDMA(0,120,240,35,BLACK);
                  }
              //}
            }
            else if(hearts[i].collideState == 0) {
                drawImageDMA(hearts[i].y,hearts[i].x, 25, 18, heart);
            }
        }
        
        if(state == WIN) {
          break;
        } else if(state == PLAY) {
          drawImageDMA(astronautObject.y, astronautObject.x, astronautObject.width, astronautObject.height, astronaut);
        }
        showTimer(timer);
        timer--;

        if(timer <= 0) 
        { 
            state = LOSE;
            drawFullScreenImageDMA(lose);
            drawRectDMA(0,120,240,35,BLACK);
        }
        
        // drawImageDMA(astronautObject.y, astronautObject.x, astronautObject.width, astronautObject.height, astronaut);

        if(KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) 
        {
            currentTime = 0;
            state = START;   
            drawFullScreenImageDMA(titlebkg);  
        }
        break;
      case WIN:
        // waitForVBlank();
        // drawFullScreenImageDMA(win);
        // // drawRectDMA(0,120,240,35,BLACK);
        //undrawImageDMA(HEIGHT - 10, WIDTH - 65, 65,10, win);
        drawString(20,50,"Press Select to play again", WHITE);
        char winArr[30];
        sprintf(winArr, "You WON by %d seconds!!", currentTime);
        drawCenteredString(10, 120, 0, 0, winArr, WHITE);
        drawString(20,50,"Press Select to play again", WHITE);
        // drawString(20,50,"Press Select to play again", WHITE);
        // drawCenteredString(10, 120, 0, 0, "YOU WIN!!", WHITE);
        // drawString(20,50,"Press Select to play again", WHITE);
        if(KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) 
        {
            currentTime = 0;
            state = START;   
            drawFullScreenImageDMA(titlebkg);  
        }
        break;
      case LOSE:
        //undrawImageDMA(HEIGHT - 10, WIDTH - 65, 65,10, lose);
        // waitForVBlank();
        drawCenteredString(10, 120, 0, 0, "YOU LOST:(", WHITE);
        drawString(20,50,"Press Select to play again", WHITE);
        if(KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) 
        {
            currentTime = 0;
            state = START;   
            drawFullScreenImageDMA(titlebkg);  
        }
        break;
    }

    previousButtons = currentButtons; // Store the current state of the buttons
  }

  UNUSED(previousButtons);

  return 0;
}
