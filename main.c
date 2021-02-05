
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
 int i = 0;     

int counter;

char pressed;
int PINB_capture;



int main (void)
{
	DDRB = 0x00;

	// current_turn->data =  head->data;
	// current_turn->next = head->next;
	MCUCR = 0x03; //make INT0 rising edge triggered
	GICR = (1 << INT0); // enable external interrupt0 
	
	
	
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