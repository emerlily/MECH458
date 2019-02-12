/******************************************************
 MILESTONE : 3
 PROGRAM : 3
 PROJECT : Lab3
 GROUP : 8
 NAME 1 : Emily Wilson, V00769311
 NAME 2 : Stefan Tomanik V00828182
 DESC : This program takes an input from two dip switches, stores their states in a queue, then displays 3 of these stored values on the LED's. 
 DATA
 REVISED
******************************************************/
#include <util/delay_basic.h>
#include <stdlib.h> // the header of the general purpose standard library of C programming language
#include <avr/io.h>// the header of i/o port
#include <avr/interrupt.h> //needed for interrupt functionality
#include "LinkedQueue.h"

void mTimer(int count);

//MAIN ROUTINE
int main(int argc, char *argv[]){
	TCCR1B |=_BV(CS10); //set timer control register 1 to logic 1
	DDRC = 0xff; //set all Port C pins to output
	PORTC = 0b00000011;


	link *head;			/* The ptr to the head of the queue */
	link *tail;			/* The ptr to the tail of the queue */
	link *newLink;		/* A ptr to a link aggregate data type (struct) */
	link *rtnLink;		/* same as the above */


	rtnLink = NULL;
	newLink = NULL;

	setup(&head, &tail);

	DDRA  = 0x00; //set Port A to input
	DDRD = 0x00; //set Port D to input
	DDRC = 0xFF; //set all Port C pins to output
	PORTC = 0b00000000; //initialize all Port C LEDs to off 

	//variables for processing input data 
	int button_count = 0;
	int display_count = 0;
	int shift_bits = 0;
	char display_data = 0b00000000;
	char new_data = 0b00000000;
	char read_input;

	while(1){
		//iterate through 4 button presses 
		while(button_count < 4){
			//wait for button to be pressed and released with delay after each for debouncing
			while((PIND & 0b00000001) == 0b00000001) {};
			mTimer(20); 
			while((PIND & 0b00000001) != 0b00000001) {}; 
			mTimer(20); 
			read_input = PINA & 0b00000011; // read last two bits from Port A
		
			//make a new link in the queue and add the data 
			initLink(&newLink);
			newLink->e.stage = read_input;
			enqueue(&head, &tail, &newLink);
			button_count ++;
			}
			
		//remove and discard first entry 
		dequeue(&head, &tail, &rtnLink);
		free(rtnLink);
		
		//iterate through remaining entries and display on port c
		while(display_count < 4){
			//get first link in the queue 
			dequeue(&head, &tail, &rtnLink);
			new_data = rtnLink->e.stage;
			//shift data to keep old data and display on assigned LEDs
			display_data = ((new_data << shift_bits) | display_data);
			PORTC = display_data;
			free(rtnLink);
			mTimer(2000); //2 second delay as per lab
			display_count++;
			shift_bits = shift_bits + 2;
		}
		//empty the queue when done to avoid memory leak 
		clearQueue(&head, &tail);
			
	} //end while
return (0);
}// end main

void mTimer(int count){
	//reference 16-bit Timer/Counter in manual
	int i; //loop counter
	i = 0;
	TCCR1B |=_BV(WGM12); // set WGM bits to 0100, see pg. 142

	OCR1A = 0x03e8; // set output compare register for 1000 cycles = 1ms
	TCNT1 = 0x0000; //set initial value of timer counter to 0x0000
	TIMSK1 = TIMSK1|0b00000010; //enable output compare interrupt 
	TIFR1 |=_BV(OCF1A); //clear timer interrupt flag and start timer

	// poll timer until reaches 0x03e8
	while(i<count){
		if((TIFR1 & 0x02) == 0x02){
			TIFR1 |=_BV(OCF1A); //clear interrupt flag
			i++; //increment loop counter
		}//end if
	}//end while
return;
}// end mTimer

/**************************************************************************************
* DESC: initializes the linked queue to 'NULL' status
* INPUT: the head and tail pointers by reference
*/

void setup(link **h,link **t){
	*h = NULL;		/* Point the head to NOTHING (NULL) */
	*t = NULL;		/* Point the tail to NOTHING (NULL) */
	return;
}/*setup*/




/**************************************************************************************
* DESC: This initializes a link and returns the pointer to the new link or NULL if error 
* INPUT: the head and tail pointers by reference
*/
void initLink(link **newLink){
	//link *l;
	*newLink = malloc(sizeof(link));
	(*newLink)->next = NULL;
	return;
}/*initLink*/




/****************************************************************************************
*  DESC: Accepts as input a new link by reference, and assigns the head and tail		
*  of the queue accordingly				
*  INPUT: the head and tail pointers, and a pointer to the new link that was created 
*/
/* will put an item at the tail of the queue */
void enqueue(link **h, link **t, link **nL){

	if (*t != NULL){
		/* Not an empty queue */
		(*t)->next = *nL;
		*t = *nL; //(*t)->next;
	}/*if*/
	else{
		/* It's an empty Queue */
		//(*h)->next = *nL;
		//should be this
		*h = *nL;
		*t = *nL;
	}/* else */
	return;
}/*enqueue*/




/**************************************************************************************
* DESC : Removes the link from the head of the list and assigns it to deQueuedLink
* INPUT: The head and tail pointers, and a ptr 'deQueuedLink' 
* 		 which the removed link will be assigned to
*/
/* This will remove the link and element within the link from the head of the queue */
void dequeue(link **h, link **t, link **deQueuedLink){
	/* ENTER YOUR CODE HERE */
	*deQueuedLink = *h;	// Will set to NULL if Head points to NULL
	/* Ensure it is not an empty queue */
	if (*h != NULL){
		*h = (*h)->next;
	}/*if*/
	else{
		*t = NULL;
	}
	
	return;
}/*dequeue*/




/**************************************************************************************
* DESC: Peeks at the first element in the list
* INPUT: The head pointer
* RETURNS: The element contained within the queue
*/
/* This simply allows you to peek at the head element of the queue and returns a NULL pointer if empty */
element firstValue(link **h){
	return((*h)->e);
}/*firstValue*/





/**************************************************************************************
* DESC: deallocates (frees) all the memory consumed by the Queue
* INPUT: the pointers to the head and the tail
*/
/* This clears the queue */
void clearQueue(link **h, link **t){

	link *temp;

	while (*h != NULL){
		temp = *h;
		*h=(*h)->next;
		free(temp);
	}/*while*/
	
	/* Last but not least set the tail to NULL */
	*t = NULL;		

	return;
}/*clearQueue*/





/**************************************************************************************
* DESC: Checks to see whether the queue is empty or not
* INPUT: The head pointer
* RETURNS: 1:if the queue is empty, and 0:if the queue is NOT empty
*/
/* Check to see if the queue is empty */
char isEmpty(link **h){
	/* ENTER YOUR CODE HERE */
	return(*h == NULL);
}/*isEmpty*/





/**************************************************************************************
* DESC: Obtains the number of links in the queue
* INPUT: The head and tail pointer
* RETURNS: An integer with the number of links in the queue
*/
/* returns the size of the queue*/
int size(link **h, link **t){

	link 	*temp;			/* will store the link while traversing the queue */
	int 	numElements;

	numElements = 0;

	temp = *h;			/* point to the first item in the list */

	while(temp != NULL){
		numElements++;
		temp = temp->next;
		}/*while*/

		return(numElements);
} 