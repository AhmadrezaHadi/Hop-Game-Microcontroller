
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>    		// Standard AVR header
#include "avr/interrupt.h"
#include <math.h>		// Delay header
#include <util/delay.h>		// Delay header
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LCDandKeypad.h"


//**************** VARIABLES PART *********************************************************************************************

int starting_number ;
int current_number ;
int hop_number ;
int players_number ;

char input[4];
char input2[4];
 int i = 0;     

int counter;

char pressed;
int PINB_capture;

int timer_on;

//*********************************************************************************************************************************

//**************** Linked List *****************************************************************************************************  

struct node
{
	int data;
	struct node *next;
};

  
void addLast(struct node **head, int val)
{
	//create a new node
	struct node *newNode = malloc(sizeof(struct node));
	newNode->data = val;
	newNode->next     = NULL;

	//if head is NULL, it is an empty list
	if(*head == NULL)
	*head = newNode;
	//Otherwise, find the last node and add the newNode
	else
	{
		struct node *lastNode = *head;

		//last node's next address will be NULL.
		while(lastNode->next != NULL)
		{
			lastNode = lastNode->next;
		}

		//add the newNode at the end of the linked list
		lastNode->next = newNode;
	}

}

void deleteNode(struct node **head_ref, int key)
{
	// Store head node
	struct node* temp = *head_ref, *prev;
	
	// If head node itself holds the key to be deleted
	if (temp != NULL && temp->data == key)
	{
		*head_ref = temp->next;   // Changed head
		free(temp);               // free old head
		return;
	}
	
	// Search for the key to be deleted, keep track of the
	// previous node as we need to change 'prev->next'
	while (temp != NULL && temp->data != key)
	{
		prev = temp;
		temp = temp->next;
	}
	
	// If key was not present in linked list
	if (temp == NULL) return;
	
	// Unlink the node from linked list
	prev->next = temp->next;
	
	free(temp);  // Free memory
}

struct node* head = NULL;
struct node* current_turn = NULL;


//**********************************************************************************************************************************

//************************ TIMER *****************************************************************************************************
int second = 0;

ISR (TIMER1_COMPA_vect) 
{
	if (timer_on)
	{
	
			second++;
			if(second == 4)
			{
				second = 0;
				lcd_init();
				lcd_print("Time is up!");
				lcd_gotoxy(1,2);
				lcd_print("You lost!");
				lcd_gotoxy(1,3);
				deleteNode(&head,current_turn->data);
				if (players_number != 1)
				{
					current_turn = current_turn->next;
					if (current_turn == NULL)
					{
						current_turn = head;
					}
					if (!winner())
					{
						OCR1A = 7811;
						TCCR1A = 0x00;
						TCCR1B = 0x0D;
						second = 0;
						char str[80];
						sprintf(str,"P%d turn",current_turn->data);
						lcd_print(str);
						lcd_gotoxy(9,3);
						char output[5] ;
						sprintf(output,"*%d*",current_number);
						lcd_print(output);
					}

				}
			}
	}
}

void start_over()
{
	if (timer_on)
	{
		OCR1A = 7811;
		TCCR1A = 0x00;
		TCCR1B = 0x0D;
		second = 0;
	}
}

//******************************** Game Functions *****************************************************************************************************

int winner()
{
	int wornot = 0;
	if (head == NULL && PINB_capture != 0x00)
	{
			lcd_init();
			lcd_print("Game is finished!");
			TIMSK &= ~(1<<OCIE1A);
			wornot = 2;
	}
	else if (head->next == NULL && players_number != 1)
	{
		_delay_ms(1000);
		lcd_init();
		lcd_gotoxy(9,1);
		char str[80];
		sprintf(str,"P%d Won!",current_turn->data);
		lcd_print(str);
		wornot = 1;
	}

	return wornot;
}

void game_next()
{
	if (winner() != 2)
	{
	
		current_number ++ ;
		/*har junk2[5] ;
		lcd_gotoxy(1,4);
		itoa(current_turn->data,junk2,10);
		lcd_print(junk2);*/
		if((current_number - starting_number)% hop_number != 0) //it is not hop
		{
			lcd_gotoxy(9,3);
			char output[5] ;
			sprintf(output,"*%d*",current_number);
			lcd_print(output);	
			lcd_gotoxy(1,1);
			current_turn = current_turn->next;
			if (current_turn == NULL)
			{
				current_turn = head;
			}
			char str[80];
			sprintf(str,"P%d turn",current_turn->data);
			lcd_print(str);
			start_over();
		}
			else // it is hop
			{

				lcd_print("It was hop!");
				lcd_gotoxy(1,2);
				lcd_print("You lost!");
				lcd_gotoxy(1,3);
				deleteNode(&head,current_turn->data);
				if (players_number != 1)
				{
						current_turn = current_turn->next;
						if (current_turn == NULL)
						{
							current_turn = head;
						}
						if (!winner())
						{
							char str[80];
							sprintf(str,"P%d turn",current_turn->data);
							lcd_print(str);
							lcd_gotoxy(9,3);
							char output[5] ;
							sprintf(output,"*%d*",current_number);
							lcd_print(output);
							start_over();
						}
				}
			}
	}
}

void game_hop()
{
	if (winner() != 2)
	{
	
		current_number ++ ;
		if((current_number - starting_number)% hop_number != 0) //it is not hop
		{
			
			lcd_print("It was not hop!");
			lcd_gotoxy(1,2);
			lcd_print("You lost!");
			lcd_gotoxy(1,3);

				deleteNode(&head,current_turn->data);
				if (players_number != 1)
				{
					current_turn = current_turn->next;
					if (current_turn== NULL)
					{
						current_turn = head;
					}
					if (!winner())
					{
						char str[80];
						sprintf(str,"P%d turn",current_turn->data);
						lcd_print(str);
						lcd_gotoxy(9,3);
						char output[5] ;
						sprintf(output,"*%d*",current_number);
						lcd_print(output);
						start_over();
					}
				}
				
		}
		else //it is hop
		{
			lcd_gotoxy(9,2);
			lcd_print("We hopped!");
			current_number ++ ;
			lcd_gotoxy(9,3);
			char output[5] ;
			sprintf(output,"*%d*",current_number);
			lcd_print(output);
			lcd_gotoxy(1,1);
			current_turn = current_turn->next;
			if (current_turn == NULL)
			{
				current_turn = head;
			}
			if (!winner())
			{
				char str[80];
				sprintf(str,"P%d turn",current_turn->data);
				lcd_print(str);
				start_over();
			}
		}
	}
}

void notyourturn()
{
	if (winner() != 2)
	{
	
		lcd_gotoxy(1,1);
		lcd_print("Its not your turn!");
		lcd_gotoxy(1,2);
		char str[80];
		sprintf(str,"P%d turn",current_turn->data);
		lcd_print(str);
		lcd_gotoxy(9,3);
		char output[5] ;
		sprintf(output,"*%d*",current_number);
		lcd_print(output);
		lcd_gotoxy(1,1);
	}
}

int main (void)
{
	DDRB = 0x00;
	DDRD = 0xFD;
	// current_turn->data =  head->data;
	// current_turn->next = head->next;
	MCUCR = 0x03; //make INT0 rising edge triggered
	GICR = (1 << INT0); // enable external interrupt0 
	
	counter = 0;
	
	lcd_init();
	lcd_print("Number of Players:");
	lcd_gotoxy(1,2);
	lcd_print("(press * after it)");
	lcd_gotoxy(1,3);
	lcd_print("(Max : 4)");
	lcd_gotoxy(1,4);
	
	sei(); //enable interrupts
	while(1);
}
ISR (INT0_vect)
{
	pressed = keyfind();
	
	if(counter == 0) //it is the first number , so its the players number
	{
		if(pressed != '*')
		{
			lcd_gotoxy(1,4);
			lcd_print(&pressed);
			players_number =(int)(pressed - '0');
		}
		if(pressed == '*')
		{
			counter++;
			lcd_init();
			lcd_print("Starting Number:");
			lcd_gotoxy(1,2);
			lcd_print("(press * after it)");
			lcd_gotoxy(1,3);
			lcd_print("(Max : 9999)");
			lcd_gotoxy(1,4);
		}
	}
	
	else if (counter == 1) // it is the second time , so its the starting number
	{
		if(pressed != '*')
		{
			lcd_gotoxy(i+1,4);
			lcd_print(&pressed);
			input[i] = pressed;
			i++;
		}
		else if(pressed == '*')
		{
			starting_number = atoi(input);
			current_number = starting_number ;
			counter++;
			i = 0;
			lcd_init();
			lcd_print("Hop Number:");
			lcd_gotoxy(1,2);
			lcd_print("(press * after it)");
			lcd_gotoxy(1,3);
			lcd_print("(Max : 9999)");
			lcd_gotoxy(1,4);
		}
	}
	
	else if (counter == 2) // it is the third time , so its the hop number
	{
		if(pressed != '*')
		{
			lcd_gotoxy(i+1,4);
			lcd_print(&pressed);
			input2[i] = pressed;
			i++;
		}
		else if(pressed == '*')
		{
			hop_number = atoi(input2);
			counter++;
			i = 0;
			for (int k = 1 ; k <= players_number ; k++)
			{
				addLast(&head,k);
			}
			lcd_init();
			lcd_print("Speed mode:");
			lcd_gotoxy(1,2);
			lcd_print("(1 for yes/0 for no)");
			lcd_gotoxy(1,3);
			lcd_print("(press * after it)");
		}
	}
	else if (counter == 3)
	{
		if(pressed != '*')
		{
			lcd_gotoxy(i+1,4);
			lcd_print(&pressed);
			timer_on =(int)(pressed - '0');
			i++;
		}
		else if(pressed == '*')
		{
			counter++;
				lcd_init();
				lcd_print("Game starts with P1");
				lcd_gotoxy(9,3);
				char output[5] ;
				sprintf(output,"*%d*",current_number);
				lcd_print(output);
				GICR = (1 << INT1);
		}
		
	}

}
ISR (INT1_vect)
{
	PINB_capture = PINB;

		lcd_init();

		if(counter == 4)
		{
			current_turn = head;
			if (timer_on)
			{
				OCR1A = 7811;
				TCCR1A = 0x00;
				TCCR1B = 0x0D;
				TIMSK = (1<<OCIE1A);
			}
			counter++;
		}

			if(PINB_capture == 0x01) //p1 next
			{
				if(current_turn->data == 1) 
				{
					game_next();
				}
				else
				{
					notyourturn();
				}
			}

			else if(PINB_capture == 0x02) //p1 hop
			{
				if (current_turn->data == 1)
				{
					game_hop();

				}
				else
				{
					notyourturn();
				}
			}

			else if(PINB_capture == 0x04) //p2 next
			{
				if (current_turn->data == 2)
				{
					game_next();
				}
				else
				{
					notyourturn();
				}
			}
			else if(PINB_capture == 0x08) //p2 hop
			{
				if (current_turn->data == 2)
				{
					game_hop();
				}
				else
				{
					notyourturn();
				}
			}
		
			else if(PINB_capture == 0x10) //p3 next
			{
				if (current_turn->data == 3)
				{
					game_next();
				}
				else
				{
					notyourturn();
				}
			}
			else if(PINB_capture == 0x20) //p3 hop
			{
				if (current_turn->data == 3)
				{
					game_hop();
				}
				else
				{
					notyourturn();
				}
			}
			else if(PINB_capture == 0x40) //p4 next
			{
				if (current_turn->data == 4)
				{
					game_next();
				}
				else
				{
					notyourturn();
				}
			}
			else if(PINB_capture == 0x80) // p4 hop
			{
				if (current_turn->data == 4)
				{
					game_hop();
				}
				else
				{
					notyourturn();
				}
			}
			
		if (PINB_capture == 0x00)
		{
			lcd_print("RESET!");
			counter = 0;
			DDRB = 0x00;
			GICR = (1 << INT0); // enable external interrupt0 
			i = 0;
			if (head != NULL)
			{
					for(int k = 0 ; k < players_number ; k++)
					{
						deleteNode(&head,k+1);
					}
			}
			timer_on = 0;
			_delay_ms(1000);
			start_over();
			lcd_init();
			lcd_print("Number of Players:");
			lcd_gotoxy(1,2);
			lcd_print("(press * after it)");
			lcd_gotoxy(1,3);
			lcd_print("(Max : 4)");
			lcd_gotoxy(1,4);
		}
}