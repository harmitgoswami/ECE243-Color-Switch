

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
void draw_square_box(int x, int y, short int colour);

int main(void)
{
    int x = 190; // X coordinate of circle center
    int y = 120; // Y coordinate of circle center
    int r = 50;
    int angle = 0;
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	int i;
	int j;
	int colours[4] = {CYAN, MAGENTA, YELLOW, PINK};
	
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
	
	//rotating square obstacle positions
	int x_square = 80; // X coordinate of circle center
    int y_square = 120; // Y coordinate of circle center
    int r_square = 35;
	int x_square_pos[4], y_square_pos[4], angle_square[4] = {0};
	int xp_square_pos[4], yp_square_pos[4] = {0};
	for(i=0;i<4;i++){
		angle_square[i] = angle;
		angle+=90;
	}
	angle = 0;
	
	//rotating triangle obstacle positions
	int x_triangle = 80; // X coordinate of circle center
    int y_triangle = 120; // Y coordinate of circle center
    int r_triangle = 20;
	int x_triangle_pos[3], y_triangle_pos[3], angle_triangle[3] = {0};
	int xp_triangle_pos[3], yp_triangle_pos[3] = {0};
	for(i=0;i<3;i++){
		angle_triangle[i] = angle;
		angle+=120;
	}
	angle = 0;
	
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
    while (1)
    {
		//circle obstacle and rotating square obstacle
		if(count ==0)overflowColour = PINK;
		if(count !=0){
			for(i=0; i<4; i++){
		 		draw_box(xp_circle_pos[i], yp_circle_pos[i], 0x0);
				draw_square_box(xp_square_pos[i], yp_square_pos[i], 0x0);
				draw_line(xp_square_pos[i], yp_square_pos[i], xp_square_pos[(i+1)%4], yp_square_pos[(i+1)%4], 0x0000);
		 	}
			for(i=0;i<3;i++){
				draw_square_box(xp_triangle_pos[i], yp_triangle_pos[i], 0x0);
				draw_line(xp_triangle_pos[i], yp_triangle_pos[i], xp_triangle_pos[(i+1)%3], yp_triangle_pos[(i+1)%3], 0x0000);
			}
		}
         for(i=0; i<4; i++){
			xp_circle_pos[i] = x_circle_pos[i];
			yp_circle_pos[i] = y_circle_pos[i];
			xp_square_pos[i] = x_square_pos[i];
			yp_square_pos[i] = y_square_pos[i]; 
        }
		for(int i=0;i<3;i++){
			xp_triangle_pos[i] = x_triangle_pos[i];
			yp_triangle_pos[i] = y_triangle_pos[i];
		}
		
        for(i=0;i<4;i++){
			x_circle_pos[i] = x + r * cos(angle_circle[i] * PI / 180.0);
			y_circle_pos[i] = y + r * sin(angle_circle[i] * PI / 180.0);
			angle_circle[i]+=1;
			if (angle_circle[i] >= 360){
				angle_circle[i] -= 360;
			}
			x_square_pos[i] = x_square + r * cos(angle_square[i] * PI / 180.0);
			y_square_pos[i] = y_square + r * sin(angle_square[i] * PI / 180.0);
			angle_square[i]+=1;
			if (angle_square[i] >= 360){
				angle_square[i] -= 360;
			}
		}
		for(i=0;i<3;i++){
			x_triangle_pos[i] = x_triangle + r * cos(angle_triangle[i] * PI / 180.0);
			y_triangle_pos[i] = y_triangle + r * sin(angle_triangle[i] * PI / 180.0);
			angle_triangle[i]+=1;
			if (angle_triangle[i] >= 360){
				angle_triangle[i] -= 360;
			}
		}
		
		for(i=0;i<4;i++){
			draw_box(x_circle_pos[i], y_circle_pos[i], colours[i]);
			draw_square_box(x_square_pos[i], y_square_pos[i], colours[i]);
			draw_line(x_square_pos[i], y_square_pos[i], x_square_pos[(i+1)%4], y_square_pos[(i+1)%4], colours[i]);
		}
		
		for(i=0;i<3;i++){
			draw_square_box(x_triangle_pos[i], y_triangle_pos[i], colours[i]);
			draw_line(x_triangle_pos[i], y_triangle_pos[i], x_triangle_pos[(i+1)%3], y_triangle_pos[(i+1)%3], colours[i]);
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
    for(i=0; i<40; i++){
        for(j=0; j<40; j++){
            plot_pixel(x+i,y+j,colour);
        }
    }
}

void draw_square_box(int x, int y, short int colour){
	int j;
	int i;
    for(i=0; i<2; i++){
        for(j=0; j<2; j++){
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

void draw_line(int x1, int y1, int x2, int y2, short int colour){
	int j;
	int i;

    int isSteep = abs(y2-y1)>abs(x2-x1) ? 1:0;

    if(isSteep == 1){
        int temp = x1;
        x1 = y1;
        y1 = temp;

        temp = x2;
        x2 = y2;
        y2 = temp;
    }

    if(x1 > x2){
        int temp = x1;
        x1 = x2;
        x2 = temp;

        temp = y1;
        y1 = y2;
        y2 = temp;
    }

    int dx = x2-x1;
    int dy = abs(y2-y1);
    int error = -1*(dx/2);
    int y = y1;
    int yShift;

    if(y1<y2){
        yShift = 1;
    }else{
        yShift = -1;
    }

    for(i=x1; i<=x2; i++){
        if(isSteep == 1){
            plot_pixel(y, i, colour);
        }else{
            plot_pixel(i, y, colour);
        }

        error += dy;
        
        if(error>0){
            y += yShift;
            error -= dx;
        }
    }

}