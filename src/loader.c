/*
|	-OOOO-O----OOO---OO----OO---OO-OOOOOOO-
|	OOOOO-OO---OO----OO----OO---OO-OOOOOOOO
|	O-----OO---OO----OO----OO---OO-OO-----O
|	O-----OOO--OO---OOOO---OO--OO--OO-----O
|	O-----OOO--OO---O--O---OO-OOO--OO------
|	OOO---OOOO-OO---O--O---OOOOO---OOOOO---
|	-OOO--OO-O-OO--OO--OO--OOOO----OOOOO---
|	--OOO-OO-OOOO--OO--OO--OOOOO---OOOOO---
|	---OO-OO--OOO--OOOOOO--OO-OOO--OO------
|	---OO-OO--OOO-OOOOOOOO-OO--OO--OO------
|	---OO-OO---OO-OO----OO-OO---OO-OO-----O
|	OOOO--OO---OO-OO----OO-OO---OO-OOOOOOOO
|	OOO--OOO----O-OO----OO-OO---OO-OOOOOOO
|	
|	For Wii U
|	v1.0
|	eliboa@gmail.com
|
*/

#include "loader.h"
#include "snake.h"

void _start()
{
	/****************************>            Fix Stack            <****************************/
	//Load a good stack
	asm(
		"lis %r1, 0x1ab5 ;"
		"ori %r1, %r1, 0xd138 ;"
		);
	/****************************>           Get Handles           <****************************/
	// Get a handle to coreinit.rpl
	uint32_t coreinit_handle;
	OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);
	/****************************>       External Prototypes       <****************************/
	//OSScreen functions
	void(*OSScreenInit)();
	unsigned int(*OSScreenGetBufferSizeEx)(unsigned int bufferNum);
	unsigned int(*OSScreenSetBufferEx)(unsigned int bufferNum, void * addr);
	//OS Memory functions
	void*(*memset)(void * dest, uint32_t value, uint32_t bytes);
	void(*OSFreeToSystem)(void *ptr);
	void* (*OSAllocFromSystem)(uint32_t size, int align);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSAllocFromSystem", &OSAllocFromSystem);		
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
	// Memory	
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
	_getIN();
}
void _getIN()
{
	// Get a handle to coreinit.rpl
	uint32_t coreinit_handle;
	OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);	
	//Get a handle to vpad.rpl */
	unsigned int vpad_handle;
	OSDynLoad_Acquire("vpad.rpl", &vpad_handle);	
	//VPAD functions
	int(*VPADRead)(int controller, VPADData *buffer, unsigned int num, int *error);
	//OS functions
	void(*_Exit)();		
	//VPAD functions
	OSDynLoad_FindExport(vpad_handle, 0, "VPADRead", &VPADRead);
	//OS functions
	OSDynLoad_FindExport(coreinit_handle, 0, "_Exit", &_Exit);	
	// Init VPAD
	VPADData vpad_data;
	/****************************>             VPAD LOOP             <****************************/
	/* Enter the VPAD loop */
	int error;
	//Read initial vpad status
	VPADRead(0, &vpad_data, 1, &error);

	//Init Snake
	Snake s;                    
	s.frame_x = 840;
	s.frame_y = 480;
	s.x = 500;
	s.y = 300;
	s.w = 20;
	s.length = 6;
	s.direction = 'R';
	s.score=0;
	s.start=0;
	s.first = NULL;
	s.food_state = 0;
	s.food_x = 0;
	s.food_y = 0;
	s.seed = 1;
	s.pause = 0;
	s.debug = 0;

	initSnake(&s);

	while(1)
	{
		// Read the data from gamepad
		VPADRead(0, &vpad_data, 1, &error);
		// Init gamepad screen
		//fillScreen(255,0,0,0);
		fillScreen(0,0,0,0);

		if(s.start) {
			triggerSnake(&s, &vpad_data);
			displaySnake(&s);
		} else {
			displayStart(&s);
		}

		if(!s.start && (vpad_data.btn_hold || vpad_data.btn_trigger)) s.start=1;

		if (vpad_data.btn_trigger & BUTTON_A) {
			if(s.loose) {
				s.length=6;
				s.score=0;
				s.food_state = 0;
				initSnake(&s);
			}
		}	
		if ((vpad_data.btn_trigger & BUTTON_B) && s.start) {
			if(s.pause) s.pause = 0;
			else s.pause = 1;
		}
		/*
		if (vpad_data.btn_trigger & BUTTON_Y) {
			if(s.debug) s.debug = 0;
			else s.debug = 1;
		}		
		*/
		// Clear buffer
		flipBuffers();
		// Exit when Home button is pressed
		if(vpad_data.btn_hold & BUTTON_HOME) break;
	}

	// WARNING: DO NOT CHANGE THIS. YOU MUST CLEAR THE FRAMEBUFFERS AND 
	//  IMMEDIATELY CALL EXIT FROM THIS FUNCTION. RETURNING TO LOADER CAUSES FREEZE.
	int ii=0;
	for(ii;ii<2;ii++)
	{
		fillScreen(0,0,0,0);
		flipBuffers();
	}
	_Exit();
}


