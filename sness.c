#include <stdio.h>

#include <stdlib.h>

#include "gpio.h"

#include "uart.h"
#include <string.h> 
#define GPIO_BASE 0xFE200000

static unsigned *gpio = (unsigned*) GPIO_BASE;

 #define CLO_REG 0x7E003004

#define INP_GPIO(g) * (gpio + ((g)/10)) &= ~(7<<(((g)%10)*3))

#define OUT_GPIO(g) * (gpio + ((g)/10)) |= (1<<(((g)%10)*3))

#define CLK 11

#define LAT 9

#define DAT 10
#define size 16 
int b;
 int buttons[16];
//int buttons[size]; 

void Init_GPIO(){

    INP_GPIO(CLK);

    OUT_GPIO(CLK);

    INP_GPIO(LAT);

    OUT_GPIO(LAT);

    INP_GPIO(DAT);

}

 //write LATCH

void Write_Latch(int c){

   
if(c==1)
{
    gpio[7] = 1 << LAT ;}

else 
{

    gpio[10] = 1 << LAT;

 

    }
}
 //write clock

void Write_Clock(int c){
if(c==1)
{
gpio[7] = 1<<CLK;
}

//clear clock
else{
   gpio[10] = 1<<CLK;

}
}


void wait(int time)

{

unsigned *clo = (unsigned*) CLO_REG;

unsigned c = *clo + time; // micro Secs

while (c > *clo);

}

 
//read_SNES
void Read_SNES(int *buttons){
   
    int i=1;
        Write_Clock(1);

        Write_Latch(1);

        wait(12);

        Write_Latch(0);  

       while(i<=16)
       {
            wait(6);

            Write_Clock(0);

            wait(6);
            

          buttons[i]=(gpio[13]>>DAT)&1;

            Write_Clock(1);
            
         i++;
    }
}

int button_pressed(int *buttons){

    int index = 1;

    while(index<=16){

        // WAS IT PRESSED ??

        if(buttons[index] == 0)

        {

            //  BECAUSE NUMBERS STARTS FROM 1 ON THE SNESS

            //  THEREFORE, DISPLAY RETURNS INDEX+1

            return index+1;

        }

       

        index++;

    }

    // NO BUTTON IS PRESSED

    return -1;

} 

    



   
int main(){ 
    Init_GPIO();
   
    int buttons[17]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    void printf(char *str)
{
    uart_puts(str);
}
printf("CREATED BY ");
printf("Press a button\n");

int result=button_pressed(buttons);


    while(1)
    {

      Read_SNES(buttons);
     if(buttons[1]==0){

        printf("You have pressed B\n");
        printf("Press a button\n");

    }

    else if(buttons[2]==0)

    {

        printf("You have pressed Y\n"); 
        printf("Press a button\n");
    }

    else if(buttons[3]==0)

    {

        printf("You have pressed Select\n");
        printf("Press a button");

    }

    else if(buttons[5]==0){

    printf("You have pressed Joy-pad UP");}

    else if(buttons[6]==0)

    {printf("You have pressed Joy-pad DOWN");

    }

    else if(buttons[7]==0){

    

    printf("You have pressed Joy-pad LEFT");

    }

    else if(buttons[8]==0)

    {printf("You have pressed Joy-pad RIGHT");

    }

    else if(buttons[9]==0)

    {printf("You have pressed A");}

 

    else if(buttons[10]==0)

    {printf("You have pressed X");

    }

    else if(buttons[11]==0)

    {printf("You have pressed left");}

 

    else if(buttons[12]==0){
        printf("You  have pressed -- ");
    }}
   return 0;     
}  
    

