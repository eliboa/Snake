#include "snake.h"
#define M 2147483647
#define A 16807
#define Q ( M / A )
#define R ( M % A )

void displaySnake(Snake* s) {

	// First draw the frame
	drawRect(1, 2, s->frame_x -1, s->frame_y -1, 255, 255, 255, 0);

	if(s->pause) drawString(0,15,"                                     PAUSE");

	// DEBUG MODE
	if(s->debug) {

		char logline[120]; int i;

		__os_snprintf(logline, 120, "FOOD   - x = %d  y = %d  state = %d", s->food_x, s->food_y, s->food_state);
		drawString(0,19,logline);
		SnakeI *buff = s->first;
		while( buff != NULL ) {
			i++;
			__os_snprintf(logline, 120, "PART %d - x = %d  y = %d  length = %d   direction = %c", i, buff->x, buff->y, buff->length, buff->direction);
			drawString(0,i+19,logline);
			buff = buff->next;
		}
	}
	
	if(s->loose) {

		drawString(0,8,"                                >>> GAME OVER <<<\n                           PRESS A TO RESTART");

	} else {

		// Get some food if not already
		if(!s->food_state) setNewFoodCoord(s);

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

		// Now we want to display the food for the snake to hunt
		if(s->food_state) drawFillCircle(s->food_x + (s->w /2), s->food_y + (s->w /2), s->w / 2, 23, 10, 191, 0);
	}
}

void setNewFoodCoord(Snake* s) {
	int rand_x, rand_y, i=100;
	while(i--) {
		// Get some food coordinates
		rand_x = randU(s->frame_x, s);
		rand_y = randU(s->frame_y, s);	
		// Check they aren't in the snake ones
		if(!isOnSnake(s, 1, rand_x, rand_y)) {
			s->food_x = rand_x;	
			s->food_y = rand_y;
			s->food_state = 1;
			break;
		}	
	}
}

void moveSnake(Snake* s, char direction) {

	// Memory allocation function
	unsigned int coreinit_handle;
	OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);	
	uint32_t *memalign, *freemem;
	OSDynLoad_FindExport(coreinit_handle, 1, "MEMAllocFromDefaultHeapEx", &memalign);
	OSDynLoad_FindExport(coreinit_handle, 1, "MEMFreeToDefaultHeap", &freemem);
	void* (*MEMAllocFromDefaultHeapEx)(uint32_t size, int align) = (void* (*)(uint32_t,int))*memalign;
	void (*MEMFreeToDefaultHeap)(void *ptr) = (void (*)(void*))*freemem;

	// Get the head
	SnakeI *head = s->first;

	// Save the head coordinates
	int prev_x = head->x;
	int prev_y = head->y;

	SnakeI *tail = head->next;
	
	// Check for forbidden moves

	// About-turn forbidden
	if((direction=='U' && tail->y == (head->y - s->w)) 
	 ||(direction=='D' && tail->y == (head->y + s->w)) 
	 ||(direction=='R' && tail->x == (head->x + s->w)) 
	 ||(direction=='L' && tail->x == (head->x - s->w))) {
		direction = tail->direction;
	}

	// The head touches the left wall
	if(head->x == 0) {
		// Top left : Up and Left forbidden
		if(head->y == 0 && (direction == 'U' || direction == 'L')) {
			// Get a new direction
			if(tail->direction == 'U') direction = 'R'; else direction = 'D';
		}
		// Bottom left : Down and Left forbidden
		else if(head->y == (s->frame_y - s->w) && (direction == 'D' || direction == 'L')) {
			// Get a new direction
			if(tail->direction == 'D') direction = 'R'; else direction = 'U';	
		}
		// Left forbidden
		else if(direction == 'L') {
			// Get a new direction
			if(tail->direction == 'D') direction = 'D'; else direction = 'U';
		}
	}
	// The head touches the right wall
	else if(head->x == (s->frame_x - s->w)) {
		// Top right: Up and Right forbidden
		if(head->y == 0 && (direction == 'U' || direction == 'R')) {
			// Get a new direction
			if(tail->direction == 'U') direction = 'L'; else direction = 'D';
		}
		// Bottom right : Down and Right forbidden
		else if(head->y == (s->frame_y - s->w) && (direction == 'D' || direction == 'R')) {
			// Get a new direction
			if(tail->direction == 'D') direction = 'L'; else direction = 'U';
		}
		// Right forbidden
		else if(direction == 'R') {
			// Get a new direction
			if(tail->direction == 'U') direction = 'U'; else  direction = 'D';
		}	
	}
	// The head touches the top wall : Up forbidden
	else if(head->y == 0 && direction == 'U') {
		if(tail->direction == 'L') direction = 'L';
		else direction = 'R';
	}
	// The head touches the bottom wall : Down forbidden
	else if(head->y == (s->frame_y - s->w) && direction == 'D') {
		if(tail->direction == 'R') direction = 'R';
		else direction = 'L';
	}	

	// Move the snake's head
	switch(direction) {
		case 'U': 
			head->y = head->y - s->w;
			break;
		case 'D':
			head->y = head->y + s->w;
			break;
		case 'L':
			head->x = head->x - s->w;	
			break;
		case 'R':
			head->x = head->x + s->w;
			break;
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
				SnakeI *si1 = MEMAllocFromDefaultHeapEx(0x40, 0x20);
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
					MEMFreeToDefaultHeap(current);
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

		// Check if the food has been eaten
		if(isOnSnake(s, 2, s->food_x, s->food_y)) {
			// Then stretch the snake (+1)
			stretchSnake(s, 1);
			// Increment the score
			s->score = s->score + 100;
			// Switch food taste
			s->food_state = 0;
		}

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
		if(current->x == x && current->y == y) return 1;
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

	if(!s->pause)
	{
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

	    if( abs_x > 0 || abs_y > 0)
	  	{
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

		if(!s->debug) moveSnake(s, s->direction);
	}
}


void initSnake(Snake* s) {

	// Memory allocation function
	unsigned int coreinit_handle;
	OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);	
	uint32_t *memalign;
	OSDynLoad_FindExport(coreinit_handle, 1, "MEMAllocFromDefaultHeapEx", &memalign);
	void* (*MEMAllocFromDefaultHeapEx)(uint32_t size, int align) = (void* (*)(uint32_t,int))*memalign;


	s->loose = 0;
	
	// Create the snake head
	SnakeI *si = MEMAllocFromDefaultHeapEx(0x40, 0x20);
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
	SnakeI *si1 = MEMAllocFromDefaultHeapEx(0x40, 0x20);
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

	// Now get the first food
	setNewFoodCoord(s);
}


void stretchSnake(Snake* s, int length) {
	// Snake's head is the current part
	SnakeI *current = s->first;

	while(current->end == 0) { current = current->next; }
	current->length = current->length + length;
}

int randU(int range_max, Snake* s)
{
    s->seed = A * (s->seed % Q) - R * (s->seed / Q);
    if (s->seed <= 0) s->seed += M;
    s->seed = s->seed % (range_max-1) + 1;
    return (s->seed / s->w) * s->w;
}
