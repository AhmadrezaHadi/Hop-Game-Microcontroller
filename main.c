
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


//************************ TIMER *****************************************************************************************************
int second = 0;

ISR (TIMER1_COMPA_vect) {
	second++;
	if(second == 60){
		second = 0;
	}
	
}

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
//**************** VARIABLES PART *********************************************************************************************

int starting_number ;
int current_number ;
int hop_number ;
int players_number ;

char input[4];
 int i = 0;     

int counter;

char pressed;
int PINB_capture;

int second_capture;

//*********************************************************************************************************************************

int winner()
{
	int wornot = 0;
	if (head->next == NULL && players_number != 1)
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
						}
				}
			}
}

void game_hop()
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
			}
		}
}

void notyourturn()
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

int main (void)
{
	DDRB = 0x00;
	DDRD = 0xFD;
	// current_turn->data =  head->data;
	// current_turn->next = head->next;
	MCUCR = 0x03; //make INT0 rising edge triggered
	GICR = (1 << INT0); // enable external interrupt0 
	
	counter = 0;
	OCR1A = 7811;
	TCCR1A = 0x00;
	TCCR1B = 0x0D;
	TIMSK = (1<<OCIE1A);
	
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
			lcd_gotoxy(i+1,4);
			lcd_print(&pressed);
			//_delay_ms(300);
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
			//debug	:
			//char output[5] ;
			//itoa(i,output,10);
			//lcd_print(output);
		}
	}
	
	else if (counter == 2) // it is the third time , so its the hop number
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
			lcd_gotoxy(i+1,4);
			lcd_print(&pressed);
			//_delay_ms(300);
			hop_number = atoi(input);
			counter++;

			for (int k = 1 ; k <= players_number ; k++)
			{
				addLast(&head,k);
			}


			lcd_init();
			lcd_print("Game starts with P1");
			lcd_gotoxy(9,3);
			char output[5] ;
			sprintf(output,"*%d*",current_number);
			lcd_print(output);
			second_capture = second;
			GICR = (1 << INT1);
		}
	}

}
ISR (INT1_vect)
{
	PINB_capture = PINB;

		lcd_init();
		/*char output[5] ;
		sprintf(output,"*%d*",second);
		lcd_print(output);
		lcd_gotoxy(1,2);
		char output2[5] ;
		sprintf(output2,"*%d*",second_capture);
		lcd_print(output2);
		lcd_gotoxy(1,3);
		char output3[5] ;
		int a = second - second_capture;
		sprintf(output3,"*%d*",a);
		lcd_print(output3);
		second_capture = second;*/
		if(counter == 3)
		{
			current_turn = head;
			counter++;
		}
		if (second - second_capture < 3)
		{
		
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
			second_capture = second;
		}
		else
		{
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
					char str[80];
					sprintf(str,"P%d turn",current_turn->data);
					lcd_print(str);
					lcd_gotoxy(9,3);
					char output[5] ;
					sprintf(output,"*%d*",current_number);
					lcd_print(output);
				}
			}
			second_capture = second;
		}
		
	
	
	
	
	
}