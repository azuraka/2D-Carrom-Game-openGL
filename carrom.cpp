
// ========================== CARROM ================================

// ==================================================================
#include <iostream>
#include <cmath>
#include <string.h>
#include <unistd.h>
#include <GL/glut.h>
using namespace std;
#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)
// ==================================================================

// ==================================================================
// Function Declarations
int checkHole(float x, float y);
void checkStability(float striker_velx, float striker_vely, float coin_velx, float coin_vely);
void checkCollision();
void drawScene();
void update(int value);
void updateAngle(char direction);
void drawBox(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
void drawHollowCircle(float rad);
void drawFilledCircle(float rad);
void drawLine(float x1, float y1, float x2, float y2);
void initRendering();
void handleResize(int w, int h);
void handleKeypress1(unsigned char key, int x, int y);
void handleKeypress2(int key, int x, int y);
void handleMouseclick(int button, int state, int x, int y);
void drawText(char text[], int length, int x, int y);
void timer(int value);
// ==================================================================

// ==================================================================
// Global Variables
float box_len_out = 4.8f;
float box_len_in = 4.0f;
float center_rad = 0.4f;
float striker_rad = 0.10f;
float coin_rad = 0.08f;
float hole_rad = 0.12f;
float power_box_x = box_len_in/2+1.0f;
float power_box_y = -box_len_in/2;
float power_box_len = box_len_in;
float power_box_wid = 0.25f;
float power = 0;
float pow_slot = 0.25f;
float power_reduction = 50;
float striker_bar_velx = 0.10f;
float striker_vel = power/power_reduction;
float striker_velx = striker_vel;
float striker_vely = striker_vel;
float coin_vel = 0.0f;
float coin_velx = 0.0f;
float coin_vely = 0.0f;
float coins_vel[15] = {0.0f};
float coins_velx[15] = {0.0f};
float coins_vely[15] = {0.0f};
float launch_bar_len = box_len_in - 10*hole_rad; // 2*(box_len_in/2+3*hole_rad)
float launch_bar_wid = 1.5 * striker_rad;
float striker_x = 0.0f;
float striker_y = -(launch_bar_len/2+launch_bar_wid/2);
float coin_x = 0.0f;
float coin_y = 0.0f;
float coin_no = 6;
float coins_x[15] = {0.0f};
float coins_y[15] = {0.0f};
float up_pointx = 0.0f;
float up_pointy = striker_rad+0.6f;
float fs_striker = 0.002;
float fs_coin = 0.0045;
float _angle = 0.0f;
float alpha, beta, theta;
// Flags and Counters
float launched = 0;
float hole_flag = 0;
float hole_queen = 0;
float hole_coin[15]={0};
float stable = 0;
int score = 30;
float stable_count;
float initial=0;
int queen_flag=0;
int coins_flag[15]={0};
// ==================================================================

// ==================================================================
int main(int argc, char **argv) {

    // Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	int w = glutGet(GLUT_SCREEN_WIDTH);
	int h = glutGet(GLUT_SCREEN_HEIGHT);
	int windowWidth = w;
	int windowHeight = h;

	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition((w - windowWidth)/2, (h - windowHeight)/2);

    glutCreateWindow("Carrom");  // Setup the window
    initRendering();

    // Register callbacks
    glutDisplayFunc(drawScene);
    glutIdleFunc(drawScene);
    glutKeyboardFunc(handleKeypress1);
    glutSpecialFunc(handleKeypress2);
    glutMouseFunc(handleMouseclick);
    glutReshapeFunc(handleResize);
    glutTimerFunc(1000, timer, 0);


    glutMainLoop();
    return 0;
}
// ==================================================================

// ==================================================================
// Function to draw objects on the screen
void drawScene() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    // Draw Outer Box
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -8.0f);
	glColor3f(0.42f, 0.11f, 0.11f);
	drawBox(-box_len_out/2, -box_len_out/2, box_len_out/2, -box_len_out/2, box_len_out/2, box_len_out/2, -box_len_out/2, box_len_out/2);
	glPopMatrix();

    // Draw Inner Box
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -8.0f);
	glColor3f(2.40f, 2.30f, 1.40f);
	drawBox(-box_len_in/2, -box_len_in/2, box_len_in/2, -box_len_in/2, box_len_in/2, box_len_in/2, -box_len_in/2, box_len_in/2);

    // Draw holes
	float hole_x = -box_len_in/2+hole_rad;
	float hole_y = box_len_in/2-hole_rad;
	for(int i=0;i<4;i++){
		glPushMatrix();
		glTranslatef(hole_x*1.017, hole_y*1.017, 0.0f);
		glColor3f(0.48f, 0.48f, 0.48f);
		drawFilledCircle(hole_rad);
		glPopMatrix();
		if(i==0)
			hole_x+=box_len_in-2*hole_rad;
		else if(i==1)
			hole_y -= box_len_in-2*hole_rad;
		else if(i==2)
			hole_x-=box_len_in-2*hole_rad;
	}

    // Draw Center
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	drawHollowCircle(center_rad);

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	drawFilledCircle(center_rad/2);
	glPopMatrix();

	glPopMatrix();

    // Draw Direction Arrow
	if(!launched){
		glPushMatrix();
		glTranslatef(striker_x, striker_y, 0.0f);
		glRotatef(_angle, 0.0f, 0.0f, 1.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		drawLine(0.0f, 0.0f, up_pointx, up_pointy);
		glPopMatrix();
	}

    // Draw Launch Bar and side arrows
	for(int i=0; i<4; i++){
		glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if(i==0)
			drawBox(-(launch_bar_len/2),-(launch_bar_len/2),-(launch_bar_len/2),-(launch_bar_len/2+launch_bar_wid),launch_bar_len/2,-(launch_bar_len/2+launch_bar_wid),launch_bar_len/2,-(launch_bar_len/2));
		if(i==1)
			drawBox(launch_bar_len/2,-(launch_bar_len/2),launch_bar_len/2+launch_bar_wid,-(launch_bar_len/2),launch_bar_len/2+launch_bar_wid,launch_bar_len/2,launch_bar_len/2,launch_bar_len/2);
		if(i==2)
			drawBox(launch_bar_len/2,launch_bar_len/2,launch_bar_len/2,launch_bar_len/2+launch_bar_wid,-(launch_bar_len/2),launch_bar_len/2+launch_bar_wid,-launch_bar_len/2,launch_bar_len/2);
		if(i==3)
			drawBox(-launch_bar_len/2,launch_bar_len/2,-(launch_bar_len/2+launch_bar_wid),launch_bar_len/2,-(launch_bar_len/2+launch_bar_wid),-launch_bar_len/2,-launch_bar_len/2,-launch_bar_len/2);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		if(i==0)
			drawLine(-(launch_bar_len/2),-(launch_bar_len/2),-(launch_bar_len/4),-(launch_bar_len/4));
		if(i==1)
			drawLine(launch_bar_len/2,-(launch_bar_len/2),launch_bar_len/4,-(launch_bar_len/4));
		if(i==2)
			drawLine(launch_bar_len/2,launch_bar_len/2,launch_bar_len/4,launch_bar_len/4);
		if(i==3)
			drawLine(-launch_bar_len/2,launch_bar_len/2,-launch_bar_len/4,launch_bar_len/4);
		glPopMatrix();    
	}

    // Draw striker
	if ((hole_flag) || stable){
		stable = 0;
		hole_flag = 0;
		striker_x = 0.0f;
		striker_y = -(launch_bar_len/2+launch_bar_wid/2);
	}
	glPushMatrix();
	glTranslatef(striker_x, striker_y, 0.0f);
	glColor3f(0.48f, 0.0f, 0.0f);
	drawFilledCircle(striker_rad);
	glPopMatrix();

    // Draw Red Coin
	if(!hole_queen){
		glPushMatrix();
		glTranslatef(coin_x, coin_y, 0.0f);
		glColor3f(0.96f, 0.0f, 0.0f);
		drawFilledCircle(coin_rad);
		glPopMatrix();
	}
    // Draw Coins
	for(int i=0;i<coin_no;i++){
		if(!hole_coin[i]){
			glPushMatrix();
			if(!initial){
			//glRotatef(360/coin_no*i, 0.0f,0.0f,1.0f);
				coins_x[i]=(center_rad-coin_rad)*sin(360*i/coin_no/2.5);
				coins_y[i]=(center_rad-coin_rad)*cos(360*i/coin_no/2.5);
			}
			glTranslatef(coins_x[i], coins_y[i], 0.0f);
			if(i%2==0)
				glColor3f(0.0f, 0.0f, 1.0f);
			else	
				glColor3f(0.684f, 0.684f, 0.684f);
			drawFilledCircle(coin_rad);
			glPopMatrix();
		}
	}

	glPopMatrix();

    // Draw Power Box
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -8.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawBox(power_box_x, power_box_y, power_box_x + power_box_wid, power_box_y, power_box_x + power_box_wid, power_box_y + power_box_len, power_box_x, power_box_y + power_box_len);
	glPopMatrix();

    // Draw Power
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -8.0f);
	glColor3f(1.0f, 1.0f, 0.0f);
	drawBox(power_box_x, power_box_y, power_box_x + power_box_wid, power_box_y, power_box_x+power_box_wid, power_box_y+power, power_box_x, power_box_y+power);
	glPopMatrix();

	char text[50]="Blue Score: ";
	int len = strlen(text);
	drawText(text,len,50,50);

	glutSwapBuffers();
}
// ==================================================================

// ==================================================================
void drawBox(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {

	glBegin(GL_QUADS);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glVertex2f(x3, y3);
	glVertex2f(x4, y4);
	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
// ==================================================================

// ==================================================================
void drawHollowCircle(float rad) {
	glBegin(GL_LINE_LOOP);
	for(int i=0 ; i<360 ; i++) {
		glVertex2f(rad * cos(DEG2RAD(i)), rad * sin(DEG2RAD(i)));
	}
	glEnd();
}
// ==================================================================

// ==================================================================
void drawFilledCircle(float rad) {
	glBegin(GL_TRIANGLE_FAN);
	for(int i=0 ; i<360 ; i++) {
		glVertex2f(rad * cos(DEG2RAD(i)), rad * sin(DEG2RAD(i)));
	}
	glEnd();
}
// ==================================================================

// ==================================================================
void drawLine(float x1, float y1, float x2, float y2) {
	glBegin(GL_LINES);
	glVertex2f(x1,y1);
	glVertex2f(x2,y2);
	glEnd();
}
// ==================================================================

// ==================================================================
// Function to handle all calculations in the scene
// updated evry 10 milliseconds
void update(int value) {
    // Update position of striker
	hole_flag=checkHole(striker_x,striker_y);
	if(hole_flag==1){
		launched=0;
		if(score>5)
			score -=5;
		else
			score=0;
	}
	hole_queen=checkHole(coin_x,coin_y);
	if(hole_queen){
		coin_velx=0;
		coin_vely=0;
		if(queen_flag==0){
			score+=50;
			queen_flag=1;
		}
	}
	for(int i=0;i<coin_no;i++){
		hole_coin[i]=checkHole(coins_x[i],coins_y[i]);
		if(hole_coin[i]==1){
			coins_velx[i]=0;
			coins_vely[i]=0;
			if(i%2==0){
				if(coins_flag[i]==0){
					score+=10;
					coins_flag[i]=1;
				}
			}
			else if(i%2!=0){
				if(score>5)
					score-=5;
				else
					score=0;
			}
		}
	}
	if (!hole_flag){
        // friction on striker
		if (fabs(striker_velx)<fs_striker/10)
			striker_velx=0;
		else{
			alpha = atan(striker_vely/striker_velx);
			float sign = striker_velx/fabs(striker_velx);
			striker_velx=(fabs(striker_velx)-fs_striker/10*fabs(cos(alpha)))*sign;
		}
		if (fabs(striker_vely)<fs_striker/10)
			striker_vely=0;
		else{
			alpha = atan(striker_vely/striker_velx);
			float sign = striker_vely/fabs(striker_vely);
			striker_vely=(fabs(striker_vely)-fs_striker/10*fabs(sin(alpha)))*sign;
		}

        // friction on coin
		if (fabs(coin_velx)<fs_coin/10)
			coin_velx=0;
		else{
			beta = atan(coin_vely/coin_velx);
			float sign = coin_velx/fabs(coin_velx);
			coin_velx=(fabs(coin_velx)-fs_coin/10*fabs(cos(beta)))*sign;
		}
		if (fabs(coin_vely)<fs_coin/10)
			coin_vely=0;
		else{
			beta = atan(coin_vely/coin_velx);
			float sign = coin_vely/fabs(coin_vely);
			coin_vely=(fabs(coin_vely)-fs_coin/10*fabs(sin(beta)))*sign;
		}

		for(int i=0;i<coin_no;i++){
        // friction on coins
			if (fabs(coins_velx[i])<fs_coin/10)
				coins_velx[i]=0;
			else{
				beta = atan(coins_vely[i]/coins_velx[i]);
				float sign = coins_velx[i]/fabs(coins_velx[i]);
				coins_velx[i]=(fabs(coins_velx[i])-fs_coin/10*fabs(cos(beta)))*sign;
			}
			if (fabs(coins_vely[i])<fs_coin/10)
				coins_vely[i]=0;
			else{
				beta = atan(coins_vely[i]/coins_velx[i]);
				float sign = coins_vely[i]/fabs(coins_vely[i]);
				coins_vely[i]=(fabs(coins_vely[i])-fs_coin/10*fabs(sin(beta)))*sign;
			}
		}

		checkStability(striker_velx,striker_vely,coin_velx,coin_vely);
		if (!stable){

        	// Handle striker collisions with board
			if((striker_x + striker_rad < box_len_in/2 && striker_x+striker_rad+striker_velx<box_len_in/2) && (striker_x - striker_rad > -box_len_in/2 && striker_x - striker_rad+striker_velx>-box_len_in/2)){
				for(int i=0;i<1000;i++)
					striker_x += striker_velx/1000;
			}
			else
				striker_velx *= -1;		
			if((striker_y + striker_rad < box_len_in/2 && striker_y + striker_rad+striker_vely < box_len_in/2) && (striker_y - striker_rad > -box_len_in/2 && striker_y - striker_rad+striker_vely > -box_len_in/2)){
				for(int i=0;i<1000;i++)
					striker_y += striker_vely/1000;
			}
			else
				striker_vely *= -1;
        	// Handle coin collision with board
			if((coin_x + coin_rad < box_len_in/2 && coin_x+coin_rad+coin_velx<box_len_in/2) && (coin_x - coin_rad > -box_len_in/2 && coin_x - coin_rad+coin_velx>-box_len_in/2)){
				for(int i=0;i<1000;i++)
					coin_x += coin_velx/1000;
			}
			else
				coin_velx *= -1;		
			if((coin_y + coin_rad < box_len_in/2 && coin_y + coin_rad+coin_vely < box_len_in/2) && (coin_y - coin_rad > -box_len_in/2 && coin_y - coin_rad+coin_vely > -box_len_in/2)){
				for(int i=0;i<1000;i++)
					coin_y += coin_vely/1000;
			}
			else
				coin_vely *= -1;

			for(int i=0;i<coin_no;i++){
			// Handle coins collision with board
				if((coins_x[i] + coin_rad < box_len_in/2 && coins_x[i]+coin_rad+coins_velx[i]<box_len_in/2) && (coins_x[1] - coin_rad > -box_len_in/2 && coins_x[i] - coin_rad+coins_velx[i]>-box_len_in/2))
					coins_x[i] += coins_velx[i];
				else
					coins_velx[i] *= -1;		
				if((coins_y[i] + coin_rad < box_len_in/2 && coins_y[i] + coin_rad+coins_vely[i] < box_len_in/2) && (coins_y[i] - coin_rad > -box_len_in/2 && coins_y[i] - coin_rad+coins_vely[i] > -box_len_in/2))
					coins_y[i] += coins_vely[i];
				else
					coins_vely[i] *= -1;	
			}
			checkCollision();
			glutTimerFunc(10, update, 0);
		}
		else{
			striker_velx = 0;
			striker_vely = 0;
		}
	}
	else{
		striker_velx=0;
		striker_vely=0;
	}
}
// ==================================================================

// ==================================================================
void updateAngle(char direction) {
	if (direction=='a' && _angle<80.0f){
		_angle += 2.0f;
	}
	else if(direction=='c' && _angle>-80.0f){
		_angle -=2.0f;
	}
	glutPostRedisplay();
}
// ==================================================================

// ==================================================================
int checkHole(float x, float y){
	if (sqrt(pow(((-box_len_in/2)+hole_rad)*1.017-x,2)+pow(((box_len_in/2)-hole_rad)*1.017-y,2)) <= hole_rad){
		return 1;
	}
	else if (sqrt(pow(((box_len_in/2)-hole_rad)*1.017-x,2)+pow(((box_len_in/2)-hole_rad)*1.017-y,2)) <= hole_rad){
		return 1;
	}
	else if (sqrt(pow(((box_len_in/2)-hole_rad)*1.017-x,2)+pow((-(box_len_in/2)+hole_rad)*1.017-y,2)) <= hole_rad){
		return 1;
	}
	else if (sqrt(pow((-(box_len_in/2)+hole_rad)*1.017-x,2)+pow((-(box_len_in/2)+hole_rad)-y,2)) <= hole_rad){
		return 1;
	}
	else
		return 0;
}
// ==================================================================

// ==================================================================
void checkStability(float striker_velx, float striker_vely, float coin_velx, float coin_vely){
	if (striker_velx==0 && striker_vely==0 && coin_velx==0 && coin_vely==0){
		stable_count=0;
		for(int i=0;i<coin_no;i++){
			if(coins_velx[i]==0 && coins_vely[i]==0)
				stable_count++;
		}
		if(stable_count==coin_no){
			launched = 0;
			stable = 1;
			sleep(1);
		}
	}
}
// ==================================================================

// ==================================================================
void checkCollision(){
	if (sqrt(pow(striker_x-coin_x,2)+pow(striker_y-coin_y,2))<= striker_rad+coin_rad){
        // Calculating angles
		if(striker_velx!=0)
			alpha = atan(striker_vely/striker_velx);
		else
			alpha = (PI/2);

		if (coin_velx!=0)
			beta = atan(coin_vely/coin_velx);
		else
			beta = (PI/2);

		if (coin_x-striker_x!=0)
			theta = atan((coin_y- striker_y)/(coin_x-striker_x));
		else 
			theta = (PI/2);

		if(_angle!=0){
			if (striker_velx>0)
				striker_vel = sqrt(pow(striker_velx,2)+pow(striker_vely,2));
			else
				striker_vel = sqrt(pow(striker_velx,2)+pow(striker_vely,2))*-1;
			if(coin_velx>0)
				coin_vel = sqrt(pow(coin_velx,2)+pow(coin_vely,2));
			else
				coin_vel = sqrt(pow(coin_velx,2)+pow(coin_vely,2))*-1;

			striker_velx = (2*coin_vel*cos(beta-theta)*cos(theta)+striker_vel*cos(alpha-theta)*cos(theta))/3 - striker_vel*sin(alpha-theta)*sin(theta);
			striker_vely = (2*coin_vel*cos(beta-theta)*sin(theta)+striker_vel*cos(alpha-theta)*sin(theta))/3 + striker_vel*sin(alpha-theta)*cos(theta);
			coin_velx = (4*striker_vel*cos(alpha-theta)*cos(theta)-coin_vel*cos(beta-theta)*cos(theta))/3- coin_vel*sin(beta-theta)*sin(theta);
			coin_vely = (4*striker_vel*cos(alpha-theta)*sin(theta)-coin_vel*cos(beta-theta)*sin(theta))/3+ coin_vel*sin(beta-theta)*cos(theta);
		}
		else{
			float temp = striker_vely;
			striker_vely = (2*coin_vely+striker_vely)/3;
			coin_vely = (4*temp-coin_vely)/3;
		}

	}
	else{
		for(int i=0;i<coin_no;i++){
			if (sqrt(pow(striker_x-coins_x[i],2)+pow(striker_y-coins_y[i],2))<= striker_rad+coin_rad){
    	    // Calculating angles
				if(striker_velx!=0)
					alpha = atan(striker_vely/striker_velx);
				else
					alpha = (PI/2);

				if (coins_velx[i]!=0)
					beta = atan(coins_vely[i]/coins_velx[i]);
				else
					beta = (PI/2);

				if (coins_x[i]-striker_x!=0)
					theta = atan((coins_y[i]- striker_y)/(coins_x[i]-striker_x));
				else 
					theta = (PI/2);

				if(_angle!=0){
					if (striker_velx>0)
						striker_vel = sqrt(pow(striker_velx,2)+pow(striker_vely,2));
					else
						striker_vel = sqrt(pow(striker_velx,2)+pow(striker_vely,2))*-1;
					if(coins_velx[i]>0)
						coins_vel[i] = sqrt(pow(coins_velx[i],2)+pow(coins_vely[i],2));
					else
						coins_vel[i] = sqrt(pow(coins_velx[i],2)+pow(coins_vely[i],2))*-1;

					striker_velx = (2*coins_vel[i]*cos(beta-theta)*cos(theta)+striker_vel*cos(alpha-theta)*cos(theta))/3 - striker_vel*sin(alpha-theta)*sin(theta);
					striker_vely = (2*coins_vel[i]*cos(beta-theta)*sin(theta)+striker_vel*cos(alpha-theta)*sin(theta))/3 + striker_vel*sin(alpha-theta)*cos(theta);
					coins_velx[i] = (4*striker_vel*cos(alpha-theta)*cos(theta)-coins_vel[i]*cos(beta-theta)*cos(theta))/3- coins_vel[i]*sin(beta-theta)*sin(theta);
					coins_vely[i] = (4*striker_vel*cos(alpha-theta)*sin(theta)-coins_vel[i]*cos(beta-theta)*sin(theta))/3+ coins_vel[i]*sin(beta-theta)*cos(theta);
				}
				else{
					float temp = striker_vely;
					striker_vely = (2*coins_vely[i]+striker_vely)/3;
					coins_vely[i] = (4*temp-coins_vely[i])/3;
				}	
			}	
		}
	}
	for(int i=0;i<coin_no;i++){
		if (sqrt(pow(coin_x-coins_x[i],2)+pow(coin_y-coins_y[i],2))<= coin_rad+coin_rad){
    	    // Calculating angles
			if(coin_velx!=0)
				alpha = atan(coin_vely/coin_velx);
			else
				alpha = (PI/2);

			if (coins_velx[i]!=0)
				beta = atan(coins_vely[i]/coins_velx[i]);
			else
				beta = (PI/2);

			if (coins_x[i]-coin_x!=0)
				theta = atan((coins_y[i]- coin_y)/(coins_x[i]-coin_x));
			else 
				theta = (PI/2);

			if(_angle!=0){
				if (coin_velx>0)
					coin_vel = sqrt(pow(coin_velx,2)+pow(coin_vely,2));
				else
					coin_vel = sqrt(pow(coin_velx,2)+pow(coin_vely,2))*-1;
				if(coins_velx[i]>0)
					coins_vel[i] = sqrt(pow(coins_velx[i],2)+pow(coins_vely[i],2));
				else
					coins_vel[i] = sqrt(pow(coins_velx[i],2)+pow(coins_vely[i],2))*-1;

				coin_velx = (2*coins_vel[i]*cos(beta-theta)*cos(theta)+coin_vel*cos(alpha-theta)*cos(theta))/3 - coin_vel*sin(alpha-theta)*sin(theta);
				coin_vely = (2*coins_vel[i]*cos(beta-theta)*sin(theta)+coin_vel*cos(alpha-theta)*sin(theta))/3 + coin_vel*sin(alpha-theta)*cos(theta);
				coins_velx[i] = (4*coin_vel*cos(alpha-theta)*cos(theta)-coins_vel[i]*cos(beta-theta)*cos(theta))/3- coins_vel[i]*sin(beta-theta)*sin(theta);
				coins_vely[i] = (4*coin_vel*cos(alpha-theta)*sin(theta)-coins_vel[i]*cos(beta-theta)*sin(theta))/3+ coins_vel[i]*sin(beta-theta)*cos(theta);
			}
			else{
				float temp = coin_vely;
				coin_vely = (2*coins_vely[i]+coin_vely)/3;
				coins_vely[i] = (4*temp-coins_vely[i])/3;
			}	
		}	
	}
	for(int j=0;j<coin_no;j++){
		for(int i=j+1;i<coin_no;i++){
			if(i!=j){
				if (sqrt(pow(coins_x[j]-coins_x[i],2)+pow(coins_y[j]-coins_y[i],2))<= coin_rad+coin_rad){
    	    // Calculating angles
					if(coins_velx[j]!=0)
						alpha = atan(coins_vely[j]/coins_velx[j]);
					else
						alpha = (PI/2);

					if (coins_velx[i]!=0)
						beta = atan(coins_vely[i]/coins_velx[i]);
					else
						beta = (PI/2);

					if (coins_x[i]-coins_x[j]!=0)
						theta = atan((coins_y[i]- coins_y[j])/(coins_x[i]-coins_x[j]));
					else 
						theta = (PI/2);

					if(_angle!=0){
						if (coins_velx[j]>0)
							coins_vel[j] = sqrt(pow(coins_velx[j],2)+pow(coins_vely[j],2));
						else
							coins_vel[j] = sqrt(pow(coins_velx[j],2)+pow(coins_vely[j],2))*-1;
						if(coins_velx[i]>0)
							coins_vel[i] = sqrt(pow(coins_velx[i],2)+pow(coins_vely[i],2));
						else
							coins_vel[i] = sqrt(pow(coins_velx[i],2)+pow(coins_vely[i],2))*-1;

						coins_velx[j] = (2*coins_vel[i]*cos(beta-theta)*cos(theta)+coins_vel[j]*cos(alpha-theta)*cos(theta))/3 - coins_vel[j]*sin(alpha-theta)*sin(theta);
						coins_vely[j] = (2*coins_vel[i]*cos(beta-theta)*sin(theta)+coins_vel[j]*cos(alpha-theta)*sin(theta))/3 + coins_vel[j]*sin(alpha-theta)*cos(theta);
						coins_velx[i] = (4*coins_vel[j]*cos(alpha-theta)*cos(theta)-coins_vel[i]*cos(beta-theta)*cos(theta))/3- coins_vel[i]*sin(beta-theta)*sin(theta);
						coins_vely[i] = (4*coins_vel[j]*cos(alpha-theta)*sin(theta)-coins_vel[i]*cos(beta-theta)*sin(theta))/3+ coins_vel[i]*sin(beta-theta)*cos(theta);
					}
					else{
						float temp = coins_vely[j];
						coins_vely[j] = (2*coins_vely[i]+coins_vely[j])/3;
						coins_vely[i] = (4*temp-coins_vely[i])/3;
					}	
				}
			}	
		}
	}
}

// ==================================================================

// ==================================================================
// Initializing some openGL 3D rendering options
void initRendering() {

    glEnable(GL_DEPTH_TEST);        // Enable objects to be drawn ahead/behind one another
    glEnable(GL_COLOR_MATERIAL);    // Enable coloring
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // Setting a background color
}
// ==================================================================

// ==================================================================
// Function called when the window is resized
void handleResize(int w, int h) {

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)w / (float)h, 0.1f, 200.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
// ==================================================================

// ==================================================================
void handleKeypress1(unsigned char key, int x, int y) {

	if (key == 27)
        exit(0);     // escape key is pressed
    if (key == 'a')
    	updateAngle('a');
    if (key == 'c')
    	updateAngle('c');

    if (key == 32 && !launched){                   // spacebar is pressed
    	initial = 1;
    	if(_angle<=0)
    		striker_vel = power/power_reduction;
    	else
    		striker_vel = power/power_reduction*(-1);

    	striker_velx = striker_vel * fabs(sin(_angle*PI/180));
    	striker_vely = fabs(striker_vel * fabs(cos(_angle*PI/180)));
    	glutTimerFunc(10, update, 0);
    	launched = 1;
    }
}
// ==================================================================

// ==================================================================
void handleKeypress2(int key, int x, int y) {

	if (key == GLUT_KEY_LEFT && !launched){
		if(striker_x - striker_rad > -launch_bar_len/2 && striker_x - striker_rad-striker_bar_velx>=-launch_bar_len/2)
			striker_x -= striker_bar_velx;
		else if(striker_x - striker_rad > -launch_bar_len/2 && striker_x - striker_rad-striker_bar_velx<-launch_bar_len/2)
			striker_x=-launch_bar_len/2+striker_rad;
	}
	if (key == GLUT_KEY_RIGHT && !launched){
		if(striker_x + striker_rad < launch_bar_len/2 && striker_x+striker_rad+striker_bar_velx<=launch_bar_len/2)
			striker_x += striker_bar_velx;
		else if(striker_x + striker_rad < launch_bar_len/2 && striker_x+striker_rad+striker_bar_velx>launch_bar_len/2){
			striker_x=launch_bar_len/2-striker_rad;
		}
	}
	if (key == GLUT_KEY_UP && !launched){
		if (power < 2*(power_box_y + power_box_len))
			power+=pow_slot;
	}
	if (key == GLUT_KEY_DOWN && !launched){
		if(power > 0)
			power-=pow_slot;
	}
}
// ==================================================================

void handleMouseclick(int button, int state, int x, int y) {
	if (state == GLUT_DOWN)
    {
        if (button == GLUT_RIGHT_BUTTON){
        	if(striker_x + striker_rad < launch_bar_len/2 && striker_x+striker_rad+striker_bar_velx<=launch_bar_len/2)
			striker_x += striker_bar_velx;
			else if(striker_x + striker_rad < launch_bar_len/2 && striker_x+striker_rad+striker_bar_velx>launch_bar_len/2)
				striker_x=launch_bar_len/2-striker_rad;
        }
        
        if (button == GLUT_LEFT_BUTTON){
        	initial = 1;
    		if(_angle<=0)
    			striker_vel = power/power_reduction;
    		else
    			striker_vel = power/power_reduction*(-1);

    		striker_velx = striker_vel * fabs(sin(_angle*PI/180));
    		striker_vely = fabs(striker_vel * fabs(cos(_angle*PI/180)));
    		glutTimerFunc(10, update, 0);
    		launched = 1;
    	}
	}
}

void drawText(char text[], int length, int x, int y){
	glMatrixMode(GL_PROJECTION);
	double *matrix = new double[16];
	glGetDoublev(GL_PROJECTION_MATRIX, matrix);
	glLoadIdentity();
	glOrtho(0,800,0,600,-5,5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(x,y);
	for(int i=0;i<length;i++){
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)text[i]);
	}
	if(score>0){
		int temp=score;
		int factor=10;
		while(temp/factor>10)
			factor=factor*10;
		while(factor!=0){
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, temp/factor+48);
			temp=temp%factor;
			factor=factor/10;	
		}
	}
	else
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 0+48);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matrix);
	glMatrixMode(GL_MODELVIEW);
}

void timer(int value){
	if(!launched)
		score-=1;
	glutTimerFunc(1000, timer, 0);
}