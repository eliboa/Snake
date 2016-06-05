#ifndef SNAKE_H
#define SNAKE_H
#include "dynamic_libs/os_functions.h"
#include "draw.h"
#include "dynamic_libs/vpad_functions.h"

typedef struct SnakeI SnakeI;
struct SnakeI {
	// Coordinates
    int x;
    int y;
    int length;
    char direction;
    // Colors
    int r;
    int g;
    int b;

    int end;

    SnakeI *next;
};

typedef struct Snake Snake;
struct Snake {
	int frame_x;
	int frame_y;
	int x;
	int y;
	int w;	
	int length;
	char direction;
	int loose;
    int pause;
	int score;
    int start;
	int food_state;
	int food_x;
	int food_y;
    int seed;
	int debug;

	SnakeI *first;
};

void initSnake(Snake* s);
void displayStart(Snake* s);
void displaySnake(Snake* s);
void moveSnake(Snake* s, char direction);
int isLooseMoove(Snake* s);
void setNewFoodCoord(Snake* s);
void triggerSnake(Snake* s, VPADData* vpad);
void stretchSnake(Snake* s, int length);
int randU(int range_max, Snake* s);


#endif /* SNAKE_H */
