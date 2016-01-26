#ifndef LOADER_H
#define LOADER_H
#include "../../../devkitPro/libwiiu/src/coreinit.h"
#include "../../../devkitPro/libwiiu/src/vpad.h"
#include "../../../devkitPro/libwiiu/src/types.h"
#include "../../../devkitPro/libwiiu/src/draw.h"
#include "../../../devkitPro/libwiiu/src/socket.h"
#include "../../../devkitPro/libwiiu/src/string.h"
#define LOG_ROW_WIDTH 59
#define LOG_MAX_ROWS 11

void _getIN();

void printU(int line, char messageU[]);
typedef struct SnakeI SnakeI;
struct SnakeI {
    int x;
    int y;
    SnakeI *next;
};

typedef struct Snake Snake;
struct Snake {
	int frame_x;
	int frame_y;
	int x ;
	int y;
	int w;	
	int length;
	char direction;
	SnakeI *first;
};

#endif /* LOADER_H */