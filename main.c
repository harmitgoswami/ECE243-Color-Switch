

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
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define GREY 0xC618
#define PINK 0xFC18
#define ORANGE 0xFC00
	
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
#include <stdbool.h>

// Begin part3.c code for Lab 7


volatile int pixel_buffer_start; // global variable
void wait_for_vsync();
void draw_box(int x, int y, short int colour);
void plot_pixel(int x, int y, short int line_color);
void draw_line(int x1, int y1, int x2, int y2, short int colour);
void clear_screen();
void draw_line_obstacle(int x1, int x2, int x3, int x4, int overflow_length, short int colour, int y);
void draw_circle_obstacle(int color,int centerx,int centery,int r);
bool detect_hit(int player_x, int player_y, short int player_colour);
void draw_colourChangeBox(int y,int clear);
void draw_line(int x1, int y1, int x2, int y2, short int colour);
void draw_starScore(int y, int clear);

int main(void)
{
    int x = 160; // X coordinate of circle center
    int y = 50; // Y coordinate of circle center
    int r = 50;
    int angle = 0;
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	volatile int * sw_ptr = (int *)0xFF200040;
	srand(time(0));
	int i;
	int j;
	int colours[4] = {CYAN, MAGENTA, YELLOW, ORANGE};
	int playerPos[2] = {160, 150};
	int pastPlayerPos[2] = {160, 150};
	int dy = 1;
	int ballColour = WHITE;
	
	//star score block position
	int starScorePos = 90;
	int pstarScorePos = 90;
	int starScoreClear = 0;
	
	//colour change block position
	int colourChangePos = 90;
	int pColourChangePos = 90;
	int colourChangeClear = 0;
	
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
	int y_line_pos = 20;
	
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
		
		//clear player box, update past position
		draw_box(pastPlayerPos[0], pastPlayerPos[1], 0x0);
		pastPlayerPos[1] = playerPos[1];
		
		
		//read input from SW[0] to move player up or down
		int key_pressed = *sw_ptr;
		
		if((key_pressed & 0x01) == 0x1 && count!=0){
			playerPos[1] -= dy*2;
		}else{
			playerPos[1] += dy*3;
		}
		
		//check collision with screen border
		if(detect_hit(playerPos[0], playerPos[1], CYAN)){
			break;
		}
		
		//circle obstacle
		if(count !=0){
			//erase previous circle obstacle positions
			for(i=0; i<4; i++){
		 		draw_box(xp_circle_pos[i], yp_circle_pos[i], 0x0);
		 	}
			
			//detect rotating circle obstacle
			for(i = 0;i<4;i++){
				if(playerPos[0] > x_circle_pos[i] - 5 && playerPos[0] < x_circle_pos[i] + 5 && playerPos[1] > y_circle_pos[i] -5 
				   && playerPos[1] < y_circle_pos[i] + 5 && colours[i]!=ballColour){
					ballColour = GREY;
				}
				if(playerPos[0] > x_line_pos[i] -2 && playerPos[0] < x_line_pos[i] + 82 && playerPos[1] > y_line_pos + 8 
				   && playerPos[1] < y_line_pos + 12 && colours[i]!=ballColour){
					ballColour = RED;
				}
			}
			
			//detect colourChange block collision
			if(playerPos[1] > colourChangePos -5 && playerPos[1] < colourChangePos + 5 && !colourChangeClear){
				ballColour = colours[rand()%((3+1)-0) + 0];
				colourChangeClear = 1;
				draw_colourChangeBox(colourChangePos, 1);	
			}
			
			//detect star block collision
			//if(playerPos[1] > starScorePos -5 && playerPos[1] < starScorePos + 5 && !starScoreClear){
				//ballColour = PINK;
				//starScoreClear = 1;
				//draw_starScore(starScorePos, 1);	
			//}
			
		
		}
		
		//rotating circle obstacle
		for(i=0; i<4; i++){
			xp_circle_pos[i] = x_circle_pos[i];
			yp_circle_pos[i] = y_circle_pos[i];
        }
        for(i=0;i<4;i++){
			
			if(count == 0){
				x_circle_pos[i] = x + r * cos(angle_circle[i] * PI / 180.0);
				y_circle_pos[i] = y + r * sin(angle_circle[i] * PI / 180.0);
			}else{
				x_circle_pos[i] = x + r * cos(angle_circle[i] * PI / 180.0);
				y_circle_pos[i] = y + r * sin(angle_circle[i] * PI / 180.0);
			}
			
			
			angle_circle[i]+=1;
			if (angle_circle[i] >= 360){
				angle_circle[i] -= 360;
			}
		}
		y++;
		
		//draw rotating circle obstacle
		for(i=0;i<4;i++){
			draw_box(x_circle_pos[i], y_circle_pos[i], colours[i]);
		}
		
		//draw player box after checking for collisions
		draw_box(playerPos[0], playerPos[1], ballColour);
		//draw colourChange box after checking for collisions
		
		if(!colourChangeClear){
			//draw_colourChangeBox(colourChangePos, 0);
		}
		if(!starScoreClear){
			draw_starScore(starScorePos, 0);
		}
		
		
   
		//line obstacle
		if(count ==0){
			overflowColour = ORANGE;
		}
		
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
		
		for(i = 20; i < 30; i++){
			draw_line_obstacle(x_line_pos[0],x_line_pos[1],x_line_pos[2],x_line_pos[3], x_line_pos[4], overflowColour, i);
		}
		
		wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
        count++;
    }
	
	
	for(i = 20; i < 40; i++){
		plot_pixel(5, i, CYAN);
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
		plot_pixel(i,y,ORANGE);
	}
}	

void draw_box(int x, int y, short int colour){
	int j;
	int i;
    for(i=0; i<15; i++){
        for(j=0; j<15; j++){
			if(x+i < RESOLUTION_X && y+j < RESOLUTION_Y){ 
				plot_pixel(x+i,y+j,colour);
			}
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

void draw_starScore(int y, int clear){
	if(clear){
		for(int i=0; i<15; i++){
        	for(int j=0; j<15; j++){
            	plot_pixel(160+i,y+j,0x0);
        	}
    	}
	}else{
		for(int i=0; i<15; i++){
        	for(int j=0; j<15; j++){
            	plot_pixel(160+i,y+j,GREY);
        	}
    	}
	}
	
}

void draw_colourChangeBox(int y, int clear){
	if(clear){
		for(int i= 156; i<176;i++){
			for(int j=y-10;j<y+10;j++){
				plot_pixel(i,j,0x0);
			}
		}
	}else{
		for(int i = 156; i<166;i++){
			for(int j = y-10; j < y; j++){
			plot_pixel(i,j,CYAN);
			}
		}
		for(int i = 166; i<176;i++){
			for(int j = y-10; j < y; j++){
				plot_pixel(i,j,YELLOW);
			}
		}
		for(int i = 156; i<166;i++){
			for(int j = y; j < y+10; j++){
				plot_pixel(i,j,ORANGE);
			}
		}
		for(int i = 166; i<176;i++){
			for(int j = y; j < y+10; j++){
				plot_pixel(i,j,MAGENTA);
			}
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

bool detect_hit(int player_x, int player_y, short int player_colour){

	if(player_y < 1 || player_y > RESOLUTION_Y-1){
		return true;
	}
	return false;
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