

#ifndef LCDANDKEYPAD_H_
#define LCDANDKEYPAD_H_



#include <avr/io.h>    		// Standard AVR header
#include "avr/interrupt.h"
#include <math.h>		// Delay header
#include <util/delay.h>		// Delay header
#include <stdio.h>
#include <stdlib.h>
#include <string.h>





//************KEYPAD PART*****************************************************************************************************************

#define	KEY_PRT  PORTC		//keyboard PORT
#define	KEY_DDR  DDRC
#define	KEY_PIN  PINC

unsigned char keypad[4][4] =//{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
{ '1','2','3','*',
	'4','5','6','%',
	'7','8','9','*',
	'*','0','#','+'   //we dont use the last column    
};

unsigned char colloc, rowloc;

char keyfind()
{
	while(1)
	{
		KEY_DDR = 0xF0;           /* set port direction as input-output */
		KEY_PRT = 0xFF;

		do
		{
			KEY_PRT &= 0x0F;      /* mask PORT for column read only */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F); /* read status of column */
		}while(colloc != 0x0F);
		
		do
		{
			do
			{
				//_delay_ms(20);             /* 20ms key debounce time */
				colloc = (KEY_PIN & 0x0F); /* read status of column */
				}while(colloc == 0x0F);        /* check for any key press */
				
				//_delay_ms (40);	            /* 20 ms key debounce time */
				colloc = (KEY_PIN & 0x0F);
			}while(colloc == 0x0F);

			/* now check for rows */
			KEY_PRT = 0xEF;            /* check for pressed key in 1st row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 0;
				break;
			}

			KEY_PRT = 0xDF;		/* check for pressed key in 2nd row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 1;
				break;
			}
			
			KEY_PRT = 0xBF;		/* check for pressed key in 3rd row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 2;
				break;
			}

			KEY_PRT = 0x7F;		/* check for pressed key in 4th row */
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
			if(colloc != 0x0F)
			{
				rowloc = 3;
				break;
			}
		}

		if(colloc == 0x0E)
		return(keypad[rowloc][0]);
		else if(colloc == 0x0D)
		return(keypad[rowloc][1]);
		else if(colloc == 0x0B)
		return(keypad[rowloc][2]);
		else
		return(keypad[rowloc][3]);
	}
//****************************************************************************************************************************************************************


//********************LCD PART*********************************************************************************************************************************

#define	LCD_DPRT  PORTA 		//LCD DATA PORT
#define	LCD_DDDR  DDRA 		//LCD DATA DDR
#define	LCD_DPIN  PINA 		//LCD DATA PIN
#define	LCD_CPRT  PORTD 	//LCD COMMANDS PORT
#define	LCD_CDDR  DDRD 		//LCD COMMANDS DDR
#define	LCD_CPIN  PIND 		//LCD COMMANDS PIN
#define	LCD_RS  5 			//LCD RS
#define	LCD_RW  6 			//LCD RW
#define	LCD_EN  7 			//LCD EN


void lcdCommand( unsigned char cmnd )
{
  LCD_DPRT = cmnd;			//send cmnd to data port
  LCD_CPRT &= ~ (1<<LCD_RS);	//RS = 0 for command
  LCD_CPRT &= ~ (1<<LCD_RW);	//RW = 0 for write 
  LCD_CPRT |= (1<<LCD_EN);		//EN = 1 for H-to-L pulse
  _delay_us(1);				//wait to make enable wide
  LCD_CPRT &= ~ (1<<LCD_EN);	//EN = 0 for H-to-L pulse
  _delay_us(100);				//wait to make enable wide
}

void lcdData( unsigned char data )
{
  LCD_DPRT = data;			//send data to data port
  LCD_CPRT |= (1<<LCD_RS);		//RS = 1 for data
  LCD_CPRT &= ~ (1<<LCD_RW);	//RW = 0 for write
  LCD_CPRT |= (1<<LCD_EN);		//EN = 1 for H-to-L pulse
  _delay_us(1);				//wait to make enable wide
  LCD_CPRT &= ~ (1<<LCD_EN);	//EN = 0 for H-to-L pulse
  _delay_us(100);				//wait to make enable wide
}

void lcd_init()
{
  LCD_DDDR = 0xFF;
  LCD_CDDR = 0xFF;
 
  LCD_CPRT &=~(1<<LCD_EN);		//LCD_EN = 0
//  LCD_CPRT |= (1<<7);		//LCD_EN = 0
  _delay_us(2000);			//wait for init.
  lcdCommand(0x38);			//init. LCD 2 line, 5´7 matrix
  lcdCommand(0x0F);			//display on, cursor blinking
  lcdCommand(0x01);			//clear LCD
  _delay_us(2000);			//wait
  lcdCommand(0x06);			//shift cursor right
}

void lcd_gotoxy(unsigned char x, unsigned char y)
{  
 unsigned char firstCharAdr[]={0x80,0xC0,0x94,0xD4};//table 12-5  
 lcdCommand(firstCharAdr[y-1] + x - 1);
 _delay_us(100);	
}

void lcd_print( char * str )
{
  unsigned char i = 0 ;
  while(str[i]!=0)
  {
    lcdData(str[i]);
    i++ ;
  }
}
//**********************************************************************************************************************************************************************
#endif /* LCDANDKEYPAD_H_ */