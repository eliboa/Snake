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

	int food_state;
	int food_x;
	int food_y;

    int seed;

	int debug;

	SnakeI *first;
};
typedef signed long long s64;
typedef unsigned int     u32;
typedef s64 OSTime;
typedef u32 OSTick;
typedef struct OSCalendarTime
{
    int sec;    // seconds after the minute [0, 61]
    int min;    // minutes after the hour [0, 59]
    int hour;   // hours since midnight [0, 23]
    int mday;   // day of the month [1, 31]
    int mon;    // month since January [0, 11]
    int year;   // years since 0000
    int wday;   // days since Sunday [0, 6]
    int yday;   // days since January 1 [0, 365]

    int msec;   // milliseconds after the second [0,999]
    int usec;   // microseconds after the millisecond [0,999]
} OSCalendarTime;



void initSnake(Snake* s);
void displaySnake(Snake* s);
void moveSnake(Snake* s, char direction);
int isLooseMoove(Snake* s);
void setNewFoodCoord(Snake* s);
void triggerSnake(Snake* s, VPADData* vpad);
void stretchSnake(Snake* s, int length);
int randU(int range_max, Snake* s);
OSCalendarTime* getDate();


#endif /* SNAKE_H */
