#ifndef SNAKE_H
#define SNAKE_H
#include "../../../../../../../devkitPro/libwiiu/src/coreinit.h"
#include "../../../../../../../devkitPro/libwiiu/src/vpad.h"
#include "../../../../../../../devkitPro/libwiiu/src/draw.h"


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

    // Pointer to the next part
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
	int score;

	int food_state;
	int food_x;
	int food_y;

	unsigned short lfsr;
	unsigned bit;

	// pointer to the tail 
	SnakeI *first;
};

void initSnake(Snake* s);
void displaySnake(Snake* s);
void moveSnake(Snake* s, char direction);
int isLooseMoove(Snake* s);
void triggerSnake(Snake* s, VPADData* vpad);
void stretchSnake(Snake* s, int length);
unsigned randU(int range_max, Snake* s);

#endif /* SNAKE_H */