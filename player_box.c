

#define PI 3.14159265
/* This files provides address values that exist in the system */

#define SDRAM_BASE            0xC0000000
#define FPGA_ONCHIP_BASE      0xC8000000
#define FPGA_CHAR_BASE        0xC9000000

/* Cyclone V FPGA devices */
#define LEDR_BASE             0xFF200000
#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define SW_BASE               0xFF200040
#define KEY_BASE              0xFF200050
#define TIMER_BASE            0xFF202000
#define PIXEL_BUF_CTRL_BASE   0xFF203020
#define CHAR_BUF_CTRL_BASE    0xFF203030

/* VGA colors */
#define WHITE 0xFFFF
#define YELLOW 0xFFE0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18

#define ABS(x) (((x) > 0) ? (x) : -(x))

/* Screen size. */
#define RESOLUTION_X 320
#define RESOLUTION_Y 240

/* Constants for animation */
#define BOX_LEN 2
#define NUM_BOXES 8

#define FALSE 0
#define TRUE 1

#include <stdlib.h>
#include <stdio.h>

// Begin part3.c code for Lab 7


volatile int pixel_buffer_start; // global variable
void wait_for_vsync();
void draw_box(int x, int y, short int colour);
void plot_pixel(int x, int y, short int line_color);
void draw_line(int x1, int y1, int x2, int y2, short int colour);
void clear_screen();
void draw_line_obstacle(int x1, int x2, int x3, int x4, int overflow_length, short int colour, int y);
void draw_circle_obstacle(int color,int centerx,int centery,int r);


int main(void)
{
    int x = 160; // X coordinate of circle center
    int y = 120; // Y coordinate of circle center
    int r = 50;
    int angle = 0;
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	volatile int * sw_ptr = (int *)0xFF200040;
	int i;
	int j;
	int colours[4] = {CYAN, MAGENTA, YELLOW, PINK};
	int playerPos[2] = {160, 150};
	int pastPlayerPos[2] = {160, 150};
	int dy = 1;
	
	//circle obstacle positions
	int x_circle_pos[4], y_circle_pos[4], angle_circle[4] = {0};
	int xp_circle_pos[4], yp_circle_pos[4] = {0};
	for(i=0;i<4;i++){
		angle_circle[i] = angle;
		angle+=90;
	}
	angle = 0;
	
	//line obstacle positions
	int x_line_pos[5] = {0,80,160,240, 0};
	
    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(); // pixel_buffer_start points to the pixel buffer
	int count = 0;
	short int overflowColour;
    while (1){
		
		draw_box(pastPlayerPos[0], pastPlayerPos[1], 0x0);
		pastPlayerPos[1] = playerPos[1];
		
		int key_pressed = *sw_ptr;
		
		if((key_pressed & 0x01) ==0x1
			&& count!=0){
			playerPos[1] -= dy*2;
		}else{
			playerPos[1] +=dy*3;
		}
		
		
		draw_box(playerPos[0], playerPos[1], CYAN);

		//circle obstacle
		if(count ==0){
			overflowColour = PINK;
		}
		if(count !=0){
			for(i=0; i<4; i++){
		 		draw_box(xp_circle_pos[i], yp_circle_pos[i], 0x0);
		 	}
		}
         for(i=0; i<4; i++){
			xp_circle_pos[i] = x_circle_pos[i];
			yp_circle_pos[i] = y_circle_pos[i];
        }
        for(i=0;i<4;i++){
			x_circle_pos[i] = x + r * cos(angle_circle[i] * PI / 180.0);
			y_circle_pos[i] = y + r * sin(angle_circle[i] * PI / 180.0);
			angle_circle[i]+=1;
			if (angle_circle[i] >= 360){
				angle_circle[i] -= 360;
			}
		}
		
		for(i=0;i<4;i++){
			draw_box(x_circle_pos[i], y_circle_pos[i], colours[i]);
		}
		
		//line obstacle
		
		for(i=0; i<4; i++){
			if(x_line_pos[i]+80 == 319){
				overflowColour = colours[i];
			}
			if(x_line_pos[i] == 319){
				x_line_pos[i] = 0;
			}
			x_line_pos[i]++;
			x_line_pos[4]++;
			if(x_line_pos[4] == 80)x_line_pos[4] = 0;
        }
		for(i = 20; i < 40; i++){
			draw_line_obstacle(x_line_pos[0],x_line_pos[1],x_line_pos[2],x_line_pos[3], x_line_pos[4], overflowColour, i);
		}
		
		
		wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
        count++;
    }
}

void draw_circle_obstacle(int color,int centerx,int centery,int r){
    int x,y;
    float a;
    float pi=3.1415927;
    for(a=0;a<2*pi;a=a+pi/180){
       x=cos(a)*r+centerx;
       y=sin(a)*r+centery;
       plot_pixel(x,y,color);
	}                                                        
}

void draw_line_obstacle(int x1, int x2, int x3, int x4, int overflow_length, short int colour, int y){
	for(int i = 0; i< overflow_length; i++){
		plot_pixel(i,y,colour);
	}
	for(int i = x1; i< x1+ RESOLUTION_X/4 -1; i++){
		plot_pixel(i,y,CYAN);
	}
	for(int i = x2; i< x2+ RESOLUTION_X/4 -1; i++){
		plot_pixel(i,y,MAGENTA);
	}
	for(int i = x3; i< x3 + RESOLUTION_X/4 -1; i++){
		plot_pixel(i,y,YELLOW);
	}
	for(int i = x4; i< x4 + RESOLUTION_X/4 -1; i++){
		plot_pixel(i,y,PINK);
	}
}	

void draw_box(int x, int y, short int colour){
	int j;
	int i;
    for(i=0; i<20; i++){
        for(j=0; j<20; j++){
            plot_pixel(x+i,y+j,colour);
        }
    }
}

void clear_screen(){
	int i;
	int j;
    for(i=0; i<RESOLUTION_X; i++){
        for(j=0; j<RESOLUTION_Y; j++){
            plot_pixel(i,j,0x0000);
        }
    }
}

void plot_pixel(int x, int y, short int line_color){
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void wait_for_vsync(){

    volatile int * pixel_ctrl_ptr = (int *)0xFF203020; 
    volatile int * status = (int *) 0xFF20302C;

    *pixel_ctrl_ptr = 1;  //write 1 to Buffer to set S == 1

    while((*status & 0x01) != 0){ //read S bit, only move on if S == 0
        int yo = 1;
    }  
    
}