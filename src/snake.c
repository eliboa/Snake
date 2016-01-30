#include "snake.h"

void displaySnake(Snake* s) {

	// First draw the frame
	drawRect(1, 2, s->frame_x -1, s->frame_y -1, 255, 0, 0, 0);
	
	if(s->loose) {

		drawString(0,8,"                                >>> YOU LOOSE <<<\n                           PRESS A TO RESTART");

	} else {
	
		// Snake's head is the current part
		SnakeI *current = s->first;

		// For each part
		while( current != NULL ) {

			// Display the part
			int x1 = current->x;
			int y1 = current->y;
			int x2 = current->x + s->w;
			int y2 = current->y + s->w;

			if(current->length > 1) {
				if(current->direction == 'L') { x2 = x2 + (s->w * (current->length - 1)); }
				if(current->direction == 'R') { x1 = x1 - (s->w * (current->length - 1)); }		
				if(current->direction == 'D') { y1 = y1 - (s->w * (current->length - 1)); }
				if(current->direction == 'U') { y2 = y2 + (s->w * (current->length - 1)); }
			}
			drawFillRect(x1, y1, x2, y2, current->r, current->g, current->b, 0);

			// Get to the next part
			current = current->next;
		}

		// Now we want some food for the snake to hunt
		switch(s->food_state) {
			
			case 0: 
			// Get some food
				// Loop until some coordinates are found 
				while(1) {
					// Get some random coordinates 			
					int rand_x = randU(s->frame_x, s);
					int rand_y = randU(s->frame_y, s);	
					// Check they aren't in the snake ones
					if(!isOnSnake(s, 1, rand_x, rand_y)) {
						s->food_x = rand_x;	
						s->food_y = rand_y;
						break;
					}
				}
				// Switch food state
				s->food_state = 1;
				break;
			case 1:
				// Check if the food has been eaten
				if(isOnSnake(s, 2, s->food_x, s->food_y)) {
					stretchSnake(s, 1);
					s->score = s->score + 100;
					s->food_state = 0;
				}
				// Display the food
				drawFillRect(s->food_x, s->food_y, s->food_x - s->w, s->food_y - s->w, 23, 10, 191, 0);
				break;
		}
	}
}

void moveSnake(Snake* s, char direction) {

	// Memory allocation function
	unsigned int coreinit_handle;
	OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);	
	void* (*OSAllocFromSystem)(uint32_t size, int align);
	void*(*memset)(void *dest, uint32_t value, uint32_t bytes);
	void (*OSFreeToSystem)(void *ptr);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSAllocFromSystem", &OSAllocFromSystem);		
	OSDynLoad_FindExport(coreinit_handle, 0, "OSFreeToSystem", &OSFreeToSystem);
	OSDynLoad_FindExport(coreinit_handle, 0, "memset", &memset);

	// Get the head
	SnakeI *head = s->first;

	// Save the head coordinates
	int prev_x = head->x;
	int prev_y = head->y;

	// Move the snake's head inside the frame

	if(direction == 'U') { // Up
		if     (head->y == 0 && head->x < (s->frame_x - s->w) ) { direction = 'R'; head->x = head->x + s->w; }
		else if(head->y == 0 && head->x == (s->frame_x - s->w)) { direction = 'D'; head->y = head->y + s->w; } 
		else if(head->y  > 0 && s->direction != 'D') head->y = head->y - s->w;
	}
	else if(direction == 'D') {// Down
		if 	   (head->y == (s->frame_y - s->w) && head->x >= 0) { direction = 'L'; head->x = head->x - s->w; }
		else if(head->y == (s->frame_y - s->w) && head->x == 0) { direction = 'U'; head->y = head->y - s->w; }
		else if(head->y  < (s->frame_y - s->w) && s->direction != 'U') head->y = head->y + s->w;
	}
	else if(direction == 'L') {// Left
		if     (head->x == 0 && head->y > 0) { direction = 'U'; head->y = head->y - s->w; }
		else if(head->x == 0 && head->y == 0) { direction = 'R'; head->y = head->y - s->w; }
		else if(head->x  > 0 && s->direction != 'R') head->x = head->x - s->w;		
	}	
	else if(direction == 'R') {// Right
		if 		(head->x == (s->frame_x - s->w) &&  head->y < (s->frame_y - s->w)) { direction = 'D'; head->y = head->y + s->w; }	
		else if (head->x == (s->frame_x - s->w) &&  head->y == (s->frame_y - s->w)) { direction = 'L'; head->x = head->x - s->w;	 }
		else if (head->x  < (s->frame_x - s->w) && s->direction != 'L') head->x = head->x + s->w;
	}

	// If the snake's head position has changed
	if(prev_x != head->x || prev_y != head->y) {

		// Save the direction
		head->direction = direction;

		// If there's a tail to rebuild
		if (head->next != NULL) {

			// Switch to the next part
			SnakeI *current = head->next;

			// If direction has changed
			if(head->direction != s->direction) {

				// Set a new part just behind the head
				//
				// Allocate memory and set coordinates
				SnakeI *si1 = OSAllocFromSystem(0x40, 64);
				// Coordinates
				si1->x = prev_x;
				si1->y = prev_y;
				// Color
				si1->r = 255; si1->g = 255; si1->b = 255;
				// Parameters
				si1->direction = head->direction;
				si1->length = 1;
				si1->end = 0;			


				// Set the created part as the head next pointer	
				head->next = si1;
				// Set the switched part as the created part next pointer
				si1->next = current;

				// If this is the last part, we decrement the length and it's done
				if(current->end) current->length--;

			} else {
				// If direction hasn't changed

				// Change the coordinates
				current->x = prev_x;
				current->y = prev_y;
				if(!current->end) current->length++;

			}

			// If the tail continues
			if(!current->end) {		

				SnakeI *before_last;

				// Now, we have to get to the last part of the tail
				while(current->end == 0) { 
					// Save the before last part
					before_last = current; 
					current = current->next; 
				}

				// If the last part is too smal to be reduced
				if(current->length == 1) {
					// Then we have to delete it
					OSFreeToSystem(current);
					// And set the before last part as the last one
					before_last->next = NULL;
					before_last->end = 1;
				} else {
					// Otherwise we simply have to reduce the last part length
					current->length--;
				}
			}

		}
		// Save the direction
		s->direction = direction;	
		// Check to see if it's a loose move
		if(isOnSnake(s, 0, head->x, head->y)) s->loose = 1;
	}
}

int isOnSnake(Snake* s, int head, int x, int y) {

	// int head : 0 --> Check the tail (skip the head)
	//            1 --> Check the entire snake
	//            2 --> Only check the head

	// Get to the snake head
	SnakeI *current  = s->first;

	// If we want to test the head
	if(head>0) {
		if(current->x == x - s->w && current->y == y - s->w) return 1;
		else if(head==2) return 0;
	}

	// If we want to skip the head
	if(head==0) current = current->next; 

	// Check if coordinates are on the snake
	while(current != NULL) {
		if(current->x == x) {
			if(current->direction == 'U' && current->y <= y && y <= (current->y + s->w * current->length)) return 1;
			if(current->direction == 'D' && current->y >= y && y >= (current->y - s->w * current->length)) return 1;
		}
		if(current->y == y) {
			if(current->direction == 'L' && current->x <= x && x <= (current->x + s->w * current->length)) return 1;
			if(current->direction == 'R' && current->x >= x && x >= (current->x - s->w * current->length)) return 1;	
		}
		current = current->next; 
	}
}

void triggerSnake(Snake* s, VPADData* vpad) {

	moveSnake(s, s->direction);

	if (vpad->btn_hold & BUTTON_UP) moveSnake(s, 'U');
	if (vpad->btn_hold & BUTTON_DOWN) moveSnake(s, 'D');
	if (vpad->btn_hold & BUTTON_LEFT) moveSnake(s, 'L');
	if (vpad->btn_hold & BUTTON_RIGHT) moveSnake(s, 'R');

	float x = vpad->lstick.x;
	float y = vpad->lstick.y;
	float abs_x = x;
	float abs_y = y;

	// Calculate absolute values
	if(x < 0) abs_x = -(x);
	if(y < 0) abs_y = -(y);

    // Calculate direction from Left stick
	// If abs(x) == abs(y), arbitrarily designate the direction on the abscissa
	if( abs_x >= abs_y ) {
		 if(x > 0)  moveSnake(s, 'R');
		 if(x < 0)  moveSnake(s, 'L');
	}
	if( abs_y > abs_x) {
		if(y > 0)  moveSnake(s, 'U');
		if(y < 0)  moveSnake(s, 'D');
	}
}


void initSnake(Snake* s) {

	// Memory allocation function
	unsigned int coreinit_handle;
	OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);	
	void* (*OSAllocFromSystem)(uint32_t size, int align);
	void*(*memset)(void *dest, uint32_t value, uint32_t bytes);
	void (*OSFreeToSystem)(void *ptr);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSAllocFromSystem", &OSAllocFromSystem);		
	OSDynLoad_FindExport(coreinit_handle, 0, "OSFreeToSystem", &OSFreeToSystem);
	OSDynLoad_FindExport(coreinit_handle, 0, "memset", &memset);

	s->loose = 0;
	
	// Create the snake head
	SnakeI *si = OSAllocFromSystem(0x40, 64);;
	// Coordinates
	si->x = s->x;
	si->y = s->y;
	si->length = 1;
	si->direction = s->direction;
	// Color
	si->r = 255; si->g = 0; si->b = 0;
	si->next = NULL;
	si->end = 0;
	
	// Save the head pointer
	s->first = si;

	// Create the tail
	// Calculate new part coordinates		
	int i, new_x, new_y;
	if(s->direction == 'L') { new_x = s->x + s->w; 	new_y = s->y; };
	if(s->direction == 'R') { new_x = s->x - s->w; 	new_y = s->y; };		
	if(s->direction == 'D') { new_x = s->x; 		new_y = s->y - s->w; };
	if(s->direction == 'U') { new_x = s->x; 		new_y = s->y + s->w; };

	// Allocate memory and set coordinates
	SnakeI *si1 = OSAllocFromSystem(0x40, 64);
	// Coordinates
	si1->x = new_x;
	si1->y = new_y;
	// Color
	si1->r = 255; si1->g = 255; si1->b = 255;
	// Parameters
	si1->direction = s->direction;
	si1->length = s->length - si->length;
	si1->next = NULL;
	si1->end = 1;

	si->next = si1;
}


void stretchSnake(Snake* s, int length) {
	// Snake's head is the current part
	SnakeI *current = s->first;

	while(current->end == 0) { current = current->next; }
	current->length = current->length + length;
}


unsigned randU(int range_max, Snake* s)
{
    while(1) {
        s->bit  = ((s->lfsr >> 0) ^ (s->lfsr >> 2) ^ (s->lfsr >> 3) ^ (s->lfsr >> 5) ) & 1;
        s->lfsr =  (s->lfsr >> 1) | (s->bit << 15);
        if(s->lfsr < range_max) {
        	int a = (s->lfsr / s->w);
        	s->lfsr = a * s->w;
        	break;
        }
    }
    return s->lfsr;
}