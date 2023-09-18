// SUDEEP SAURABH 2k19/EC/190
// Swapnil Gour 2k19/EC/191

#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#define _XTAL_FREQ 4000000
#include <xc.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int rand_num;
int old_rand_num;
int score;
int score_tens;
int score_ones;
int shifter;
int delay;

//a function that varies the delay. Because passing variables to __delay_ms() doesn't work
void someDelay(){
    switch(delay){
        case 500:
            __delay_ms(500);
            break;
        case 200:
            __delay_ms(200);
            break;
        case 100:
            __delay_ms(100);
            break;
        case 50:
            __delay_ms(50);
            break;
    }
    return;
}
void main(void) {
    
    T1CON = 0b00000001;             //turn on TMR1
    INTCON = 0b10101000;            //use global, TMR0 and RB change interrupts
    OPTION_REG = 0b00000100;        //use 1:32 prescale for TMR0
    TRISB = 0xF0;                   //inputs on RB4 to RB7, outputs for RB0 to RB3
    TRISC = 0;                      //all PORTC are outputs
    TRISD = 0;                      //all PORTD are outputs
    
    delay = 500;                    //default delay
   
    loop:
    while(1){
        srand(TMR1);                //use TMR1 value as seed for random
        rand_num = rand() % 9;      //generate random numbers from 0 to 9
        //filter out consecutive numbers, 0 and those that has at least two bits high (011 = 3, 101 = 5, 110 = 6, 111 = 7)
        if(rand_num == old_rand_num || rand_num == 0 || rand_num == 3 || rand_num == 5 || rand_num == 6 || rand_num == 7){ 
            goto loop;
        }
        PORTD = rand_num;           
        old_rand_num = rand_num;    //
        someDelay();                //a function that varies the delay. Because passing variables to __delay_ms() doesn't work 
    }
    
    return;
}

void interrupt isr(void){
    if(TMR0IF){
         //switch digits for every interrupt. Read about POV for better understanding
            shifter++;        
            switch(shifter){
                case 1:
                    PORTB = 0b11111110;
                    PORTC = score_ones;
                    break;
                case 2:
                    PORTB = 0b11111101;
                    PORTC = score_tens;
                    break;
                case 6:
                    shifter = 0;
                    break;
            }
            TMR0IF = 0;     //manual clearing of interrupt flag
    }
    if(RBIF){
        int dummy = PORTB;          //read PORTB to clear mismatch condition
        int masked = (PORTB & 0xF0) >> 4;   //acquire pressed buttons
        if(~masked == (PORTD + 0xFFF0)){
            score++;
            score_tens = score/10;
            score_ones = score - score_tens*10;
            //adjust speed according to score
            if(score <= 5){ 
                delay = 500;
            }else if(score > 5 && score <= 10){
                delay = 200;
            }else if(score > 10 && score <= 20){
                delay = 100;
            }else if(score > 20 && score <= 30){
                delay = 50;
            }else if(score > 99){
                score = 99;
            }else{
                delay = 500;
            }
        }
        RBIF = 0;
    }
}