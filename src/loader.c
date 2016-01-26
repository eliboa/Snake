#include "loader.h"


void _start()
{
	/****************************>            Fix Stack            <****************************/
	//Load a good stack
	asm(
		"lis %r1, 0x1ab5 ;"
		"ori %r1, %r1, 0xd138 ;"
		);
	/****************************>       External Prototypes       <****************************/
	uint32_t coreinit_handle;
	OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);
	/* Memory allocation functions */
	void* (*OSAllocFromSystem)(uint32_t size, int align);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSAllocFromSystem", &OSAllocFromSystem);	
	//OSScreen functions
	void(*OSScreenInit)();
	unsigned int(*OSScreenGetBufferSizeEx)(unsigned int bufferNum);
	unsigned int(*OSScreenSetBufferEx)(unsigned int bufferNum, void * addr);
	//OS Memory functions
	void*(*memset)(void * dest, uint32_t value, uint32_t bytes);
	void(*OSFreeToSystem)(void *ptr);
	//IM functions
	int(*IM_Open)();
	int(*IM_Close)(int fd);
	int(*IM_SetDeviceState)(int fd, void *mem, int state, int a, int b);
	/****************************>             Exports             <****************************/
	//OSScreen functions
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenInit", &OSScreenInit);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenGetBufferSizeEx", &OSScreenGetBufferSizeEx);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenSetBufferEx", &OSScreenSetBufferEx);
	//OS Memory functions
	OSDynLoad_FindExport(coreinit_handle, 0, "memset", &memset);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSAllocFromSystem", &OSAllocFromSystem);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSFreeToSystem", &OSFreeToSystem);
	//IM functions
	OSDynLoad_FindExport(coreinit_handle, 0, "IM_Open", &IM_Open);
	OSDynLoad_FindExport(coreinit_handle, 0, "IM_Close", &IM_Close);
	OSDynLoad_FindExport(coreinit_handle, 0, "IM_SetDeviceState", &IM_SetDeviceState);
	/****************************>          Initial Setup          <****************************/
	//Restart system to get lib access
	int fd = IM_Open();
	void *mem = OSAllocFromSystem(0x100, 64);
	memset(mem, 0, 0x100);
	//set restart flag to force quit browser
	IM_SetDeviceState(fd, mem, 3, 0, 0); 
	IM_Close(fd);
	OSFreeToSystem(mem);
	//wait a bit for browser end
	unsigned int t1 = 0x1FFFFFFF;
	while(t1--) ;
	//Call the Screen initilzation function.
	OSScreenInit();
	//Grab the buffer size for each screen (TV and gamepad)
	int buf0_size = OSScreenGetBufferSizeEx(0);
	int buf1_size = OSScreenGetBufferSizeEx(1);
	//Set the buffer area.
	OSScreenSetBufferEx(0, (void *)0xF4000000);
	OSScreenSetBufferEx(1, (void *)0xF4000000 + buf0_size);
	//Clear both framebuffers.
	int ii = 0;
	for (ii; ii < 2; ii++)
	{
		fillScreen(0,0,0,0);
		flipBuffers();
	}
	//Jump to entry point.
	//_entryPoint();
	_getIN();
}

void _getIN()
{
	/****************************>           Get Handles           <****************************/
	//Get a handle to coreinit.rpl
	unsigned int coreinit_handle;
	OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);
	//Get a handle to vpad.rpl */
	unsigned int vpad_handle;
	OSDynLoad_Acquire("vpad.rpl", &vpad_handle);
	/****************************>       External Prototypes       <****************************/
	//VPAD functions
	int(*VPADRead)(int controller, VPADData *buffer, unsigned int num, int *error);
	//OS functions
	void(*_Exit)();
	/****************************>             Exports             <****************************/
	//VPAD functions
	OSDynLoad_FindExport(vpad_handle, 0, "VPADRead", &VPADRead);
	//OS functions
	OSDynLoad_FindExport(coreinit_handle, 0, "_Exit", &_Exit);
	/****************************>             Var Inits             <****************************/
	VPADData vpad_data;
	/****************************>             VPAD LOOP             <****************************/
	/* Enter the VPAD loop */
	int error;
	//Read initial vpad status
	VPADRead(0, &vpad_data, 1, &error);


	// Définition du snake 28 * 16
	Snake s;
	SnakeI si;
	s.frame_x = 840;
	s.frame_y = 480;
	s.x = 0;
	s.y = 0;
	s.w = 20;
	s.length = 4;
	s.direction = 'R';

	si.x = s.x;
	si.y = s.y;
	si.next = NULL;
	s.first = &si;


	char logMessage[64];

	while(1)
	{
		// Read the data from gamepad
		VPADRead(0, &vpad_data, 1, &error);
		// Init gamepad screen
		//fillScreen(255,0,0,0);
		fillScreen(0,0,0,0);


		printU(1, "eliboa! Snake");
		__os_snprintf(logMessage, 100, "Snake x : %d    y : %d    w : %d", s.x, s.y, s.w);
		printU(3, logMessage);

		// On affiche le snake
		triggerSnake(&s, &vpad_data);
		displaySnake(&s);


		if (vpad_data.btn_trigger & BUTTON_A) {
			s.x = 700;
			s.y = 30;
			s.w = 30;			
		}

		// Clear buffer
		flipBuffers();
		// Exit when Home button is pressed
		if(vpad_data.btn_hold & BUTTON_HOME) break;
	}

	//WARNING: DO NOT CHANGE THIS. YOU MUST CLEAR THE FRAMEBUFFERS AND IMMEDIATELY CALL EXIT FROM THIS FUNCTION. RETURNING TO LOADER CAUSES FREEZE.
	int ii=0;
	for(ii;ii<2;ii++)
	{
		fillScreen(0,0,0,0);
		flipBuffers();
	}
	_Exit();
}

void printU(int line, char messageU[])
{
	// Display text in <message[]> on gamepad at line <line>
	drawString(0,line,messageU);	
}

displaySnake(Snake* s) {
	int x2 = (*s).x + (*s).w;
	int y2 = (*s).y + (*s).w;

	//(*s).first = 
	// Draw the snake head
	drawFillRect((*s).x, (*s).y, x2, y2, 255, 255, 255, 0);

	// Draw the tail
	int i;
	int prev_x = (*s).x;
	int prev_y = (*s).y;
	for( i=1; i<(*s).length; i++) {
		int new_x, new_y;
		if((*s).direction == 'D') { new_x = prev_x; new_y = prev_y - (*s).w; };
		if((*s).direction == 'R') { new_x = prev_x - (*s).w; new_y = prev_y; };
		if((*s).direction == 'U') { new_x = prev_x; new_y = prev_y + (*s).w; };
		if((*s).direction == 'L') { new_x = prev_x + (*s).w; new_y = prev_y; };

		drawFillRect(new_x, new_y, new_x + (*s).w, new_y + (*s).w, 255, 255, 255, 0);	

		prev_x = new_x;
		prev_y = new_y;
	}
}

moveSnake(Snake* s, char direction) {

	// Change direction
	(*s).direction = direction;

	// Move the snake inside the frame
	switch(direction) {

		case 'U': // Up
			if( (*s).y > 0 ) 
				(*s).y = (*s).y - (*s).w;
			break;
		case 'D': // Down
			if( (*s).y < ((*s).frame_y - (*s).w) ) 
				(*s).y = (*s).y + (*s).w;
			break;
		case 'L': // Left
			if( (*s).x > 0 )
				(*s).x = (*s).x - (*s).w;		
			break;	
		case 'R': // Right
			if( (*s).x < ((*s).frame_x - (*s).w) )
			(*s).x = (*s).x + (*s).w;
			break;
	}
}

triggerSnake(Snake* s, VPADData* vpad) {
	if ((*vpad).btn_hold & BUTTON_UP) moveSnake(s, 'U');
	if ((*vpad).btn_hold & BUTTON_DOWN) moveSnake(s, 'D');
	if ((*vpad).btn_hold & BUTTON_LEFT) moveSnake(s, 'L');
	if ((*vpad).btn_hold & BUTTON_RIGHT) moveSnake(s, 'R');

	float x = (*vpad).lstick.x;
	float y = (*vpad).lstick.y;
	float abs_x = x;
	float abs_y = y;

	// Calcul de la valeur absolue de x et y
	if(x < 0) abs_x = -(x);
	if(y < 0) abs_y = -(y);

    // Calcul de la direction 
	// Si abs(x) == abs(y), on désigne arbitrairement la direction sur l'abscisse
	if( abs_x >= abs_y ) {
		 if(x > 0)  moveSnake(s, 'R');
		 if(x < 0)  moveSnake(s, 'L');
	}
	if( abs_y > abs_x) {
		if(y > 0)  moveSnake(s, 'U');
		if(y < 0)  moveSnake(s, 'D');
	}
}