//Raspberry Pi Game 


#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "gpio.h"
#include "uart.h"
#include "fb.h"

//includes the header files for images 
#include "startimg.h"
#include "quitimg.h"
#include "tile.h"
#include "dk.h"
#include "livesimg.h"
#include "timerimg.h"
#include "cactus.h"
#include "snakeimg.h"
#include "rockimg.h"
#include "wallimg.h"
#include "coin.h"
#include "scoresimg.h"
#include "zero.h"
#include "oneimg.h"
#include "two.h"
#include "three.h"
#include "four.h"
#include "five.h"
#include "six.h"
#include "sevenimg.h"
#include "eight.h"
#include "nineimg.h"
#include "heartimg.h"
#include "flagimg.h"
#include "winimg.h"
#include "lostimg.h"
#include "pumpkinimg.h"
#include "sandimg.h"


#define GPIO_BASE 0xFE200000
#define GPIO_BASE 0xFE200000
#define CLO_REG 0x7E003004
#define INP_GPIO(g) * (gpio + ((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) * (gpio + ((g)/10)) |= (1<<(((g)%10)*3))
#define CLK 11                                                       // defining clock line to 11
#define LAT 9                                                        // defining Latch line to 9
#define DAT 10                                                       // defining data line to 10

static unsigned *gpio = (unsigned *) GPIO_BASE;
unsigned int time_left_sec;

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
        int i=1;                                                     //initialises i to 1
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
 

 //defines structure for obstacles with their x and y coordinate 
struct obstacle{
    int x;
    int y;
};
struct obstacle obs_array[16];

int stage=1;
int grid[24][32];
xoff=0;
yoff=0;
dk_x=0;
dk_y=23;
int lives=4;
int winflag=0;
int loseflag=0;

//draws the main screen background 
void gamescreen()
{
//draw the tiles for background 
for(int i=0;i<24;i++) // rows
{
    for(int j=0;j<32;j++) // cols 
    {
        int offx = 32 * j;
        int offy = 32 * i;
        myDrawImage(tile.pixel_data,tile.width,tile.height,offx,offy);
    }
}
myDrawImage(dk.pixel_data,dk.width,dk.height,0,23*32);
} 

//dk when moving right
void moveRight(){
    if(dk_x==8 && dk_y==22 || dk_x==0 && dk_y==13 || dk_x==8 && dk_y==8 || dk_x==5 && dk_y==9)
    {
        myDrawImage(cactus.pixel_data,cactus.width,cactus.height,dk_x*32,dk_y*32);
    }
    else if(dk_x==21 && dk_y==5 || dk_x==11 && dk_y==11 || dk_x==0 && dk_y==8 || dk_x==5 && dk_y==3)
    {
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==11 && dk_y==20 || dk_x==6 && dk_y==10 || dk_x==4 && dk_y==6 || dk_x==11 && dk_y==5)
    {
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==7 && dk_y==15 || dk_x==2 && dk_y==10 || dk_x==2 && dk_y==5 || dk_x==9 && dk_y==2)
    {
        myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(dk_x)*32,(dk_y)*32);
    }
    else{
        myDrawImage(tile.pixel_data,tile.width,tile.height,dk_x*32,dk_y*32);
    }
    dk_x=dk_x+1;
    if(dk_x>=0&& dk_y>=0&& dk_x<32&& dk_y<24)
    {
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
    else
    {
        dk_x=31;
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
}


//dk when moving left
void moveLeft(){
    if(dk_x==8 && dk_y==22 || dk_x==0 && dk_y==13 || dk_x==8 && dk_y==8 || dk_x==5 && dk_y==9)
    {
        myDrawImage(cactus.pixel_data,cactus.width,cactus.height,dk_x*32,dk_y*32);
    }
    else if(dk_x==21 && dk_y==5 || dk_x==11 && dk_y==11 || dk_x==0 && dk_y==8 || dk_x==5 && dk_y==3)
    {
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==11 && dk_y==20 || dk_x==6 && dk_y==10 || dk_x==4 && dk_y==6 || dk_x==11 && dk_y==5)
    {
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==7 && dk_y==15 || dk_x==2 && dk_y==10 || dk_x==2 && dk_y==5 || dk_x==9 && dk_y==2)
    {
        myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(dk_x)*32,(dk_y)*32);
    }
    else{
        myDrawImage(tile.pixel_data,tile.width,tile.height,dk_x*32,dk_y*32);
    }
    dk_x=dk_x-1;
    dk_y=dk_y;
    if(dk_x>=0&& dk_y>=0 && dk_x<32&& dk_y<24)
    {
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
    else{
        dk_x=0;
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
}

//dk when moving up
void moveUp(){
    if(dk_x==8 && dk_y==22 || dk_x==0 && dk_y==13 || dk_x==8 && dk_y==8 || dk_x==5 && dk_y==9)
    {
        myDrawImage(cactus.pixel_data,cactus.width,cactus.height,dk_x*32,dk_y*32);
    }
    else if(dk_x==21 && dk_y==5 || dk_x==11 && dk_y==11 || dk_x==0 && dk_y==8 || dk_x==5 && dk_y==3)
    {
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==11 && dk_y==20 || dk_x==6 && dk_y==10 || dk_x==4 && dk_y==6 || dk_x==11 && dk_y==5)
    {
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==7 && dk_y==15 || dk_x==2 && dk_y==10 || dk_x==2 && dk_y==5 || dk_x==9 && dk_y==2)
    {
        myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(dk_x)*32,(dk_y)*32);
    }
    else{
        myDrawImage(tile.pixel_data,tile.width,tile.height,dk_x*32,dk_y*32);
    }
    dk_x=dk_x;
    dk_y=dk_y-1;
    if(dk_x>=0&& dk_y>=0 && dk_x<32&& dk_y<24)
    {
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
    else{
        dk_y=0;
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
        }
    }

//dk when moving down
void moveDown(){   
    if(dk_x==8 && dk_y==22 || dk_x==0 && dk_y==13 || dk_x==8 && dk_y==8 || dk_x==5 && dk_y==9)
    {
        myDrawImage(cactus.pixel_data,cactus.width,cactus.height,dk_x*32,dk_y*32);
    }
    else if(dk_x==21 && dk_y==5 || dk_x==11 && dk_y==11 || dk_x==0 && dk_y==8 || dk_x==5 && dk_y==3)
    {
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==11 && dk_y==20 || dk_x==6 && dk_y==10 || dk_x==4 && dk_y==6 || dk_x==11 && dk_y==5)
    {
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==7 && dk_y==15 || dk_x==2 && dk_y==10 || dk_x==2 && dk_y==5 || dk_x==9 && dk_y==2)
    {
        myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(dk_x)*32,(dk_y)*32);
    }
    else{
        myDrawImage(tile.pixel_data,tile.width,tile.height,dk_x*32,dk_y*32);
    }
    dk_y=dk_y+1;
    if(dk_x>=0&& dk_y>=0 && dk_x<32&& dk_y<24)
    {
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
        }
    else{
        dk_y=23;
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
        }
    }


//draws the obstacles on the screen
void draw_obstacles(){
    obs_array[0].x= 8;
    obs_array[0].y= 22;
    obs_array[1].x= 21;
    obs_array[1].y= 5;
    obs_array[2].x=11;
    obs_array[2].y=20;
    obs_array[3].x=7;
    obs_array[3].y=15;
    obs_array[4].x=0;
    obs_array[4].y=13;
    obs_array[5].x= 11;
    obs_array[5].y=11;
    obs_array[6].x=6;
    obs_array[6].y=10;
    obs_array[7].x=2;
    obs_array[7].y=10;
    obs_array[8].x=8;
    obs_array[8].y=8;
    obs_array[9].x=0;
    obs_array[9].y=8;
    obs_array[10].x=4;
    obs_array[10].y=6;
    obs_array[11].x=2;
    obs_array[11].y=5;
    obs_array[12].x=5;
    obs_array[12].y=9;
    obs_array[13].x=5;
    obs_array[13].y=3;
    obs_array[14].x=11;
    obs_array[14].y=5;
    obs_array[15].x=9;
    obs_array[15].y=2;
    for(int i=0;i<16;i=i+4)
    {
        myDrawImage(cactus.pixel_data,cactus.width,cactus.height,(obs_array[i].x)*32,(obs_array[i].y)*32);
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(obs_array[i+1].x)*32,(obs_array[i+1].y)*32);
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(obs_array[i+2].x)*32,(obs_array[i+2].y)*32);
        myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(obs_array[i+3].x)*32,(obs_array[i+3].y)*32);
    }
}

//fixes the position for placing coins 
int coinx[]={9,23,7,11,21,22,27,30,30};
int coiny[]={23,23,22,22,21,10,11,9,5};

//initiates value for score 
int scores=0;

//detects collision of dk and obstacle
void detect_collision()
{
    for(int i=0;i<16;i++)
    {
        if(dk_x==obs_array[i].x && dk_y==obs_array[i].y)
        {
            uart_puts("collided");
        }
    }
}

//draws the coin on the game screen
void drawCoins()
{
    for (int i=0,j=0;i<9 && j<9;i++,j++)
    {
        myDrawImage(coin.pixel_data,coin.width,coin.height,coinx[i]*32,coiny[j]*32);
    }
}

//calculates the scores as the game goes on 
void calculate_scores()
{
    for(int i=0,j=0;i<9 && j<9 ; i++,j++)
    {
        if (dk_x==coinx[i] && dk_y==coiny[j])       //if position of dk and coin is same
        {
            if(coinx[i]!=0 && coiny[j]!=0)          
            {           
                scores=scores+100;                  //then it increases the score by 100 
                coinx[i]=0;
                coiny[j]=0;
                extract_digits();                   
            }
        }
    }
}
    
//initiates x_score and y_score coordinates 
int x_score;
int y_score;

//count the digits of current scores       
int count_digits(int n)
{
    int count = 0;
    do {
        n=n/10;
        ++count;
    }
    while (n != 0);
    return count;
}

int c;

//extracts the digits as score is counted 
void extract_digits()
{
    int num = scores;
    c = count_digits(scores);
    int power = 1;
    while(num!=0)
    {
        int digits = num % 10;
        drawScoreImage(digits);
        num = num/10;
    }
}

//displays the score as the game goes on 
void drawScoreImage(int d)
{
    switch(d)
    {
        case 0:
        myDrawImage(zero.pixel_data,zero.width,zero.height,(c+1)*32,y_score*32);
        c--;
        break;
        
        case 1:
        myDrawImage(oneimg.pixel_data,oneimg.width,oneimg.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 2:
        myDrawImage(two.pixel_data,two.width,two.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 3:
        myDrawImage(three.pixel_data,three.width,three.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 4:
        myDrawImage(four.pixel_data,four.width,four.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 5:
        myDrawImage(five.pixel_data,five.width,five.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 6:
        myDrawImage(six.pixel_data,six.width,six.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 7:
        myDrawImage(sevenimg.pixel_data,sevenimg.width,sevenimg.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 8:
        myDrawImage(eight.pixel_data,eight.width,eight.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 9:
        myDrawImage(nineimg.pixel_data,nineimg.width,nineimg.height,(c+1)*32,y_score*32);
        c--;
        break;
    }
}

//checks for collision of dk and obstacle 
void check_collision()
{
    for(int i=0;i<16;i++)
    {
        if(dk_x==obs_array[i].x && dk_y==obs_array[i].y);
        {
            lives = lives-1;
           
        }
    }
}

//checks if dk wins to proceed to next stage
void checkwin()
{
    if(dk_x==30 &&dk_y==1)
    {
        myDrawImage(winimg.pixel_data,winimg.width,winimg.height,11*32,12*32);
        winflag=1;
        //when win condition is true
        //it then proceeds to stage 2 
        gamescreenstagetwo();                                                           //draws game screen for stage 2
        draw_obstaclesstagetwo();                                                       //draws obstacles for stage 2
        drawCoinsstagetwo();                                                            //draws coins for stage 2

        //draws following images 
        myDrawImage(scoresimg.pixel_data,scoresimg.width,scoresimg.height,0,0);         
        myDrawImage(zero.pixel_data,zero.width,zero.height,2*32,0);
        myDrawImage(livesimg.pixel_data,livesimg.width,livesimg.height,11*32,0*32);
        myDrawImage(heartimg.pixel_data,heartimg.width,heartimg.height,13*32,0*32);
        myDrawImage(heartimg.pixel_data,heartimg.width,heartimg.height,14*32,0*32);
        myDrawImage(heartimg.pixel_data,heartimg.width,heartimg.height,15*32,0*32);
        myDrawImage(heartimg.pixel_data,heartimg.width,heartimg.height,16*32,0*32);
        myDrawImage(timerimg.pixel_data,timerimg.width,timerimg.height,21*32,0*32);
        myDrawImage(flagimg.pixel_data,flagimg.width,flagimg.height,30*32,1*32);
    }
}


int l = 16;
//checks if dk loses the game on hitting obstacles 
void checkLose()
{
    for(int i=0;i<16;i++)
    {
        if(dk_x==obs_array[i].x &&dk_y==obs_array[i].y)
        {
            lives=lives-1;
            if(lives>=1 && l>=14 && l<=16)
            {
                myDrawImage(tile.pixel_data,tile.width,tile.height,l*32,0);
                l = l-1;
            }
            else{
                myDrawImage(tile.pixel_data,tile.width,tile.height,l*32,0);
                myDrawImage(lostimg.pixel_data,lostimg.width,lostimg.height,11*32,12*32);
                loseflag=1;
                }
            }
        }
}

//displays time on screen 
extract_time_digits(){
    int num = time_left_sec;
    c = count_digits(time_left_sec);
    int power=1;
    while(num!=0)
    {
        int digits=num%10;
        num=num/10;
    }
}

//displays the running time 
void drawtimeImage(int d)
{
    switch(d)
      {
        case 0:
        myDrawImage(zero.pixel_data,zero.width,zero.height,(c+23)*32,y_score*32);
        c--;
        break;
        
        case 1:
        myDrawImage(oneimg.pixel_data,oneimg.width,oneimg.height,(c+23)*32,y_score*32);
        c--;
        break;

        case 2:
        myDrawImage(two.pixel_data,two.width,two.height,(c+23)*32,y_score*32);
        c--;
        break;

        case 3:
        myDrawImage(three.pixel_data,three.width,three.height,(c+23)*32,y_score*32);
        c--;
        break;

        case 4:
        myDrawImage(four.pixel_data,four.width,four.height,(c+23)*32,y_score*32);
        c--;
        break;

        case 5:
        myDrawImage(five.pixel_data,five.width,five.height,(c+23)*32,y_score*32);
        c--;
        break;

        case 6:
        myDrawImage(six.pixel_data,six.width,six.height,(c+23)*32,y_score*32);
        c--;
        break;

        case 7:
        myDrawImage(sevenimg.pixel_data,sevenimg.width,sevenimg.height,(c+23)*32,y_score*32);
        c--;
        break;

        case 8:
        myDrawImage(eight.pixel_data,eight.width,eight.height,(c+23)*32,y_score*32);
        c--;
        break;

        case 9:
        myDrawImage(nineimg.pixel_data,nineimg.width,nineimg.height,(c+23)*32,y_score*32);
        c--;
        break;
    }
}

//**********************************************STAGE TWO***********************************************//

//defines the structure for obstacles for stage two 
struct obstacle obs_arraytwo[20];

//draws the main screen background 
void gamescreenstagetwo()
{
//draw tiles for background 
for(int i=0;i<24;i++) // rows
{
    for(int j=0;j<32;j++) // cols 
    {
        int offx = 32 * j;
        int offy = 32 * i;
        myDrawImage(sandimg.pixel_data,sandimg.width,sandimg.height,offx,offy);
    }
}
myDrawImage(dk.pixel_data,dk.width,dk.height,0,23*32);
} 

//dk when moving right 
void moveRightstagetwo(){
    if(dk_x==8 && dk_y==22 || dk_x==0 && dk_y==13 || dk_x==8 && dk_y==8 || dk_x==5 && dk_y==9 || dk_x==23 &&dk_y==14)
    {
        myDrawImage(pumpkinimg.pixel_data,pumpkinimg.width,pumpkinimg.height,dk_x*32,dk_y*32);
    }
    else if(dk_x==21 && dk_y==5 || dk_x==11 && dk_y==11 || dk_x==0 && dk_y==8 || dk_x==5 && dk_y==3 ||dk_x==15 && dk_y==10)
    {
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==11 && dk_y==20 || dk_x==6 && dk_y==10 || dk_x==4 && dk_y==6 || dk_x==11 && dk_y==5 ||dk_x==3 &&dk_x==7)
    {
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==7 && dk_y==15 || dk_x==2 && dk_y==10 || dk_x==2 && dk_y==5 || dk_x==9 && dk_y==2 || dk_x==29 &&dk_y==29)
    {
        myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(dk_x)*32,(dk_y)*32);
    }
    else{
        myDrawImage(sandimg.pixel_data,sandimg.width,sandimg.height,dk_x*32,dk_y*32);
        }
    dk_x=dk_x+1;
    if(dk_x>=0&& dk_y>=0&& dk_x<32&& dk_y<24)
    {
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
    else
    {
        dk_x=31;
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
}

//dk when moving left
void moveLeftstagetwo(){
    if(dk_x==8 && dk_y==22 || dk_x==0 && dk_y==13 || dk_x==8 && dk_y==8 || dk_x==5 && dk_y==9 || dk_x==23 &&dk_y==14)
    {
        myDrawImage(pumpkinimg.pixel_data,pumpkinimg.width,pumpkinimg.height,dk_x*32,dk_y*32);
    }
    else if(dk_x==21 && dk_y==5 || dk_x==11 && dk_y==11 || dk_x==0 && dk_y==8 || dk_x==5 && dk_y==3 ||dk_x==15 && dk_y==10)
    {
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==11 && dk_y==20 || dk_x==6 && dk_y==10 || dk_x==4 && dk_y==6 || dk_x==11 && dk_y==5 ||dk_x==3 &&dk_x==7)
    {
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==7 && dk_y==15 || dk_x==2 && dk_y==10 || dk_x==2 && dk_y==5 || dk_x==9 && dk_y==2 || dk_x==29 &&dk_y==29 )
    {
        myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(dk_x)*32,(dk_y)*32);
    }
    else{
        myDrawImage(sandimg.pixel_data,sandimg.width,sandimg.height,dk_x*32,dk_y*32);
    }
    dk_x=dk_x-1;
    dk_y=dk_y;
    if(dk_x>=0&& dk_y>=0 && dk_x<32&& dk_y<24)
    {
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
    else
    {
        dk_x=0;
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
}

//dk when moving up
void moveUpstagetwo(){
    if(dk_x==8 && dk_y==22 || dk_x==0 && dk_y==13 || dk_x==8 && dk_y==8 || dk_x==5 && dk_y==9 || dk_x==23 &&dk_y==14)
    {
        myDrawImage(pumpkinimg.pixel_data,pumpkinimg.width,pumpkinimg.height,dk_x*32,dk_y*32);
    }
    else if(dk_x==21 && dk_y==5 || dk_x==11 && dk_y==11 || dk_x==0 && dk_y==8 || dk_x==5 && dk_y==3 ||dk_x==15 && dk_y==10)
    {
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==11 && dk_y==20 || dk_x==6 && dk_y==10 || dk_x==4 && dk_y==6 || dk_x==11 && dk_y==5 ||dk_x==3 &&dk_x==7)
    {
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==7 && dk_y==15 || dk_x==2 && dk_y==10 || dk_x==2 && dk_y==5 || dk_x==9 && dk_y==2 || dk_x==29 &&dk_y==29)
    {
      myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(dk_x)*32,(dk_y)*32);
    }
    else{
        myDrawImage(sandimg.pixel_data,sandimg.width,sandimg.height,dk_x*32,dk_y*32);
    }
    dk_x=dk_x;
    dk_y=dk_y-1;
    if(dk_x>=0&& dk_y>=0 && dk_x<32&& dk_y<24)
    {
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
    else
    {
        dk_y=0;
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
}

//dk when moving down 
void moveDownstagetwo(){   
    if(dk_x==8 && dk_y==22 || dk_x==0 && dk_y==13 || dk_x==8 && dk_y==8 || dk_x==5 && dk_y==9 || dk_x==23 &&dk_y==14)
    {
        myDrawImage(pumpkinimg.pixel_data,pumpkinimg.width,pumpkinimg.height,dk_x*32,dk_y*32);
    }
    else if(dk_x==21 && dk_y==5 || dk_x==11 && dk_y==11 || dk_x==0 && dk_y==8 || dk_x==5 && dk_y==3 ||dk_x==15 && dk_y==10)
    {
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==11 && dk_y==20 || dk_x==6 && dk_y==10 || dk_x==4 && dk_y==6 || dk_x==11 && dk_y==5 ||dk_x==3 &&dk_x==7)
    {
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==7 && dk_y==15 || dk_x==2 && dk_y==10 || dk_x==2 && dk_y==5 || dk_x==9 && dk_y==2 || dk_x==29 &&dk_y==29 )
    {
        myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(dk_x)*32,(dk_y)*32);
    }
    else{
        myDrawImage(sandimg.pixel_data,sandimg.width,sandimg.height,dk_x*32,dk_y*32);
    }
    dk_y=dk_y+1;
    if(dk_x>=0&& dk_y>=0 && dk_x<32&& dk_y<24)
    {
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
        }
    else
    {
        dk_y=23;
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
}

//draws the obstacles for stage two 
void draw_obstaclesstagetwo()
{
    obs_arraytwo[0].x= 8;
    obs_arraytwo[0].y= 22;
    obs_arraytwo[1].x= 21;
    obs_arraytwo[1].y= 5;
    obs_arraytwo[2].x=11;
    obs_arraytwo[2].y=20;
    obs_arraytwo[3].x=7;
    obs_arraytwo[3].y=15;
    obs_arraytwo[4].x=0;
    obs_arraytwo[4].y=13;
    obs_arraytwo[5].x= 11;
    obs_arraytwo[5].y=11;
    obs_arraytwo[6].x=6;
    obs_arraytwo[6].y=10;
    obs_arraytwo[7].x=2;
    obs_arraytwo[7].y=10;
    obs_arraytwo[8].x=8;
    obs_arraytwo[8].y=8;
    obs_arraytwo[9].x=0;
    obs_arraytwo[9].y=8;
    obs_arraytwo[10].x=4;
    obs_arraytwo[10].y=6;
    obs_arraytwo[11].x=2;
    obs_arraytwo[11].y=5;
    obs_arraytwo[12].x=5;   
    obs_arraytwo[12].y=9;
    obs_arraytwo[13].x=5;
    obs_arraytwo[13].y=3;
    obs_arraytwo[14].x=11;
    obs_arraytwo[14].y=5;
    obs_arraytwo[15].x=9;
    obs_arraytwo[15].y=2;
    obs_arraytwo[16].x=23;
    obs_arraytwo[16].y=14;
    obs_arraytwo[17].x=15;
    obs_arraytwo[17].y=10;
    obs_arraytwo[18].x=3;
    obs_arraytwo[18].y=7;
    obs_arraytwo[19].x=29;
    obs_arraytwo[19].y=29;
    for(int i=0;i<20;i=i+4)
    {
        myDrawImage(pumpkinimg.pixel_data,pumpkinimg.width,pumpkinimg.height,(obs_arraytwo[i].x)*32,(obs_arraytwo[i].y)*32);
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(obs_arraytwo[i+1].x)*32,(obs_arraytwo[i+1].y)*32);
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(obs_arraytwo[i+2].x)*32,(obs_arraytwo[i+2].y)*32);
        myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(obs_arraytwo[i+3].x)*32,(obs_arraytwo[i+3].y)*32);
    }
}

//detects the collision of dk and obstacles for stage two 
void detect_collisionstagetwo()
{
    for(int i=0;i<20;i++)
    {
        if(dk_x==obs_array[i].x && dk_y==obs_array[i].y)
        {
            uart_puts("collided");
        }
    }
}

//draws the coins on screen for stage two 
void drawCoinsstagetwo()
{
    for (int i=0,j=0;i<9 && j<9;i++,j++)
    {
        myDrawImage(coin.pixel_data,coin.width,coin.height,coinx[i]*32,coiny[j]*32);
    }
}

//calculates the scores for stage two 
void calculate_scoresstagetwo()
{
    for(int i=0,j=0;i<9 && j<9 ; i++,j++)
    {
        if (dk_x==coinx[i] && dk_y==coiny[j])
        {
            if(coinx[i]!=0 && coiny[j]!=0)
            {
                scores=scores+100;
                coinx[i]=0;
                coiny[j]=0;
                extract_digitsstagetwo();
            }
        }
    }
}
    
//intializes the x_score and y_score coordinates 
int x_score;
int y_score;

//counts the digits of the score 
int count_digitsstagetwo(int n)
{
    int count = 0;
    do {
        n=n/10;
        ++count;
    }
    while (n != 0);
    return count;
}

int c;
//extracts the digits of the score calculated 
void extract_digitsstagetwo()
{
    int num =scores;
    c = count_digitsstagetwo(scores);
    int power=1;
    while(num!=0)
    {
        int digits=num%10;
        drawScoreImagestagetwo(digits);
        num=num/10;
    }
}

//displays the score as the game goes on 
void drawScoreImagestagetwo(int d)
{
    switch(d)
    {
        case 0:
        myDrawImage(zero.pixel_data,zero.width,zero.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 1:
        myDrawImage(oneimg.pixel_data,oneimg.width,oneimg.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 2:
        myDrawImage(two.pixel_data,two.width,two.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 3:
        myDrawImage(three.pixel_data,three.width,three.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 4:
        myDrawImage(four.pixel_data,four.width,four.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 5:
        myDrawImage(five.pixel_data,five.width,five.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 6:
        myDrawImage(six.pixel_data,six.width,six.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 7:
        myDrawImage(sevenimg.pixel_data,sevenimg.width,sevenimg.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 8:
        myDrawImage(eight.pixel_data,eight.width,eight.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 9:
        myDrawImage(nineimg.pixel_data,nineimg.width,nineimg.height,(c+1)*32,y_score*32);
        c--;
        break;
    }
}

//checks for collision of dk and obstacle 
void check_collisionstagetwo()
{
    for(int i=0;i<20;i++)
    {
        if(dk_x==obs_arraytwo[i].x && dk_y==obs_arraytwo[i].y);         //if dk collides with an obstacle 
        {
            lives=lives-1;                                              //then it decreases the lives by 1
           
        }
    }
}
\//checks the win condition for stage two 
void checkwinstagetwo()
{
    if(dk_x==30 && dk_y==1)
    {
        myDrawImage(winimg.pixel_data,winimg.width,winimg.height,11*32,12*32);
        winflag=1;
    }
   
}

//checks the lose condition for stage two 
void checkLosestagetwo()
{
    for(int i=0;i<20;i++)
    {
        if(dk_x==obs_array[i].x &&dk_y==obs_array[i].y)
        {
            lives=lives-1;
            if(lives>=1 && l>=14 && l<=16)
            {
                myDrawImage(sandimg.pixel_data,tile.width,tile.height,l*32,0);
                l = l-1;
            }
            else{
                myDrawImage(sandimg.pixel_data,sandimg.width,sandimg.height,l*32,0);
                myDrawImage(lostimg.pixel_data,lostimg.width,lostimg.height,11*32,12*32);
                loseflag=1;
            }
        }
    }
}



//************************************STAGE THREE************************************************//


//defines the structure for obstacles for stage three
struct obstacle obs_arraythree[20];

//draws the main screen background 
void gamescreenstagethree()
{
//draw tiles for background 
for(int i=0;i<24;i++) // rows
{
    for(int j=0;j<32;j++) // cols 
    {
        int offx = 32 * j;
        int offy = 32 * i;
        myDrawImage(sandimg.pixel_data,sandimg.width,sandimg.height,offx,offy);
    }
}
myDrawImage(dk.pixel_data,dk.width,dk.height,0,23*32);
} 
//dk when moving right 
void moveRightstagethree(){
    if(dk_x==8 && dk_y==22 || dk_x==0 && dk_y==13 || dk_x==8 && dk_y==8 || dk_x==5 && dk_y==9 || dk_x==23 &&dk_y==14)
    {
        myDrawImage(pumpkinimg.pixel_data,pumpkinimg.width,pumpkinimg.height,dk_x*32,dk_y*32);
    }
    else if(dk_x==21 && dk_y==5 || dk_x==11 && dk_y==11 || dk_x==0 && dk_y==8 || dk_x==5 && dk_y==3 ||dk_x==15 && dk_y==10)
    {
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==11 && dk_y==20 || dk_x==6 && dk_y==10 || dk_x==4 && dk_y==6 || dk_x==11 && dk_y==5 ||dk_x==3 &&dk_x==7)
    {
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==7 && dk_y==15 || dk_x==2 && dk_y==10 || dk_x==2 && dk_y==5 || dk_x==9 && dk_y==2 || dk_x==29 &&dk_y==29)
    {
        myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(dk_x)*32,(dk_y)*32);
    }
    else{
        myDrawImage(sandimg.pixel_data,sandimg.width,sandimg.height,dk_x*32,dk_y*32);
        }
    dk_x=dk_x+1;
    if(dk_x>=0&& dk_y>=0&& dk_x<32&& dk_y<24)
    {
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
    else
    {
        dk_x=31;
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
}

//dk when moving left
void moveLeftstagethree(){
    if(dk_x==8 && dk_y==22 || dk_x==0 && dk_y==13 || dk_x==8 && dk_y==8 || dk_x==5 && dk_y==9 || dk_x==23 &&dk_y==14)
    {
        myDrawImage(pumpkinimg.pixel_data,pumpkinimg.width,pumpkinimg.height,dk_x*32,dk_y*32);
    }
    else if(dk_x==21 && dk_y==5 || dk_x==11 && dk_y==11 || dk_x==0 && dk_y==8 || dk_x==5 && dk_y==3 ||dk_x==15 && dk_y==10)
    {
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==11 && dk_y==20 || dk_x==6 && dk_y==10 || dk_x==4 && dk_y==6 || dk_x==11 && dk_y==5 ||dk_x==3 &&dk_x==7)
    {
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==7 && dk_y==15 || dk_x==2 && dk_y==10 || dk_x==2 && dk_y==5 || dk_x==9 && dk_y==2 || dk_x==29 &&dk_y==29 )
    {
        myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(dk_x)*32,(dk_y)*32);
    }
    else{
        myDrawImage(sandimg.pixel_data,sandimg.width,sandimg.height,dk_x*32,dk_y*32);
    }
    dk_x=dk_x-1;
    dk_y=dk_y;
    if(dk_x>=0&& dk_y>=0 && dk_x<32&& dk_y<24)
    {
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
    else
    {
        dk_x=0;
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
}

//dk when moving up
void moveUpstagethree(){
    if(dk_x==8 && dk_y==22 || dk_x==0 && dk_y==13 || dk_x==8 && dk_y==8 || dk_x==5 && dk_y==9 || dk_x==23 &&dk_y==14)
    {
        myDrawImage(pumpkinimg.pixel_data,pumpkinimg.width,pumpkinimg.height,dk_x*32,dk_y*32);
    }
    else if(dk_x==21 && dk_y==5 || dk_x==11 && dk_y==11 || dk_x==0 && dk_y==8 || dk_x==5 && dk_y==3 ||dk_x==15 && dk_y==10)
    {
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==11 && dk_y==20 || dk_x==6 && dk_y==10 || dk_x==4 && dk_y==6 || dk_x==11 && dk_y==5 ||dk_x==3 &&dk_x==7)
    {
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==7 && dk_y==15 || dk_x==2 && dk_y==10 || dk_x==2 && dk_y==5 || dk_x==9 && dk_y==2 || dk_x==29 &&dk_y==29)
    {
      myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(dk_x)*32,(dk_y)*32);
    }
    else{
        myDrawImage(sandimg.pixel_data,sandimg.width,sandimg.height,dk_x*32,dk_y*32);
    }
    dk_x=dk_x;
    dk_y=dk_y-1;
    if(dk_x>=0&& dk_y>=0 && dk_x<32&& dk_y<24)
    {
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
    else
    {
        dk_y=0;
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
}

//dk when moving down 
void moveDownstagethree(){   
    if(dk_x==8 && dk_y==22 || dk_x==0 && dk_y==13 || dk_x==8 && dk_y==8 || dk_x==5 && dk_y==9 || dk_x==23 &&dk_y==14)
    {
        myDrawImage(pumpkinimg.pixel_data,pumpkinimg.width,pumpkinimg.height,dk_x*32,dk_y*32);
    }
    else if(dk_x==21 && dk_y==5 || dk_x==11 && dk_y==11 || dk_x==0 && dk_y==8 || dk_x==5 && dk_y==3 ||dk_x==15 && dk_y==10)
    {
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==11 && dk_y==20 || dk_x==6 && dk_y==10 || dk_x==4 && dk_y==6 || dk_x==11 && dk_y==5 ||dk_x==3 &&dk_x==7)
    {
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==7 && dk_y==15 || dk_x==2 && dk_y==10 || dk_x==2 && dk_y==5 || dk_x==9 && dk_y==2 || dk_x==29 &&dk_y==29 )
    {
        myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(dk_x)*32,(dk_y)*32);
    }
    else{
        myDrawImage(sandimg.pixel_data,sandimg.width,sandimg.height,dk_x*32,dk_y*32);
    }
    dk_y=dk_y+1;
    if(dk_x>=0&& dk_y>=0 && dk_x<32&& dk_y<24)
    {
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
        }
    else
    {
        dk_y=23;
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
}


//draws the obstacles for stage three 
void draw_obstaclesstagethree()
{
    obs_arraythree[0].x= 8;
    obs_arraythree[0].y= 22;
    obs_arraythree[1].x= 21;
    obs_arraythree[1].y= 5;
    obs_arraythree[2].x=11;
    obs_arraythree[2].y=20;
    obs_arraythree[3].x=7;
    obs_arraythree[3].y=15;
    obs_arraythree[4].x=0;
    obs_arraythree[4].y=13;
    obs_arraythree[5].x= 11;
    obs_arraythree[5].y=11;
    obs_arraythree[6].x=6;
    obs_arraythree[6].y=10;
    obs_arraythree[7].x=2;
    obs_arraythree[7].y=10;
    obs_arraythree[8].x=8;
    obs_arraythree[8].y=8;
    obs_arraythree[9].x=0;
    obs_arraythree[9].y=8;
    obs_arraythree[10].x=4;
    obs_arraythree[10].y=6;
    obs_arraythree[11].x=2;
    obs_arraythree[11].y=5;
    obs_arraythree[12].x=5;   
    obs_arraythree[12].y=9;
    obs_arraythree[13].x=5;
    obs_arraythree[13].y=3;
    obs_arraythree[14].x=11;
    obs_arraythree[14].y=5;
    obs_arraythree[15].x=9;
    obs_arraythree[15].y=2;
    obs_arraythree[16].x=23;
    obs_arraythree[16].y=14;
    obs_arraythree[17].x=15;
    obs_arraythree[17].y=10;
    obs_arraythree[18].x=3;
    obs_arraythree[18].y=7;
    obs_arraythree[19].x=29;
    obs_arraythree[19].y=29;
    for(int i=0;i<20;i=i+4)
    {
        myDrawImage(pumpkinimg.pixel_data,pumpkinimg.width,pumpkinimg.height,(obs_arraythree[i].x)*32,(obs_arraythree[i].y)*32);
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(obs_arraythree[i+1].x)*32,(obs_arraythree[i+1].y)*32);
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(obs_arraythree[i+2].x)*32,(obs_arraythree[i+2].y)*32);
        myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(obs_arraythree[i+3].x)*32,(obs_arraythree[i+3].y)*32);
    }
}

//detects the collision of dk and obstacles for stage three
void detect_collisionstagethree()
{
    for(int i=0;i<20;i++)
    {
        if(dk_x==obs_array[i].x && dk_y==obs_array[i].y)
        {
            uart_puts("collided");
        }
    }
}

//draws the coins on screen for stage three
void drawCoinsstagethree()
{
    for (int i=0,j=0;i<9 && j<9;i++,j++)
    {
        myDrawImage(coin.pixel_data,coin.width,coin.height,coinx[i]*32,coiny[j]*32);
    }
}

//calculates the scores for stage two 
void calculate_scoresstagethree()
{
    for(int i=0,j=0;i<9 && j<9 ; i++,j++)
    {
        if (dk_x==coinx[i] && dk_y==coiny[j])
        {
            if(coinx[i]!=0 && coiny[j]!=0)
            {
                scores=scores+100;
                coinx[i]=0;
                coiny[j]=0;
                extract_digitsstagethree();
            }
        }
    }
}
    
//intializes the x_score and y_score coordinates 
int x_score;
int y_score;

//counts the digits of the score 
int count_digitsstagethree(int n)
{
    int count = 0;
    do {
        n=n/10;
        ++count;
    }
    while (n != 0);
    return count;
}

int c;
//extracts the digits of the score calculated 
void extract_digitsstagethree()
{
    int num =scores;
    c = count_digitsstagethree(scores);
    int power=1;
    while(num!=0)
    {
        int digits=num%10;
        drawScoreImagestagethree(digits);
        num=num/10;
    }
}

//displays the score as the game goes on 
void drawScoreImagestagethree(int d)
{
    switch(d)
    {
        case 0:
        myDrawImage(zero.pixel_data,zero.width,zero.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 1:
        myDrawImage(oneimg.pixel_data,oneimg.width,oneimg.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 2:
        myDrawImage(two.pixel_data,two.width,two.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 3:
        myDrawImage(three.pixel_data,three.width,three.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 4:
        myDrawImage(four.pixel_data,four.width,four.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 5:
        myDrawImage(five.pixel_data,five.width,five.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 6:
        myDrawImage(six.pixel_data,six.width,six.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 7:
        myDrawImage(sevenimg.pixel_data,sevenimg.width,sevenimg.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 8:
        myDrawImage(eight.pixel_data,eight.width,eight.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 9:
        myDrawImage(nineimg.pixel_data,nineimg.width,nineimg.height,(c+1)*32,y_score*32);
        c--;
        break;
    }
}

//checks for collision of dk and obstacle 
void check_collisionstagethree()
{
    for(int i=0;i<20;i++)
    {
        if(dk_x==obs_arraythree[i].x && dk_y==obs_arraythree[i].y);         //if dk collides with an obstacle 
        {
            lives=lives-1;                                              //then it decreases the lives by 1
           
        }
    }
}
\//checks the win condition for stage two 
void checkwinstagethree()
{
    if(dk_x==30 && dk_y==1)
    {
        myDrawImage(winimg.pixel_data,winimg.width,winimg.height,11*32,12*32);
        winflag=1;
    }
   
}

//checks the lose condition for stage two 
void checkLosestagethree()
{
    for(int i=0;i<20;i++)
    {
        if(dk_x==obs_array[i].x &&dk_y==obs_array[i].y)
        {
            lives=lives-1;
            if(lives>=1 && l>=14 && l<=16)
            {
                myDrawImage(sandimg.pixel_data,tile.width,tile.height,l*32,0);
                l = l-1;
            }
            else{
                myDrawImage(sandimg.pixel_data,sandimg.width,sandimg.height,l*32,0);
                myDrawImage(lostimg.pixel_data,lostimg.width,lostimg.height,11*32,12*32);
                loseflag=1;
            }
        }
    }
}





//**************************************STAGE FOUR**********************************************//




//defines the structure for obstacles for stage four 
struct obstacle obs_arrayfour[20];

//draws the main screen background 
void gamescreenstagefour()
{
//draw tiles for background 
for(int i=0;i<24;i++) // rows
{
    for(int j=0;j<32;j++) // cols 
    {
        int offx = 32 * j;
        int offy = 32 * i;
        myDrawImage(sandimg.pixel_data,sandimg.width,sandimg.height,offx,offy);
    }
}
myDrawImage(dk.pixel_data,dk.width,dk.height,0,23*32);
} 

//dk when moving right 
void moveRightstagefour(){
    if(dk_x==8 && dk_y==22 || dk_x==0 && dk_y==13 || dk_x==8 && dk_y==8 || dk_x==5 && dk_y==9 || dk_x==23 &&dk_y==14)
    {
        myDrawImage(pumpkinimg.pixel_data,pumpkinimg.width,pumpkinimg.height,dk_x*32,dk_y*32);
    }
    else if(dk_x==21 && dk_y==5 || dk_x==11 && dk_y==11 || dk_x==0 && dk_y==8 || dk_x==5 && dk_y==3 ||dk_x==15 && dk_y==10)
    {
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==11 && dk_y==20 || dk_x==6 && dk_y==10 || dk_x==4 && dk_y==6 || dk_x==11 && dk_y==5 ||dk_x==3 &&dk_x==7)
    {
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==7 && dk_y==15 || dk_x==2 && dk_y==10 || dk_x==2 && dk_y==5 || dk_x==9 && dk_y==2 || dk_x==29 &&dk_y==29)
    {
        myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(dk_x)*32,(dk_y)*32);
    }
    else{
        myDrawImage(sandimg.pixel_data,sandimg.width,sandimg.height,dk_x*32,dk_y*32);
        }
    dk_x=dk_x+1;
    if(dk_x>=0&& dk_y>=0&& dk_x<32&& dk_y<24)
    {
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
    else
    {
        dk_x=31;
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
}

//dk when moving left
void moveLeftstagefour(){
    if(dk_x==8 && dk_y==22 || dk_x==0 && dk_y==13 || dk_x==8 && dk_y==8 || dk_x==5 && dk_y==9 || dk_x==23 &&dk_y==14)
    {
        myDrawImage(pumpkinimg.pixel_data,pumpkinimg.width,pumpkinimg.height,dk_x*32,dk_y*32);
    }
    else if(dk_x==21 && dk_y==5 || dk_x==11 && dk_y==11 || dk_x==0 && dk_y==8 || dk_x==5 && dk_y==3 ||dk_x==15 && dk_y==10)
    {
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==11 && dk_y==20 || dk_x==6 && dk_y==10 || dk_x==4 && dk_y==6 || dk_x==11 && dk_y==5 ||dk_x==3 &&dk_x==7)
    {
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==7 && dk_y==15 || dk_x==2 && dk_y==10 || dk_x==2 && dk_y==5 || dk_x==9 && dk_y==2 || dk_x==29 &&dk_y==29 )
    {
        myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(dk_x)*32,(dk_y)*32);
    }
    else{
        myDrawImage(sandimg.pixel_data,sandimg.width,sandimg.height,dk_x*32,dk_y*32);
    }
    dk_x=dk_x-1;
    dk_y=dk_y;
    if(dk_x>=0&& dk_y>=0 && dk_x<32&& dk_y<24)
    {
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
    else
    {
        dk_x=0;
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
}

//dk when moving up
void moveUpstagefour(){
    if(dk_x==8 && dk_y==22 || dk_x==0 && dk_y==13 || dk_x==8 && dk_y==8 || dk_x==5 && dk_y==9 || dk_x==23 &&dk_y==14)
    {
        myDrawImage(pumpkinimg.pixel_data,pumpkinimg.width,pumpkinimg.height,dk_x*32,dk_y*32);
    }
    else if(dk_x==21 && dk_y==5 || dk_x==11 && dk_y==11 || dk_x==0 && dk_y==8 || dk_x==5 && dk_y==3 ||dk_x==15 && dk_y==10)
    {
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==11 && dk_y==20 || dk_x==6 && dk_y==10 || dk_x==4 && dk_y==6 || dk_x==11 && dk_y==5 ||dk_x==3 &&dk_x==7)
    {
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==7 && dk_y==15 || dk_x==2 && dk_y==10 || dk_x==2 && dk_y==5 || dk_x==9 && dk_y==2 || dk_x==29 &&dk_y==29)
    {
      myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(dk_x)*32,(dk_y)*32);
    }
    else{
        myDrawImage(sandimg.pixel_data,sandimg.width,sandimg.height,dk_x*32,dk_y*32);
    }
    dk_x=dk_x;
    dk_y=dk_y-1;
    if(dk_x>=0&& dk_y>=0 && dk_x<32&& dk_y<24)
    {
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
    else
    {
        dk_y=0;
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
}

//dk when moving down 
void moveDownstagefour(){   
    if(dk_x==8 && dk_y==22 || dk_x==0 && dk_y==13 || dk_x==8 && dk_y==8 || dk_x==5 && dk_y==9 || dk_x==23 &&dk_y==14)
    {
        myDrawImage(pumpkinimg.pixel_data,pumpkinimg.width,pumpkinimg.height,dk_x*32,dk_y*32);
    }
    else if(dk_x==21 && dk_y==5 || dk_x==11 && dk_y==11 || dk_x==0 && dk_y==8 || dk_x==5 && dk_y==3 ||dk_x==15 && dk_y==10)
    {
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==11 && dk_y==20 || dk_x==6 && dk_y==10 || dk_x==4 && dk_y==6 || dk_x==11 && dk_y==5 ||dk_x==3 &&dk_x==7)
    {
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(dk_x)*32,(dk_y)*32);
    }
    else if(dk_x==7 && dk_y==15 || dk_x==2 && dk_y==10 || dk_x==2 && dk_y==5 || dk_x==9 && dk_y==2 || dk_x==29 &&dk_y==29 )
    {
        myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(dk_x)*32,(dk_y)*32);
    }
    else{
        myDrawImage(sandimg.pixel_data,sandimg.width,sandimg.height,dk_x*32,dk_y*32);
    }
    dk_y=dk_y+1;
    if(dk_x>=0&& dk_y>=0 && dk_x<32&& dk_y<24)
    {
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
        }
    else
    {
        dk_y=23;
        myDrawImage(dk.pixel_data,dk.width,dk.height,dk_x*32,dk_y*32);
    }
}

//draws the obstacles for stage four
void draw_obstaclesstagefour()
{
    obs_arrayfour[0].x= 8;
    obs_arrayfour[0].y= 22;
    obs_arrayfour[1].x= 21;
    obs_arrayfour[1].y= 5;
    obs_arrayfour[2].x=11;
    obs_arrayfour[2].y=20;
    obs_arrayfour[3].x=7;
    obs_arrayfour[3].y=15;
    obs_arrayfour[4].x=0;
    obs_arrayfour[4].y=13;
    obs_arrayfour[5].x= 11;
    obs_arrayfour[5].y=11;
    obs_arrayfour[6].x=6;
    obs_arrayfour[6].y=10;
    obs_arrayfour[7].x=2;
    obs_arrayfour[7].y=10;
    obs_arrayfour[8].x=8;
    obs_arrayfour[8].y=8;
    obs_arrayfour[9].x=0;
    obs_arrayfour[9].y=8;
    obs_arrayfour[10].x=4;
    obs_arrayfour[10].y=6;
    obs_arrayfour[11].x=2;
    obs_arrayfour[11].y=5;
    obs_arrayfour[12].x=5;   
    obs_arrayfour[12].y=9;
    obs_arrayfour[13].x=5;
    obs_arrayfour[13].y=3;
    obs_arrayfour[14].x=11;
    obs_arrayfour[14].y=5;
    obs_arrayfour[15].x=9;
    obs_arrayfour[15].y=2;
    obs_arrayfour[16].x=23;
    obs_arrayfour[16].y=14;
    obs_arrayfour[17].x=15;
    obs_arrayfour[17].y=10;
    obs_arrayfour[18].x=3;
    obs_arrayfour[18].y=7;
    obs_arrayfour[19].x=29;
    obs_arrayfour[19].y=29;
    for(int i=0;i<20;i=i+4)
    {
        myDrawImage(pumpkinimg.pixel_data,pumpkinimg.width,pumpkinimg.height,(obs_arrayfour[i].x)*32,(obs_arrayfour[i].y)*32);
        myDrawImage(snakeimg.pixel_data,snakeimg.width,snakeimg.height,(obs_arrayfour[i+1].x)*32,(obs_arrayfour[i+1].y)*32);
        myDrawImage(rockimg.pixel_data,rockimg.width,rockimg.height,(obs_arrayfour[i+2].x)*32,(obs_arrayfour[i+2].y)*32);
        myDrawImage(wallimg.pixel_data,wallimg.width,wallimg.height,(obs_arrayfour[i+3].x)*32,(obs_arrayfour[i+3].y)*32);
    }
}

//detects the collision of dk and obstacles for stage four
void detect_collisionstagefour()
{
    for(int i=0;i<20;i++)
    {
        if(dk_x==obs_array[i].x && dk_y==obs_array[i].y)
        {
            uart_puts("collided");
        }
    }
}

//draws the coins on screen for stage four
void drawCoinsstagefour()
{
    for (int i=0,j=0;i<9 && j<9;i++,j++)
    {
        myDrawImage(coin.pixel_data,coin.width,coin.height,coinx[i]*32,coiny[j]*32);
    }
}

//calculates the scores for stage four
void calculate_scoresstagefour()
{
    for(int i=0,j=0;i<9 && j<9 ; i++,j++)
    {
        if (dk_x==coinx[i] && dk_y==coiny[j])
        {
            if(coinx[i]!=0 && coiny[j]!=0)
            {
                scores=scores+100;
                coinx[i]=0;
                coiny[j]=0;
                extract_digitsstagefour();
            }
        }
    }
}
    
//intializes the x_score and y_score coordinates 
int x_score;
int y_score;

//counts the digits of the score 
int count_digitsstagefour(int n)
{
    int count = 0;
    do {
        n=n/10;
        ++count;
    }
    while (n != 0);
    return count;
}

int c;
//extracts the digits of the score calculated 
void extract_digitsstagefour()
{
    int num =scores;
    c = count_digitsstagefour(scores);
    int power=1;
    while(num!=0)
    {
        int digits=num%10;
        drawScoreImagestagefour(digits);
        num=num/10;
    }
}

//displays the score as the game goes on 
void drawScoreImagestagefour(int d)
{
    switch(d)
    {
        case 0:
        myDrawImage(zero.pixel_data,zero.width,zero.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 1:
        myDrawImage(oneimg.pixel_data,oneimg.width,oneimg.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 2:
        myDrawImage(two.pixel_data,two.width,two.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 3:
        myDrawImage(three.pixel_data,three.width,three.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 4:
        myDrawImage(four.pixel_data,four.width,four.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 5:
        myDrawImage(five.pixel_data,five.width,five.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 6:
        myDrawImage(six.pixel_data,six.width,six.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 7:
        myDrawImage(sevenimg.pixel_data,sevenimg.width,sevenimg.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 8:
        myDrawImage(eight.pixel_data,eight.width,eight.height,(c+1)*32,y_score*32);
        c--;
        break;

        case 9:
        myDrawImage(nineimg.pixel_data,nineimg.width,nineimg.height,(c+1)*32,y_score*32);
        c--;
        break;
    }
}

//checks for collision of dk and obstacle 
void check_collisionstagefour()
{
    for(int i=0;i<20;i++)
    {
        if(dk_x==obs_arrayfour[i].x && dk_y==obs_arrayfour[i].y);         //if dk collides with an obstacle 
        {
            lives=lives-1;                                              //then it decreases the lives by 1
           
        }
    }
}
\//checks the win condition for stage four
void checkwinstagefour()
{
    if(dk_x==30 && dk_y==1)
    {
        myDrawImage(winimg.pixel_data,winimg.width,winimg.height,11*32,12*32);
        winflag=1;
    }
   
}

//checks the lose condition for stage four
void checkLosestagefour()
{
    for(int i=0;i<20;i++)
    {
        if(dk_x==obs_array[i].x &&dk_y==obs_array[i].y)
        {
            lives=lives-1;
            if(lives>=1 && l>=14 && l<=16)
            {
                myDrawImage(sandimg.pixel_data,tile.width,tile.height,l*32,0);
                l = l-1;
            }
            else{
                myDrawImage(sandimg.pixel_data,sandimg.width,sandimg.height,l*32,0);
                myDrawImage(lostimg.pixel_data,lostimg.width,lostimg.height,11*32,12*32);
                loseflag=1;
            }
        }
    }
}


unsigned *clo = (unsigned*) CLO_REG;
unsigned int game_starts_at=*clo;
unsigned int game_ends_at=game_starts_at+(30)*1000000;
int main()
{
    unsigned int current_time;
    void printf(char *str)
    {
        uart_puts(str);
    }
    //initiates GPIO and framebuffer 
    Init_GPIO();
    uart_init();
    fb_init();
   // myDrawImage(startimg.pixel_data,startimg.width,startimg.height,0,0);

   // myDrawImage(quitimg.pixel_data,quitimg.width,quitimg.height,546,500);

    int n=0;
    int startflag=0;
    int endflag=0;
    int start=1;
    int quit=0;
    int buttons[17]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};             //array to store the current state of the buttons
    int prev_state[17]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};          //array to store the previous state of the buttons
    printf("Created by: Deepshikha Dhammi and Kanika Kapila\n");     //prints the creator names
    printf("Please press a button...\n");                            //User input to press a button
    void Print_Message()                                             //prints the message when the button is pressed 
    {
        if(buttons[1]==0){                                           //prints B if button number 1 is pressed
            printf("You have pressed B\n");
            printf("Please press a button...\n");
            }
            else if(buttons[2]==0)                                    //prints Y if button number 2 is pressed
            {
                printf("You have pressed Y\n"); 
                printf("Please press a button...\n");
            }
            else if(buttons[3]==0)                                     //prints Select if button number 3 is pressed
            {
                printf("You have pressed Select\n");
                printf("Please press a button...\n");

            }
            else if(buttons[9]==0)
            {
                drawString(544,448,"You have pressed start",0x0c ); 
                myDrawRect(0,0,1024,768,0,0);
                gamescreen();
                draw_obstacles();
                drawCoins();

                myDrawImage(scoresimg.pixel_data,scoresimg.width,scoresimg.height,0,0);
                myDrawImage(zero.pixel_data,zero.width,zero.height,2*32,0);
                myDrawImage(livesimg.pixel_data,livesimg.width,livesimg.height,11*32,0*32);
                myDrawImage(heartimg.pixel_data,heartimg.width,heartimg.height,13*32,0*32);
                myDrawImage(heartimg.pixel_data,heartimg.width,heartimg.height,14*32,0*32);
                myDrawImage(heartimg.pixel_data,heartimg.width,heartimg.height,15*32,0*32);
                myDrawImage(heartimg.pixel_data,heartimg.width,heartimg.height,16*32,0*32);
                myDrawImage(timerimg.pixel_data,timerimg.width,timerimg.height,21*32,0*32);
                myDrawImage(flagimg.pixel_data,flagimg.width,flagimg.height,30*32,1*32);
                   
                calculate_scores();
                extract_digits();
                start=1;
                quit=0;
                }
            else if(buttons[5]==0)                                //prints Joy-pad UP if button number 5 is pressed
            {
                if(stage == 1)
                {
                    moveUp();
                    calculate_scores();
                    checkwin();
                    checkLose();
                    if(loseflag==1)
                    {
                        fillScreen(0);
                    }
                    if(winflag  == 1)
                    {
                        stage = stage + 1;
                        xoff=0;
                        yoff=0;
                        dk_x=0;
                        dk_y=23;
                        lives=4;
                        l=16;
                        scores=0;
                        winflag=0;
                        loseflag=0;
                    }
                }
                if(stage == 2)
                { 
                    moveUpstagetwo();
                    calculate_scoresstagetwo();
                    checkwinstagetwo();
                    checkLosestagetwo();
                    if(loseflag==1)
                    {
                        fillScreen(0);
                    }
                    if(winflag==1)
                    {
                        stage = stage + 1;
                        xoff=0;
                        yoff=0;
                        dk_x=0;
                        dk_y=23;
                        lives=4;
                        l=16;
                        scores=0;
                        winflag=0;
                        loseflag=0;
                    }
                }
                if(stage == 3){
                    moveUpstagethree();
                    calculate_scoresstagethree();
                    checkwinstagethree();
                    checkLosestagethree();
                    if(loseflag==1){
                        fillScreen(0);
                    }
                    if(winflag==1){
                        stage = stage + 1;
                        xoff=0;
                        yoff=0;
                        dk_x=0;
                        dk_y=23;
                        lives=4;
                        l=16;
                        scores=0;
                        winflag=0;
                        loseflag=0;
                    }
                }
                if(stage == 4){
                    moveUpstagefour();
                    calculate_scoresstagefour();
                    checkwinstagefour();
                    checkLosestagefour();
                    if(loseflag==1){
                        fillScreen(0);
                    }
                    if(winflag==1){
                        stage = stage + 1;
                        xoff=0;
                        yoff=0;
                        dk_x=0;
                        dk_y=23;
                        lives=4;
                        l=16;
                        scores=0;
                        winflag=0;
                        loseflag=0;
                    }
                }
                       printf("You have pressed Joy-pad UP\n");
                       printf("Please press a button...\n");
                }
                 

                else if(buttons[6]==0)                                //prints Joy-pad DOWN if button number 6 is pressed
                {
                    if(stage==1)
                    {
                    moveDown();
                    calculate_scores();
                    checkwin();
                    checkLose();
                     
                     if(loseflag==1)
                     {
                        fillScreen(0);
                     }
                     if(winflag==1)
                     {
                        stage=stage+1;
                        xoff=0;
                        yoff=0;
                        dk_x=0;
                        dk_y=23;
                        lives=4;
                        l=16;
                        scores=0;
                        winflag=0;
                        loseflag=0;
                     }
                    }
                    if(stage==2)
                    { 
                        moveDownstagetwo();
                        calculate_scoresstagetwo();
                        checkwinstagetwo();
                        checkLosestagetwo();
                        if(loseflag==1)
                        {
                            fillScreen(0);
                        }
                        if(winflag=1)
                        {
                            stage=stage+1;
                            xoff=0;
                            yoff=0;
                            dk_x=0;
                            dk_y=23;
                            lives=4;
                            l=16;
                            scores=0;
                            winflag=0;
                            loseflag=0;
                     }
                    }
                    if(stage == 3){
                        moveDownstagethree();
                        calculate_scoresstagethree();
                        checkwinstagethree();
                        checkLosestagethree();
                        if(loseflag==1)
                        {
                            fillScreen(0);
                        }
                        if(winflag=1)
                        {
                            stage=stage+1;
                            xoff=0;
                            yoff=0;
                            dk_x=0;
                            dk_y=23;
                            lives=4;
                            l=16;
                            scores=0;
                            winflag=0;
                            loseflag=0;

                        }
                    }
                    if(stage == 4){
                        moveDownstagefour();
                        calculate_scoresstagefour();
                        checkwinstagefour();
                        checkLosestagefour();
                        if(loseflag==1)
                        {
                            fillScreen(0);
                        }
                        if(winflag=1)
                        {
                            stage=stage+1;
                            xoff=0;
                            yoff=0;
                            dk_x=0;
                            dk_y=23;
                            lives=4;
                            l=16;
                            scores=0;
                            winflag=0;
                            loseflag=0;

                        }

                    }


                       printf("You have pressed Joy-pad DOWN\n");
                       printf("Please press a button...\n");
                }

                else if(buttons[7]==0)                                //prints Joy-pad LEFT if button number 7 is pressed
                {
                    if(stage==1){
                       moveLeft();
                       calculate_scores();
                       checkwin();
                       checkLose();
                       if(loseflag=1)
                       {
                        fillScreen(0);
                       }
                       if(winflag==1)
                        {
                            stage=stage+1;
                            xoff=0;
                            yoff=0;
                            dk_x=0;
                            dk_y=23;
                            lives=4;
                            l=16;
                            scores=0;
                            winflag=0;
                            loseflag=0;
                            }
                    }
                    if(stage==2)
                    { 
                        moveLeftstagetwo();
                        calculate_scoresstagetwo();
                        checkwinstagetwo();
                        checkLosestagetwo();
                        if(loseflag=1)
                        {
                            fillScreen(0);
                        }
                        if(winflag==1)
                        {
                            stage=stage+1;
                            xoff=0;
                            yoff=0;
                            dk_x=0;
                            dk_y=23;
                            lives=4;
                            l=16;
                            scores=0;
                            winflag=0;
                            loseflag=0;
                        }
                    }
                    if(stage == 3) {
                        moveLeftstagethree();
                        calculate_scoresstagethree();
                        checkwinstagethree();
                        checkLosestagethree();
                        if(loseflag=1)
                        {
                            fillScreen(0);
                        }
                        if(winflag==1)
                        {
                            stage=stage+1;
                            xoff=0;
                            yoff=0;
                            dk_x=0;
                            dk_y=23;
                            lives=4;
                            l=16;
                            scores=0;
                            winflag=0;
                            loseflag=0;
                        }


                    }
                    if(stage == 4){
                        moveLeftstagefour();
                        calculate_scoresstagefour();
                        checkwinstagefour();
                        checkLosestagefour();
                        if(loseflag=1)
                        {
                            fillScreen(0);
                        }
                        if(winflag==1)
                        {
                            stage=stage+1;
                            xoff=0;
                            yoff=0;
                            dk_x=0;
                            dk_y=23;
                            lives=4;
                            l=16;
                            scores=0;
                            winflag=0;
                            loseflag=0;
                        }

                    }
                       printf("You have pressed Joy-pad LEFT\n"); 
                       printf("Please press a button...\n");
                }

                else if(buttons[8]==0)                                //prints Joy-pad RIGHT if button number 8 is pressed

                {
                    if(stage==1)
                    {
                        moveRight();
                        calculate_scores();
                        checkwin();
                        checkLose();
                        if(loseflag==1)
                        {
                            fillScreen(0);
                        }
                        if(winflag==1)
                        {
                            stage=stage+1;
                            xoff=0;
                            yoff=0;
                            dk_x=0;
                            dk_y=23;
                            lives=4;
                            l=16;
                            scores=0;
                            winflag=0;
                            loseflag=0;
                        }
                    }
                     if(stage==2)
                    {
                      moveRightstagetwo();
                      calculate_scoresstagetwo();
                      checkwinstagetwo();
                      checkLoosestagetwo();
                      if(loseflag==1)
                      {
                        fillScreen(0);
                      }
                      if(winflag==1)
                        {
                            stage=stage+1;
                            xoff=0;
                            yoff=0;
                            dk_x=0;
                            dk_y=23;
                            lives=4;
                            l=16;
                            scores=0;
                            winflag=0;
                            loseflag=0;
                        }
                    } 
                    if(stage == 3){
                      moveRightstagethree();
                      calculate_scoresstagethree();
                      checkwinstagethree();
                      checkLoosestagethree();
                      if(loseflag==1)
                      {
                        fillScreen(0);
                      }
                      if(winflag==1)
                        {
                            stage=stage+1;
                            xoff=0;
                            yoff=0;
                            dk_x=0;
                            dk_y=23;
                            lives=4;
                            l=16;
                            scores=0;
                            winflag=0;
                            loseflag=0;
                        }

                    }
                    if (stage == 4){
                      moveRightstagefour();
                      calculate_scoresstagefour();
                      checkwinstagefour();
                      checkLoosestagefour();
                      if(loseflag==1)
                      {
                        fillScreen(0);
                      }
                      if(winflag==1)
                        {
                            stage=stage+1;
                            xoff=0;
                            yoff=0;
                            dk_x=0;
                            dk_y=23;
                            lives=4;
                            l=16;
                            scores=0;
                            winflag=0;
                            loseflag=0;
                        }
                    }
                    printf("You have pressed Joy-pad RIGHT\n");
                    printf("Please press a button...\n");

                }

                else if(buttons[4]==0)                               
                {
                       printf("You have pressed start\n");
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
                else if(buttons[12]==0)
                {
                        printf("You have pressed Right\n");
                        printf("Please press a button...\n");
                }
          }
          while(startflag==0 && endflag==0)
          {
            Read_SNES(buttons);    
            current_time = *clo;
            time_left_sec=(game_ends_at-current_time)/1000000; 
            int num =time_left_sec;
            char timer_array[3];
            int k=0;
            while(num!=0)
            {
                int digits=num%10;
                char t=(char)digits+48;
                timer_array[k]=t;
                k++;
                num=num/10;         
            }
            drawString(14*32,0,timer_array,0x0f);
            for(int i=1;i<=16;i++)                                          //loops through every button to check if it is pressed or not
            {
                if(buttons[i]==0 && prev_state[i]==1)
                {
                    Print_Message();                                          //calls Print_Message()
                    prev_state[i]=buttons[i];                                 //store the state of the button that was just pressed
                    copy_array(prev_state,buttons);                           //copy the whole array of current state(buttons) to previous state(prev_state)
                }
                prev_state[i]=buttons[i];
    
            }
        }
    }

     
    
     

   




