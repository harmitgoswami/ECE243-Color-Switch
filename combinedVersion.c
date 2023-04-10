
	

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
#define HEX3_HEX0_BASE_ADDR 0xFF200020
#define HEX5_HEX4_BASE_ADDR 0xFF200030
volatile unsigned int *HEX3_HEX0_ptr = (unsigned int *)HEX3_HEX0_BASE_ADDR;
volatile unsigned int *HEX5_HEX4_ptr = (unsigned int *)HEX5_HEX4_BASE_ADDR;
	
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
#include <string.h>
#include <time.h>
#include <math.h>
// Begin part3.c code for Lab 7


volatile int pixel_buffer_start; // global variable
void wait_for_vsync();
void draw_box(int x, int y, short int colour);
void plot_pixel(int x, int y, short int line_color);
void draw_line(int x1, int y1, int x2, int y2, short int colour);
void clear_screen();
void draw_line_obstacle(int x1, int x2, int x3, int x4, int overflow_length, short int colour, int y);
void draw_circle_obstacle(int color,int centerx,int centery,int r);
bool detect_bounds_hit(int player_x, int player_y, short int player_colour);
void draw_colourChangeBox(int y,int clear);
void draw_line(int x1, int y1, int x2, int y2, short int colour);
void draw_starScore(int y, int clear);
void draw_square_box(int x, int y, short int colour);
void draw_full_circle_obstacle(int centerX, int centerY, int r, int playPos[2], int pPlayPos[2], int xCircle_pos[4], 
							   int yCircle_pos[4], int angleCircle[4], 
							   int xpCircle_pos[4], int ypCircle_pos[4], int colours[4], int colour, int count);
void draw_full_line_obstacle(int playPos[2], int pPlayPos[2], int xLine_pos[4], int yLine_pos, 
							 int ypLine_pos[2], int colours[4], int colour, short int overflowColour, int count);
void draw_full_triangle_obstacle(int centerX, int centerY, int r, int playerPos[2], int pPlayerPos[2], int xTriangle_pos[3], 
							   int yTriangle_pos[3], int angleTriangle[3], 
							   int xpTriangle_pos[3], int ypTriangle_pos[3], int colours[4], int colour, int count);
void draw_circle_box(int x, int y, short int colour);
void draw_full_star_block(int y, int clear);
void erase_star_block(int y);
void draw_sprite(int x, int y, int w, int h, uint8_t sprite[]);

    int circleX = 160; // X coordinate of circle center
    int circleY = 0; // Y coordinate of circle center
    int r = 50;
    int angle = 0;
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	volatile int * key_ptr = (int *)0xFF200050;
	volatile int * sw_ptr = (int *)0xFF200040;
	volatile int * hex_ptr = (int *)0xFF20020;

	int i;
	int j;
	int colours[4] = {CYAN, MAGENTA, YELLOW, ORANGE};
	int playerPos[2] = {160, 150};
	int pastPlayerPos[2] = {160, 150};
	int dy = 1;
	int ballColour = WHITE;
	
	//star score block position
	int starScorePos = 10;
	int pstarScorePos = 10;
	int clear = 0;	
	int starScoreCounter = 0;
	unsigned char int_to_seven_seg(unsigned int n) {
    static const unsigned char lookup[10] = {
			0x3F, // 0
			0x06, // 1
			0x5B, // 2
			0x4F, // 3
			0x66, // 4
			0x6D, // 5
			0x7D, // 6
			0x07, // 7
			0x7F, // 8
			0x6F  // 9
		};

    	return lookup[n % 10];
	}
	
	//colour change block position
	int colourChangePos = 90;
	int pColourChangePos = 90;
	int colourChangeClear = 0;
	
	//circle obstacle positions
	int x_circle_pos[4], y_circle_pos[4], angle_circle[4] = {0};
	int xp_circle_pos[4], yp_circle_pos[4] = {0};
	
	
	
	//rotating square obstacle positions
	int x_square = 80; // X coordinate of circle center
    int y_square = 120; // Y coordinate of circle center
    int r_square = 35;
	int x_square_pos[4], y_square_pos[4], angle_square[4] = {0};
	int xp_square_pos[4], yp_square_pos[4] = {0};
	
	
	//rotating triangle obstacle positions
	int x_triangle = 160; // X coordinate of circle center
    int y_triangle = 20; // Y coordinate of circle center
    int r_triangle = 20;
	int x_triangle_pos[3], y_triangle_pos[3], angle_triangle[3] = {0};
	int xp_triangle_pos[3], yp_triangle_pos[3] = {0};
	
	//line obstacle positions
	int x_line_pos[5] = {0,80,160,240, 0};
	int y_line_pos = 20;
	int yp_line_pos[2] = {20, 20};
	int score = 0;

int main(void)
{
	srand(time(0));
	int i,j;
	for(i=0;i<4;i++){
		angle_circle[i] = angle;
		angle+=90;
	}
	angle = 0;
	for(i=0;i<4;i++){
		angle_square[i] = angle;
		angle+=90;
	}
	angle = 0;
	
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
	int drawAllowed = 0;
	int drawNewAllowed = 0;
	int alreadyNewObstacle = 0;
	int drawObstacle;
	int drawOldObstacle;
	int oldObstacleCoords;
	int drawNewObstacle;
	int firstIteration;
	
	clock_t begin = clock();
    while (1){
		/****************************** SCORE DISPLAY ******************************/	
		unsigned int hex3 = int_to_seven_seg(starScoreCounter / 1000);
        unsigned int hex2 = int_to_seven_seg((starScoreCounter % 1000) / 100);
        unsigned int hex1 = int_to_seven_seg((starScoreCounter % 100) / 10);
        unsigned int hex0 = int_to_seven_seg(starScoreCounter % 10);

        // Write the encoded values to the HEX display's memory-mapped I/O addresses
        *HEX3_HEX0_ptr = (hex3 << 24) | (hex2 << 16) | (hex1 << 8) | hex0;
        *HEX5_HEX4_ptr = 0; // Clear HEX5 and HEX4 (if they are present on your board)
		
		/****************************** END OF SCORE DISPLAY ******************************/	
		
/****************************** PLAYER CONTROL ******************************/		
		
		//clear player box, update past position
		draw_box(pastPlayerPos[0], pastPlayerPos[1], 0x0);
		pastPlayerPos[1] = playerPos[1];
		
		
		//read input from SW[0] to move player up or down
		int switch_pressed = *sw_ptr;
		int key_pressed = *key_ptr;
		
		if((switch_pressed & 0x01) == 0x1 && count!=0){
			playerPos[1] -= dy*2;
		}else{
			playerPos[1] += dy*2;
		}
		if((key_pressed & 0x01) == 0x1){
			ballColour = CYAN;
		}
		if((key_pressed & 0x2) == 0x2){
			ballColour = MAGENTA;
		}
		if((key_pressed & 0x4) == 0x4){
			ballColour = YELLOW;
		}
		if((key_pressed & 0x8) == 0x8){
			ballColour = ORANGE;
		}
		
		
		//check collision with screen border
		if(detect_bounds_hit(playerPos[0], playerPos[1], CYAN)){
			break;
		}
		
		
		erase_star_block(pstarScorePos);
		pstarScorePos = starScorePos;
		starScorePos++;
		
		//detect collision
		if(playerPos[1] > starScorePos -5 && playerPos[1] < starScorePos + 5 && !clear){
			clear = 1;
			starScoreCounter++;
			starScorePos = 10;
		}
		
		draw_full_star_block(starScorePos, clear);
		
		clear = 0;
		
		//draw player box after checking for collisions
		draw_box(playerPos[0], playerPos[1], ballColour);

		
/****************************** END OF PLAYER CONTROL ******************************/
		
		if(!drawAllowed) {
			if(count != 0) {
				drawAllowed = 1;
				drawObstacle = rand()%3;
				score++;
			}
		}

if(drawAllowed) {
    switch(drawObstacle) {
        case 0:
            draw_full_circle_obstacle(circleX, circleY, r, playerPos, pastPlayerPos, x_circle_pos, y_circle_pos, angle_circle, xp_circle_pos, yp_circle_pos, colours, ballColour, count);
            if(circleY >= 300) {
                circleY = 0;
                memset(x_circle_pos, 0, 4);
                memset(y_circle_pos, 0, 4);
                memset(xp_circle_pos, 0, 4);
                memset(yp_circle_pos, 0, 4);
                
                drawAllowed = 0;
            }
            break;
        case 1:
            draw_full_line_obstacle(playerPos, pastPlayerPos, x_line_pos, y_line_pos, yp_line_pos, colours, ballColour, overflowColour, count);
            if(yp_line_pos[1] >= 265) {
                yp_line_pos[0] = 0;
                yp_line_pos[1] = 0;
                drawAllowed = 0;
            }
            break;
        case 2:
            draw_full_triangle_obstacle(x_triangle, y_triangle, r, playerPos, pastPlayerPos, x_triangle_pos, y_triangle_pos, angle_triangle, xp_triangle_pos, yp_triangle_pos, colours, ballColour, count);
            if(y_triangle_pos[0] >= 270 && y_triangle_pos[1] >= 270 && y_triangle_pos[2] >= 270) {
                memset(x_triangle_pos, 0, 3);
                memset(y_triangle_pos, 0, 3);
                memset(xp_triangle_pos, 0, 3);
                memset(yp_triangle_pos, 0, 3);
                drawAllowed = 0;
            }
            break;
    }
}
		count++;
		

		
/*	for(i=0; i<4; i++){
		draw_square_box(xp_square_pos[i], yp_square_pos[i], 0x0);
		draw_line(xp_square_pos[i], yp_square_pos[i], xp_square_pos[(i+1)%4], yp_square_pos[(i+1)%4], 0x0000);
	}
		
	for(i=0; i<4; i++){
		xp_square_pos[i] = x_square_pos[i];
		yp_square_pos[i] = y_square_pos[i]; 
    }
		
	for(i=0;i<4;i++){
		x_square_pos[i] = x_square + r_square * cos(angle_square[i] * PI / 180.0);
		y_square_pos[i] = y_square + r_square * sin(angle_square[i] * PI / 180.0);
		angle_square[i]+=1;
		if (angle_square[i] >= 360){
			angle_square[i] -= 360;
		}
	}
	
	for(i=0;i<4;i++){
		draw_square_box(x_square_pos[i], y_square_pos[i], colours[i]);
		draw_line(x_square_pos[i], y_square_pos[i], x_square_pos[(i+1)%4], y_square_pos[(i+1)%4], colours[i]);
	}
		*/
		
/****************************** END OF SQUARE OBSTACLE ******************************/
		
		wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
        
    }
	
}

void draw_full_triangle_obstacle(int centerX, int centerY, int r, int playerPos[2], int pPlayerPos[2], int xTriangle_pos[3], 
							   int yTriangle_pos[3], int angleTriangle[3], 
							   int xpTriangle_pos[3], int ypTriangle_pos[3], int colours[4], int colour, int count){
		int i,j;	
		//erase old triangle (rotation)
		for(i=0;i<3;i++){
			draw_square_box(xpTriangle_pos[i], ypTriangle_pos[i], 0x0);
			draw_line(xpTriangle_pos[i], ypTriangle_pos[i], xpTriangle_pos[(i+1)%3], ypTriangle_pos[(i+1)%3], 0x0000);
		}
		
		//update triangle position (rotation)
		for(int i=0;i<3;i++){
			xpTriangle_pos[i] = xTriangle_pos[i];
			ypTriangle_pos[i] = yTriangle_pos[i];
		}
		
		//calculate angle offsets
		for(i=0;i<3;i++){
			xTriangle_pos[i] = centerX + r * cos((double)angleTriangle[i] * PI / 180.0);
			yTriangle_pos[i] = centerY + r * sin((double)angleTriangle[i] * PI / 180.0);
			angleTriangle[i]+=1;
			if (angleTriangle[i] >= 360){
				angleTriangle[i] -= 360;
			}
		}
		
		//draw new triangle
		for(i=0;i<3;i++){
			draw_square_box(xTriangle_pos[i], yTriangle_pos[i], colours[i]);
			draw_line(xTriangle_pos[i], yTriangle_pos[i], xTriangle_pos[(i+1)%3], yTriangle_pos[(i+1)%3], colours[i]);
		}
		y_triangle = y_triangle + 4;
	
		memcpy(x_triangle_pos, xTriangle_pos, 4);
		memcpy(y_triangle_pos, yTriangle_pos, 4);
		memcpy(xp_triangle_pos, xpTriangle_pos, 4);
		memcpy(yp_triangle_pos, ypTriangle_pos, 4);
		ballColour = colour;
		
}

void draw_full_line_obstacle(int playPos[2], int pPlayPos[2], int xLine_pos[4], int yLine_pos, 
							 int ypLine_pos[2], int colours[4], int colour, short int overflowColour, int count){
	int i,j;
	if(count ==0){
			overflowColour = ORANGE;
		}
		
		for(i = 0;i<4;i++){
				if(playPos[0] > xLine_pos[i] -2 && playPos[0] < xLine_pos[i] + 82 && playPos[1] > ypLine_pos[0] 
				   && playPos[1] < ypLine_pos[0] + 5 && colours[i]!=colour){
					colour = RED;
				}
		}
		
		for(i=0; i<4; i++){
			if(xLine_pos[i]+80 >= 319){
				overflowColour = colours[i];
			}
			if(xLine_pos[i] == 319){
				xLine_pos[i] = 0;
			}
			xLine_pos[i]++;
			
			if(xLine_pos[4] == 80)xLine_pos[4] = 0;
        }
		xLine_pos[4]++;
	
		//erase old line
		for(i = ypLine_pos[0] -4; i < ypLine_pos[0] + 5; i++){
			for(int j=0; j<RESOLUTION_X; j++){
				plot_pixel(j,i,0x0);
			}
		}
	
		//increment new line
		for(i = ypLine_pos[1] ; i < ypLine_pos[1]+5; i++){
			draw_line_obstacle(xLine_pos[0],xLine_pos[1],xLine_pos[2],xLine_pos[3], xLine_pos[4], overflowColour, i);
		}
	
		ypLine_pos[0] = ypLine_pos[1];
		ypLine_pos[1] = ypLine_pos[1] + 4;
	
		memcpy(x_line_pos, xLine_pos, 4);
		memcpy(yp_line_pos, ypLine_pos, 2);
		ballColour = colour;
		
}

void draw_full_circle_obstacle(int centerX, int centerY, int r, int playPos[2], int pPlayPos[2], int xCircle_pos[4], 
							   int yCircle_pos[4], int angleCircle[4], 
							   int xpCircle_pos[4], int ypCircle_pos[4], int colours[4], int colour, int count){
	int i,j;
	if(count !=0){
			//erase previous circle obstacle positions
			for(i=0; i<4; i++){
		 		draw_circle_box(xpCircle_pos[i], ypCircle_pos[i], 0x0);
		 	}
			
			//detect rotating circle obstacle
			for(i = 0;i<4;i++){
				if(playPos[0] > xCircle_pos[i] - 5 && playPos[0] < xCircle_pos[i] + 5 && playPos[1] > yCircle_pos[i] -5 
				   && playPos[1] < yCircle_pos[i] + 5 && colours[i]!=colour){
					colour = GREY;
				}
			}
			
			//detect colourChange block collision
		
			/*	for(int i= 156; i<176;i++){
				for(int j = pColourChangePos-10; j < pColourChangePos+10; j++){
					plot_pixel(i,j,0x0);
				}
			}
			
			
			
			pColourChangePos = colourChangePos;
			colourChangePos++;
			*/
			
			
			
			//detect star block collision
			//if(playerPos[1] > starScorePos -5 && playerPos[1] < starScorePos + 5 && !starScoreClear){
				//ballColour = PINK;
				//starScoreClear = 1;
				//draw_starScore(starScorePos, 1);	
			//}
			
		
		}
		
		for(i=0; i<4; i++){
			xpCircle_pos[i] = xCircle_pos[i];
			ypCircle_pos[i] = yCircle_pos[i];
        }
        for(i=0;i<4;i++){
			
			if(count == 0){
				xCircle_pos[i] = centerX + r * cos((double)angleCircle[i] * PI / 180.0);
				yCircle_pos[i] = centerY + r * sin((double)angleCircle[i] * PI / 180.0);
			}else{
				xCircle_pos[i] = centerX + r * cos((double)angleCircle[i] * PI / 180.0);
				yCircle_pos[i] = centerY + r * sin((double)angleCircle[i] * PI / 180.0);
			}
			
			
			angleCircle[i]+=1;
			if (angleCircle[i] >= 360){
				angleCircle[i] -= 360;
			}
		}
		
		
		//draw rotating circle obstacle
		for(i=0;i<4;i++){
			draw_circle_box(xCircle_pos[i], yCircle_pos[i], colours[i]);
		}
	
		centerY = centerY + 4;
	
		memcpy(x_circle_pos, xCircle_pos, 4);
		memcpy(y_circle_pos, yCircle_pos, 4);
		circleX = centerX;
		circleY = centerY;
		memcpy(xp_circle_pos, xpCircle_pos, 4);
		memcpy(yp_circle_pos, ypCircle_pos, 4);
		ballColour = colour;
		
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

void erase_star_block(int y){
	for(int i=0; i<15; i++){
        	for(int j=0; j<15; j++){
            	plot_pixel(160+i,y+j,0x0);
        }
    }
}

void draw_full_star_block(int y, int clear){
	
	//draw block depending on collision detection
	if(!clear){
		for(int i=0; i<15; i++){
        	for(int j=0; j<15; j++){
            	plot_pixel(160+i,y+j,PINK);
        	}
    	}
	}

}

void draw_box(int x, int y, short int colour){
	int j;
	int i;
    for(i=0; i<15; i++){
        for(j=0; j<15; j++){
			if(y+j>0 && y+j<240){
				plot_pixel(x+i,y+j,colour);
			}
        }
    }
}

void draw_circle_box(int x, int y, short int colour){
	int j;
	int i;
    for(i=0; i<25; i++){
        for(j=0; j<25; j++){
			if(y+j>0 && y+j<240){
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

void draw_square_box(int x, int y, short int colour){
	int j;
	int i;
    for(i=0; i<2; i++){
        for(j=0; j<2; j++){
            if(y+j>0 && y+j<240){
				plot_pixel(x+i,y+j,colour);
			}
        }
    }
}

void plot_pixel(int x, int y, short int line_color){
	
	if(x < RESOLUTION_X && y < RESOLUTION_Y && y>0){
		*(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
	}
 
}

void wait_for_vsync(){

    volatile int * pixel_ctrl_ptr = (int *)0xFF203020; 
    volatile int * status = (int *) 0xFF20302C;

    *pixel_ctrl_ptr = 1;  //write 1 to Buffer to set S == 1

    while((*status & 0x01) != 0){ //read S bit, only move on if S == 0
        int yo = 1;
    }  
    
}

bool detect_bounds_hit(int player_x, int player_y, short int player_colour){

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