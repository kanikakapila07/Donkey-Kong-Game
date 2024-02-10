
//Simple Device Driver for SNES Controller

#include <stdio.h>
#include <stdlib.h>
#include "gpio.h"
#include "uart.h"
#include <string.h> 

//defining macros

#define GPIO_BASE 0xFE200000
#define CLO_REG 0x7E003004
#define INP_GPIO(g) * (gpio + ((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) * (gpio + ((g)/10)) |= (1<<(((g)%10)*3))
#define CLK 11                                                       // defining clock line to 11
#define LAT 9                                                        // defining Latch line to 9
#define DAT 10                                                       // defining data line to 10

static unsigned *gpio = (unsigned*) GPIO_BASE;

//initialising GPIO Lines   

void Init_GPIO(){

    INP_GPIO(CLK);                                                   //Initialising clock line to input
    OUT_GPIO(CLK);                                                   //Initialising clock line to output
    INP_GPIO(LAT);                                                   //Initialising latch line to input
    OUT_GPIO(LAT);                                                   //initialising latch line to output
    INP_GPIO(DAT);                                                   //Initialising data line to input

}

//Writing bit to GPIO Latch line

void Write_Latch(int c){

   
if(c==1)                                                             //writes a bit to GPIO LATCH line when c is 1
{
    gpio[7] = 1 << LAT ;}

else 
{

    gpio[10] = 1 << LAT;                                              //else clears GPIO latch line

 

    }
}

//Writing bit to GPIO Clock line

void Write_Clock(int c){
if(c==1)
{
gpio[7] = 1<<CLK;                                                     //writes a bit to GPIO Clock line
}

else{
   gpio[10] = 1<<CLK;                                                 //else clears GPIO Clock line

}
}

//Reads a bit from the GPIO Data line

 int Read_Data(){                                                           
   

   int v = (gpio[13]>>DAT)&1 ;                                        //Reading a value from Data Line

    return v;

}

//waits for a time interval 

void wait(int time)

{
unsigned *clo = (unsigned*) CLO_REG;
unsigned c = *clo + time;                                              //micro Secs
while (c > *clo);                                                      //waits until c>*clo

}

//Reads input(buttons) from the SNES controller 

void Read_SNES(int *buttons){
   
        int i=1;                                                      //initialises i to 1
        Write_Clock(1);                                               //writes 1 to clock line
        Write_Latch(1);                                               //writes 1 to latch line
        wait(12);                                                     //wait for 12 microseconds
        Write_Latch(0);                                               //clears the latch line
        while(i<=16)                                                  //loop for every button until i<=16
       {
            wait(6);                                                  //wait for 6 microseconds
            Write_Clock(0);                                           //clears the clock line
            wait(6);                                                  //wait for 6 microseconds
            buttons[i]=Read_Data();                                   //reads data for every button
            Write_Clock(1);                                           //write 1 to the clock line
         i++;                                                         //increments i
    }
}

//Copying elements of one array into another
void copy_array(int *a, int *b)
{
    for (int i=1;i<=16;i++)
    {
        a[i]=b[i];                                                    //copy each element of an array into another
    }
}
 

int main()
{ 
    Init_GPIO();                                                      //calls Init_GPIO subroutine
    void printf(char *str)                                            //used to print the string
        {
            uart_puts(str);
         }

    int buttons[17]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};              //array to store the current state of the buttons
    int prev_state[17]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};           //array to store the previous state of the buttons
   

     printf("Created by: ... \n ");                                    //prints the creator names
     printf("Please press a button...\n");                            //User input to press a button

          void Print_Message()                                        //prints the message when the button is pressed 
          {
                if(buttons[1]==0){                                    //prints B if button number 1 is pressed
  
                        printf("You have pressed B\n");
                        printf("Please press a button...\n");

                }

                 else if(buttons[2]==0)                               //prints Y if button number 2 is pressed

                {

                        printf("You have pressed Y\n"); 
                        printf("Please press a button...\n");
                }
                else if(buttons[3]==0)                                //prints Select if button number 3 is pressed
                {

                        printf("You have pressed Select\n");
                        printf("Please press a button...\n");

                }

                else if(buttons[5]==0)                                //prints Joy-pad UP if button number 5 is pressed
                {

                       printf("You have pressed Joy-pad UP\n");
                       printf("Please press a button...\n");
                }
                 

                else if(buttons[6]==0)                                //prints Joy-pad DOWN if button number 6 is pressed

                {
                       printf("You have pressed Joy-pad DOWN\n");
                       printf("Please press a button...\n");
                }

                else if(buttons[7]==0)                                //prints Joy-pad LEFT if button number 7 is pressed
                {
   
                       printf("You have pressed Joy-pad LEFT\n"); 
                       printf("Please press a button...\n");
                }

                else if(buttons[8]==0)                                //prints Joy-pad RIGHT if button number 8 is pressed

                {
                    printf("You have pressed Joy-pad RIGHT\n");
                    printf("Please press a button...\n");

                }

                else if(buttons[9]==0)                                //prints A if button number 9 is pressed

                {
                       printf("You have pressed A\n");
                       printf("Please press a button...\n");
                }

 

                 else if(buttons[10]==0)                              //prints X if button number 10 is pressed

                {
                       printf("You have pressed X\n");
                       printf("Please press a button...\n");

                }

                 else if(buttons[11]==0)                              //prints Left if button number 11 is pressed

                {  
                        printf("You have pressed Left\n");
                        printf("Please press a button...\n");
                }

                else if(buttons[12]==0)                               //prints Right if button number 12 is pressed
                {
                        printf("You have pressed Right\n");
                        printf("Please press a button...\n");
                }
}

    while(buttons[4]!=0)                                              //if start button is not pressed 
      {
       Read_SNES(buttons);                                            //then calls Read_SNES(buttons)
      
      for(int i=1;i<=16;i++)                                          //loops through every button to check if it is pressed or not
      {
        
       if(prev_state[i]==1 && buttons[i]==0)                          //if previous state is off and current state is on
       {
            Print_Message();                                          //calls Print_Message()
            prev_state[i]=buttons[i];                                 //store the state of the button that was just pressed
            copy_array(prev_state,buttons);                           //copy the whole array of current state(buttons) to previous state(prev_state)
          }
   
    
       }
   }
    
    printf("Program is terminating...\n");                            //prints when Start button is pressed
    return 0;                                                         //loop ends
  }
 
  

    

