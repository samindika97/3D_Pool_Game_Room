#include <stdlib.h> 
#include <GL/glut.h>
#include <math.h>
#include <SOIL2.h>
#include <stdio.h> 
#include <time.h> 
#include <cmath>

int room_size = 15;
int roof_size = room_size-8;
int roof_height = 12;
float wall_thickness = 0.1;
int width;
int height;
unsigned char* image;
GLuint tex;
GLboolean hit = false;
GLboolean beforeHit = true;
GLuint method;

int animationFactor = 0;

float rotation_factor = 0.0;
int position = 0;

//variables to move the camera
GLfloat camX = 0.0; GLfloat camY = 0.0; GLfloat camZ = 0.0;

//variables to move the scene
GLfloat sceRX = 0.0; GLfloat sceRY = 0.0; GLfloat sceRZ = 0.0;
GLfloat sceTX = 0.0; GLfloat sceTY = 0.0; GLfloat sceTZ = 0.0;

//variables to  move the objects
GLfloat objRX = 0.0; GLfloat objRY = 0.0; GLfloat objRZ = 0.0;
GLfloat objTX = 0.0; GLfloat objTY = 0.0; GLfloat objTZ = 0.0;


#define NUM_BALLS 15 // Number of balls 

//float vertices[][3] = {};

//Roof surfaces
static float roofV[][3] = { {-roof_size*2,roof_height,roof_size*2},{roof_size*2,roof_height,roof_size*2},{roof_size*2,roof_height,-roof_size*2},{-roof_size*2,roof_height,-roof_size*2},{0.0,roof_height+5,0.0} };


typedef struct {
	float before_x, before_z;
	float after_x, after_z;
	float color[3];
} Position;

Position balls[NUM_BALLS] ; // Array to store ball positions
Position cueBallPosition;
Position cueStickPosition;


// Define a function for linear interpolation
float lerp(float a, float b, float t) {
	return a + t * (b - a);
}

void assignBallColor(int index) {
	switch (index) {
	case 0:
		balls[index].color[0] = 1.0f;  // Yellow
		balls[index].color[1] = 1.0f;
		balls[index].color[2] = 0.0f;
		break;
	case 1:
		balls[index].color[0] = 0.0f;  // Blue
		balls[index].color[1] = 0.0f;
		balls[index].color[2] = 1.0f;
		break;
	case 2:
		balls[index].color[0] = 0.5f;  // Red
		balls[index].color[1] = 0.0f;
		balls[index].color[2] = 0.0f;
		break;
	case 3:
		balls[index].color[0] = 1.0f;  // Purple
		balls[index].color[1] = 0.0f;
		balls[index].color[2] = 1.0f;
		break;
	case 4:
		balls[index].color[0] = 0.0f;  // Orange
		balls[index].color[1] = 0.5f;
		balls[index].color[2] = 1.0f;
		break;
	case 5:
		balls[index].color[0] = 0.0f;  // Light Blue
		balls[index].color[1] = 1.0f;
		balls[index].color[2] = 1.0f;
		break;
	case 6:
		balls[index].color[0] = 0.5f;  // Green
		balls[index].color[1] = 1.0f;
		balls[index].color[2] = 0.0f;
		break;
	case 7:
		balls[index].color[0] = 1.0f;  // Brown
		balls[index].color[1] = 0.5f;
		balls[index].color[2] = 0.0f;
		break;
	case 8:
		balls[index].color[0] = 1.0f;  // Pink
		balls[index].color[1] = 0.5f;
		balls[index].color[2] = 1.0f;
		break;
	case 9:
		balls[index].color[0] = 0.0f;  // Dark Blue
		balls[index].color[1] = 0.0f;
		balls[index].color[2] = 0.5f;
		break;
	case 10:
		balls[index].color[0] = 0.0f;  // Light Green
		balls[index].color[1] = 1.0f;
		balls[index].color[2] = 0.5f;
		break;
	case 11:
		balls[index].color[0] = 0.5f;  // Dark Red
		balls[index].color[1] = 0.0f;
		balls[index].color[2] = 0.5f;
		break;
	case 12:
		balls[index].color[0] = 0.0f;  // Light Brown
		balls[index].color[1] = 0.5f;
		balls[index].color[2] = 0.5f;
		break;
	case 13:
		balls[index].color[0] = 0.5f;  // Dark Green
		balls[index].color[1] = 0.5f;
		balls[index].color[2] = 0.0f;
		break;
	case 14:
		balls[index].color[0] = 0.5f;  // Light Purple
		balls[index].color[1] = 0.0f;
		balls[index].color[2] = 0.5f;
		break;
	default:
		// Default color for unspecified cases
		balls[index].color[0] = 1.0f;
		balls[index].color[1] = 0.0f;
		balls[index].color[2] = 0.0f;  // White
		break;
	}
}

// Function to calculate and set the starting positions of the balls on the table
void setStartingPositions() {

	int count = 0;
	float offsetX = 0.0f, offsetZ = 0.0f, radius = 0.2f;
	// Seed the random number generator with the current time
	srand(static_cast<unsigned>(time(nullptr)));

	for (int i = 0; i < NUM_BALLS; ++i) {
		offsetZ = i * radius * 1.1f;
		for (int j = 0; j <= i; ++j) {
			offsetX = (j - i * 0.5f) * radius;
			balls[count].before_x = offsetX;
			balls[count].before_z = -offsetZ;
			// Randomize the after_x and after_z values
			balls[count].after_x = static_cast<float>(rand() % 590) / 100.0f - 2.90f;  // Random value between -3 and +3
			balls[count].after_z = static_cast<float>(rand() % 781) / 100.0f - 3.90f;  // Random value between -4 and +4
			//printf("%f , %f \n", balls[count].after_x, balls[count].after_z);
			assignBallColor(count);
			count++;
		}
	}
}

void setStrightShot() {
	cueBallPosition.before_x = -1.80;
	cueBallPosition.before_z = -1.70;
	cueBallPosition.after_x = -2.25;
	cueBallPosition.after_z = -2.25;

	cueStickPosition.before_x = -1.5;
	cueStickPosition.before_z = -3.5;
	cueStickPosition.after_x = -2;
	cueStickPosition.after_z = -3.5;

	balls[0].before_x = -2;
	balls[0].before_z = -2;
	balls[0].after_x = -3;
	balls[0].after_z = -4;
	assignBallColor(0);

	balls[2].before_x = 2.432;
	balls[2].before_z = -1.320;
	balls[2].after_x = 2.432;
	balls[2].after_z = -1.320;
	assignBallColor(2);

	balls[3].before_x = 1.432;
	balls[3].before_z = -0.320;
	balls[3].after_x = 1.432;
	balls[3].after_z = -0.320;
	assignBallColor(3);

	balls[4].before_x = 0.742;
	balls[4].before_z = 1.653;
	balls[4].after_x = 0.742;
	balls[4].after_z = 1.653;
	assignBallColor(4);

	balls[5].before_x = -0.742;
	balls[5].before_z = -1.653;
	balls[5].after_x = -0.742;
	balls[5].after_z = -1.653;
	assignBallColor(5);
}

void setCarromShot() {

	cueStickPosition.before_x = 0;
	cueStickPosition.before_z = -1.8;
	cueStickPosition.after_x = 0;
	cueStickPosition.after_z = -3.5;

	cueBallPosition.before_x = 0;
	cueBallPosition.before_z = 0;
	cueBallPosition.after_x = 0.5;
	cueBallPosition.after_z = -2;


	balls[0].before_x = 0.6;
	balls[0].before_z = -2;
	balls[0].after_x = 3;
	balls[0].after_z = -4;
	assignBallColor(3);

	balls[1].before_x = 0.4;
	balls[1].before_z = -2;
	balls[1].after_x = -3;
	balls[1].after_z = -4;
	assignBallColor(1);

	balls[2].before_x = 2.432;
	balls[2].before_z = -1.320;
	balls[2].after_x = 2.432;
	balls[2].after_z = -1.320;
	assignBallColor(2);

	balls[3].before_x = 0.567;
	balls[3].before_z = -0.320;
	balls[3].after_x = 0.567;
	balls[3].after_z = -0.320;
	assignBallColor(3);

	balls[4].before_x = 0.123;
	balls[4].before_z = 1.653;
	balls[4].after_x = 0.123;
	balls[4].after_z = 1.653;
	assignBallColor(4);

	balls[5].before_x = -0.742;
	balls[5].before_z = -1.653;
	balls[5].after_x = -0.742;
	balls[5].after_z = -1.653;
	assignBallColor(5);

}

void loadTextureDataFromImage(const char* filename) {
	image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);

	if (image == NULL) {
		printf("Error : %s", SOIL_last_result());
	}
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB,
		GL_UNSIGNED_BYTE, image);  // Create texture from image data
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

void drawGrid() {
	GLfloat step = 1.0f;
	GLint line;

	glBegin(GL_LINES);
	for (line = -20; line <= 20; line += step) {
		glVertex3f(line, -0.4, 20);
		glVertex3f(line, -0.4, -20);

		glVertex3f(20, -0.4, line);
		glVertex3f(-20, -0.4, line);
	}
	glEnd();
}

void drawAxes() {

	glBegin(GL_LINES);

	glLineWidth(1.5);

	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(-200, 0, 0);
	glVertex3f(200, 0, 0);

	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0, -200, 0);
	glVertex3f(0, 200, 0);

	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0, 0, -200);
	glVertex3f(0, 0, 200);

	glEnd();
}

void setLighting() {
	// Lighting set up
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	// Set lighting intensity and color
	GLfloat qaAmbientLight[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat qaDiffuseLight[] = { 1,1,1, 1.0 };
	GLfloat qaSpecularLight[] = { 1.0, 1.0, 1.0, 1.0 };

	GLfloat qaLightPosition[] = { 2,4,9.5, 1.0 };  //lamp
	GLfloat qaLightPosition1[] = { -1,0,0, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, qaAmbientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, qaDiffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, qaSpecularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, qaLightPosition);

	glLightfv(GL_LIGHT1, GL_AMBIENT, qaAmbientLight);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, qaDiffuseLight);
	glLightfv(GL_LIGHT1, GL_SPECULAR, qaSpecularLight);
	glLightfv(GL_LIGHT1, GL_POSITION, qaLightPosition1);


	GLfloat qaWhite[] = { 1.0, 1.0, 1.0, 1.0 };

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, qaWhite);
	glMaterialf(GL_FRONT, GL_SHININESS, 128.0);

}

void setMaterial() {
	// Set material properties
	GLfloat qaBlack[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat qaGreen[] = { 0.0, 1.0, 0.0, 1.0 };
	GLfloat qaRed[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat qaBlue[] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat qaWhite[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat qaPyramid[] = { 0.855, 0.647, 0.125, 1.0 };
	GLfloat qaLowAmbient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat qaFullAmbient[] = { 1.0, 1.0, 1.0, 1.0 };


	//Set, ambient, diffuse and specular lighting. Set ambient to 20%.
	glMaterialfv(GL_FRONT, GL_AMBIENT, qaPyramid);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, qaPyramid);
	glMaterialfv(GL_FRONT, GL_SPECULAR, qaWhite);
	glMaterialf(GL_FRONT, GL_SHININESS, 128.0);


}

void init(void) {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
}

void drawCircle(float cx, float cy, float r, int num_segments)
{
	float theta = 3.1415926 * 2 / float(num_segments);
	float tangetial_factor = tanf(theta);//calculate the tangential factor 

	float radial_factor = cosf(theta);//calculate the radial factor 

	float x = r;//we start at angle = 0 

	float y = 0;
	glLineWidth(2);
	glBegin(GL_POLYGON);
	for (int ii = 0; ii < num_segments; ii++)
	{
		glVertex2f(x + cx, y + cy);//output vertex 

		float tx = -y;
		float ty = x;

		//add the tangential vector 

		x += tx * tangetial_factor;
		y += ty * tangetial_factor;

		//correct using the radial factor 

		x *= radial_factor;
		y *= radial_factor;
	}
	glEnd();
}

void triangle(int a, int b, int c)
{
	//glEnable(GL_TEXTURE_2D);
	glColor3f(1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0);
	glVertex3fv(roofV[a]);

	glTexCoord2f(1.0, 0);
	glVertex3fv(roofV[b]);

	glTexCoord2f(0.5, 1.0);
	glVertex3fv(roofV[c]);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}


void drawPicture() {
	glEnable(GL_TEXTURE_2D);
	char filepath[15] = "beach.jpeg";
	loadTextureDataFromImage(filepath);
	glColor3f(1, 1, 1);
	glPushMatrix();
	glTranslatef(0, 7, -6.5);
	glScalef(0.7, 0.2, 0.5);
	glBegin(GL_POLYGON);
	//glColor3f(0, 0, 1);
	glTexCoord2f(0.0f, 0.0f);  glVertex3f(-room_size / 2, room_size, -room_size / 2);
	glTexCoord2f(1.0f, 0.0f);  glVertex3f(room_size / 2, room_size, -room_size / 2);
	glTexCoord2f(1.0f, 1.0f);  glVertex3f(room_size / 2, 0, -room_size / 2);
	glTexCoord2f(0.0f, 1.0f);  glVertex3f(-room_size / 2, 0, -room_size / 2);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

}

void drawText(const char* text, float x, float y, float z) {
	
	glPushMatrix();
	glColor3f(1, 1, 0);  // Set text color (white in this case)
	glRasterPos3f(x, y, z);

	// Loop through each character in the string and display it
	for (const char* c = text; *c != '\0'; ++c) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
	}

	glPopMatrix();
}

void drawBoard() {
	glPushMatrix();
	glColor3f(0, 0, 1);
	glTranslatef(20, 8, 15);
	glScalef(0.5, 10, 0.5);
	glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
	glColor3f(0.4, 0.4, 0);
	glTranslatef(20, 12, 15);
	glScalef(5, 2, 0.5);
	glutSolidCube(1);
	glPopMatrix();
	drawText("POOL", 18, 12, 15 + 0.8);
	drawText("CLUB", 18, 11, 15 + 0.8);
}

void drawTree() {
	GLUquadric* quad = gluNewQuadric();

	glPushMatrix();
	glColor3f(0.4, 0.2, 0);
	glTranslatef(-25, 5, 15);
	glRotatef(90, 1, 0, 0);
	gluCylinder(quad, 1, 1, 5, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0, 1, 0);
	glTranslatef(-25, 8, 15);
	glRotatef(90, 1, 0, 0);
	gluCylinder(quad, 0.5, 4, 5, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0, 1, 0);
	glTranslatef(-25, 12, 15);
	glRotatef(90, 1, 0, 0);
	gluCylinder(quad, 0.5, 4, 5, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0, 1, 0);
	glTranslatef(-25, 15, 15);
	glRotatef(90, 1, 0, 0);
	gluCylinder(quad, 0.2, 3, 5, 20, 20);
	glPopMatrix();

}

void drawTreeBunch() {
	drawTree();
	glPushMatrix();
	glTranslatef(-6, 0, 0);
	drawTree();
	glPopMatrix();
	
	glPushMatrix();
//	glScalef(2, 2, 2);
	glTranslatef(-7, 0, 4);
	drawTree();
	glPopMatrix();

}

void drawGround() {
	
	loadTextureDataFromImage("grass.jpg");
	glColor3f(1, 1, 1);
	//glColor3f(1, 0.5, 0);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef(0, 2, 0);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0f, 0.0f);  glVertex3f(-room_size*3, 0, -room_size*3);
	glTexCoord2f(1.0f, 0.0f);  glVertex3f(room_size*3, 0, -room_size*3);
	glTexCoord2f(1.0f, 1.0f);  glVertex3f(room_size*3, 0, room_size*3);
	glTexCoord2f(0.0f, 1.0f);  glVertex3f(-room_size*3, 0, room_size*3);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void drawFloorTexture() {
	char filepath[15] = "wood_floor.jpg";
	loadTextureDataFromImage(filepath);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef(0, 2.3, 0);
	glBegin(GL_POLYGON);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 0.0f);  glVertex3f(-room_size, 0, -room_size);
	glTexCoord2f(1.0f, 0.0f);  glVertex3f(room_size, 0, -room_size);
	glTexCoord2f(1.0f, 1.0f);  glVertex3f(room_size, 0, room_size);
	glTexCoord2f(0.0f, 1.0f);  glVertex3f(-room_size, 0, room_size);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void drawCeylingTexture() {
	char filepath[20] = "ceiling_texture.jpg";
	loadTextureDataFromImage(filepath);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef(0, 2, 0);
	glBegin(GL_POLYGON);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 0.0f);  glVertex3f(-room_size, room_size / 2, -room_size);
	glTexCoord2f(1.0f, 0.0f);  glVertex3f(room_size, room_size / 2, -room_size);
	glTexCoord2f(1.0f, 1.0f);  glVertex3f(room_size, room_size / 2, room_size);
	glTexCoord2f(0.0f, 1.0f);  glVertex3f(-room_size, room_size / 2, room_size);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void drawWall(int size) {
	glPushMatrix();
	glTranslatef(wall_thickness / 2, size / 2, 0);
	glScaled(wall_thickness, 1.0, 1.0);
	glutSolidCube(size);
	glPopMatrix();
}

void drawCueBall(float x, float z, float ball_radius, float tabble_height, float table_thickness) {
	glColor3f(1, 1, 1);
	glPushMatrix();
	glTranslatef(x, tabble_height + table_thickness, z);
	glutSolidSphere(ball_radius, 55, 55);
	glPopMatrix();
}

void drawBall(float x, float z, float ball_radius, float tabble_height, float table_thickness, float color[3]) {
	glColor3f(color[0],color[1],color[2]);
	glPushMatrix();
	glTranslatef(x, tabble_height + table_thickness, z);
	if ((x == -3 && z == -4) || (x == 3 && z == 4) || (x == 3 && z == -4) || (x == 3 && z == 4)) {

	}
	glutSolidSphere(ball_radius, 50, 50);
	glPopMatrix();
}

void drawTableTopTexture() {
	//glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef(0, 4.1, 0);
	glBegin(GL_POLYGON);
	glColor3f(1, 1, 1);
	glVertex3f(-3.0, 0, -4.0);
	glVertex3f(3.0, 0, -4.0);
	glVertex3f(3.0, 0, 4.0);
	glVertex3f(-3.0, 0, 4.0);
	glEnd();
	glPopMatrix();
	//glDisable(GL_TEXTURE_2D);
}

void drawCueStick(float x, float z) {
	glColor3f(0.6, 0.2, 0);
	glPushMatrix();
	glTranslatef(x, 0, z);
	glRotatef(-30, 1, 0, 1);
	glPushMatrix();
	glTranslatef(-1.3, 3, 8);     //x,y,z
	glRotatef(180, 0, 1, 0);
	glutSolidCone(0.1, 5, 20, 20);
	glPopMatrix();
	glPopMatrix();
}

//room inside

void drawRoof()
{
	glPushMatrix();
	glScalef(1.2, 1, 1);
	triangle(0, 1, 4);
	triangle(1, 2, 4);
	triangle(2, 4, 3);
	triangle(0, 3, 4);
	glPopMatrix();
}

void drawWallTexture() {
	loadTextureDataFromImage("brick_wall.jpg");
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();

	//back
	glBegin(GL_POLYGON);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 0.0f);  glVertex3f(-room_size / 2, room_size, -room_size / 2);
	glTexCoord2f(1.0f, 0.0f);  glVertex3f(room_size / 2, room_size, -room_size / 2);
	glTexCoord2f(1.0f, 1.0f);  glVertex3f(room_size / 2, 0, -room_size / 2);
	glTexCoord2f(0.0f, 1.0f);  glVertex3f(-room_size / 2, 0, -room_size / 2);
	glEnd();

	//front
	glBegin(GL_POLYGON);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 0.0f);  glVertex3f(-room_size / 2, room_size, room_size / 2);
	glTexCoord2f(1.0f, 0.0f);  glVertex3f(room_size / 2, room_size, room_size / 2);
	glTexCoord2f(1.0f, 1.0f);  glVertex3f(room_size / 2, 0, room_size / 2);
	glTexCoord2f(0.0f, 1.0f);  glVertex3f(-room_size / 2, 0, room_size / 2);
	glEnd();

	//left
	glBegin(GL_POLYGON);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 0.0f);  glVertex3f(-room_size / 2, room_size, room_size / 2);
	glTexCoord2f(1.0f, 0.0f);  glVertex3f(-room_size / 2, room_size, -room_size / 2);
	glTexCoord2f(1.0f, 1.0f);  glVertex3f(-room_size / 2, 0, -room_size / 2);
	glTexCoord2f(0.0f, 1.0f);  glVertex3f(-room_size / 2, 0, room_size / 2);
	glEnd();

	//right
	glBegin(GL_POLYGON);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 0.0f);  glVertex3f(room_size / 2, room_size, -room_size / 2);
	glTexCoord2f(1.0f, 0.0f);  glVertex3f(room_size / 2, room_size, room_size / 2);
	glTexCoord2f(1.0f, 1.0f);  glVertex3f(room_size / 2, 0, room_size / 2);
	glTexCoord2f(0.0f, 1.0f);  glVertex3f(room_size / 2, 0, -room_size / 2);
	glEnd();

	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

void drawLamp() {
	GLUquadric* quad = gluNewQuadric();
	glPushMatrix();
	glTranslatef((room_size / 2) - 2, 2, (-room_size / 2) + 2);

	glColor3f(1, 1, 1);
	//pole
	glPushMatrix();
	glColor3f(0.5, 0.25, 0);
	glRotatef(90, -1, 0, 0);
	gluCylinder(quad, 0.1, 0.1, 5, 20, 20);
	glPopMatrix();

	//head
	glPushMatrix();
	glColor3f(1, 1, 0);
	glTranslatef(0, 5, 0);
	glRotatef(90, -1, 0, 0);
	gluCylinder(quad, 1, 0.5, 1, 20, 20);
	glPopMatrix();

	//base
	glPushMatrix();
	glColor3f(0.5, 0.25, 0);
	glRotatef(90, -1, 0, 0);
	gluCylinder(quad, 1, 1, 0.5, 20, 20);
	glPopMatrix();

	glPopMatrix();

}

void drawChair() {
	glPushMatrix();
	glColor3f(0.0f, 1.0f, 0.5f);
	glTranslatef(11, 3, 0);
	glScalef(0.3, 0.3, 1);
	glRotatef(-90,0, 1, 0);
	glBegin(GL_QUADS);
	//Front
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-2.0f, -0.2f, 2.0f);
	glVertex3f(2.0f, -0.2f, 2.0f);
	glVertex3f(2.0f, 0.2f, 2.0f);
	glVertex3f(-2.0f, 0.2f, 2.0f);

	//Right
	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(2.0f, -0.2f, -2.0f);
	glVertex3f(2.0f, 0.2f, -2.0f);
	glVertex3f(2.0f, 0.2f, 2.0f);
	glVertex3f(2.0f, -0.2f, 2.0f);

	//Back
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(-2.0f, -0.2f, -2.0f);
	glVertex3f(-2.0f, 0.2f, -2.0f);
	glVertex3f(2.0f, 0.2f, -2.0f);
	glVertex3f(2.0f, -0.2f, -2.0f);

	//Left
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-2.0f, -0.2f, -2.0f);
	glVertex3f(-2.0f, -0.2f, 2.0f);
	glVertex3f(-2.0f, 0.2f, 2.0f);
	glVertex3f(-2.0f, 0.2f, -2.0f);

	//top
	glNormal3f(0.0f, 1.0f, 0.0f);

	glVertex3f(2.0f, 0.2f, 2.0f);
	glVertex3f(-2.0f, 0.2f, 2.0f);
	glVertex3f(-2.0f, 0.2f, -2.0f);
	glVertex3f(2.0f, 0.2f, -2.0f);

	//bottom
	glNormal3f(0.0f, -1.0f, 0.0f);

	glVertex3f(2.0f, -0.2f, 2.0f);
	glVertex3f(-2.0f, -0.2f, 2.0f);
	glVertex3f(-2.0f, -0.2f, -2.0f);
	glVertex3f(2.0f, -0.2f, -2.0f);

	//table front leg
	//front
	glNormal3f(0.0f, 0.0f, 1.0f);

	glVertex3f(1.8f, -0.2f, 1.6f);
	glVertex3f(1.4f, -0.2f, 1.6f);
	glVertex3f(1.4f, -3.0f, 1.6f);
	glVertex3f(1.8f, -3.0f, 1.6f);

	//back
	glNormal3f(0.0f, 0.0f, -1.0f);

	glVertex3f(1.8f, -0.2f, 1.2f);
	glVertex3f(1.4f, -0.2f, 1.2f);
	glVertex3f(1.4f, -3.0f, 1.2f);
	glVertex3f(1.8f, -3.0f, 1.2f);

	//right
	glNormal3f(1.0f, 0.0f, 0.0f);

	glVertex3f(1.8f, -0.2f, 1.6f);
	glVertex3f(1.8f, -0.2f, 1.2f);
	glVertex3f(1.8f, -3.0f, 1.2f);
	glVertex3f(1.8f, -3.0f, 1.6f);

	//left
	glNormal3f(-1.0f, 0.0f, 0.0f);

	glVertex3f(1.4f, -0.2f, 1.6f);
	glVertex3f(1.4f, -0.2f, 1.2f);
	glVertex3f(1.4f, -3.0f, 1.2f);
	glVertex3f(1.4f, -3.0f, 1.6f);

	//back leg back
	//front
	glNormal3f(0.0f, 0.0f, -1.0f);

	glVertex3f(1.8f, -0.2f, -1.2f);
	glVertex3f(1.4f, -0.2f, -1.2f);
	glVertex3f(1.4f, -3.0f, -1.2f);
	glVertex3f(1.8f, -3.0f, -1.2f);

	//back
	glNormal3f(0.0f, 0.0f, -1.0f);

	glVertex3f(1.8f, -0.2f, -1.6f);
	glVertex3f(1.4f, -0.2f, -1.6f);
	glVertex3f(1.4f, -3.0f, -1.6f);
	glVertex3f(1.8f, -3.0f, -1.6f);

	//right
	glNormal3f(1.0f, 0.0f, 0.0f);

	glVertex3f(1.8f, -0.2f, -1.6f);
	glVertex3f(1.8f, -0.2f, -1.2f);
	glVertex3f(1.8f, -3.0f, -1.2f);
	glVertex3f(1.8f, -3.0f, -1.6f);

	//left
	glNormal3f(1.0f, 0.0f, 0.0f);

	glVertex3f(1.4f, -0.2f, -1.6f);
	glVertex3f(1.4f, -0.2f, -1.2f);
	glVertex3f(1.4f, -3.0f, -1.2f);
	glVertex3f(1.4f, -3.0f, -1.6f);

	//leg left front
	glNormal3f(0.0f, 0.0f, 1.0f);

	glVertex3f(-1.8f, -0.2f, 1.6f);
	glVertex3f(-1.4f, -0.2f, 1.6f);
	glVertex3f(-1.4f, -3.0f, 1.6f);
	glVertex3f(-1.8f, -3.0f, 1.6f);

	//back
	glNormal3f(0.0f, 0.0f, -1.0f);

	glVertex3f(-1.8f, -0.2f, 1.2f);
	glVertex3f(-1.4f, -0.2f, 1.2f);
	glVertex3f(-1.4f, -3.0f, 1.2f);
	glVertex3f(-1.8f, -3.0f, 1.2f);

	//right
	glNormal3f(1.0f, 0.0f, 0.0f);

	glVertex3f(-1.8f, -0.2f, 1.6f);
	glVertex3f(-1.8f, -0.2f, 1.2f);
	glVertex3f(-1.8f, -3.0f, 1.2f);
	glVertex3f(-1.8f, -3.0f, 1.6f);

	//left
	glNormal3f(-1.0f, 0.0f, 0.0f);

	glVertex3f(-1.4f, -0.2f, 1.6f);
	glVertex3f(-1.4f, -0.2f, 1.2f);
	glVertex3f(-1.4f, -3.0f, 1.2f);
	glVertex3f(-1.4f, -3.0f, 1.6f);

	//left leg back front

	//front
	glNormal3f(0.0f, 0.0f, -1.0f);

	glVertex3f(-1.8f, -0.2f, -1.2f);
	glVertex3f(-1.4f, -0.2f, -1.2f);
	glVertex3f(-1.4f, -3.0f, -1.2f);
	glVertex3f(-1.8f, -3.0f, -1.2f);

	//back
	glNormal3f(0.0f, 0.0f, -1.0f);

	glVertex3f(-1.8f, -0.2f, -1.6f);
	glVertex3f(-1.4f, -0.2f, -1.6f);
	glVertex3f(-1.4f, -3.0f, -1.6f);
	glVertex3f(-1.8f, -3.0f, -1.6f);

	//right
	glNormal3f(1.0f, 0.0f, 0.0f);

	glVertex3f(-1.8f, -0.2f, -1.6f);
	glVertex3f(-1.8f, -0.2f, -1.2f);
	glVertex3f(-1.8f, -3.0f, -1.2f);
	glVertex3f(-1.8f, -3.0f, -1.6f);

	//left
	glNormal3f(-1.0f, 0.0f, 0.0f);

	glVertex3f(-1.4f, -0.2f, -1.6f);
	glVertex3f(-1.4f, -0.2f, -1.2f);
	glVertex3f(-1.4f, -3.0f, -1.2f);
	glVertex3f(-1.4f, -3.0f, -1.6f);

	//chair back
	//front
	glColor3f(1, 0, 0);
	//glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-1.8f, 0.2f, -1.8f);
	glVertex3f(1.8f, 0.2f, -1.8f);
	glVertex3f(1.8f, 3.5f, -1.8f);
	glVertex3f(-1.8f, 3.5f, -1.8f);

	//back
 //glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-1.8f, 0.2f, -2.0f);
	glVertex3f(1.8f, 0.2f, -2.0f);
	glVertex3f(1.8f, 3.5f, -2.0f);
	glVertex3f(-1.8f, 3.5f, -2.0f);


	//  glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-1.8f, 0.2f, -2.0f);
	glVertex3f(-1.8f, 3.5f, -2.0f);
	glVertex3f(-1.8f, 3.5f, -1.8f);
	glVertex3f(-1.8f, 0.2f, -1.8f);


	glVertex3f(1.8f, 0.2f, -2.0f);
	glVertex3f(1.8f, 3.5f, -2.0f);
	glVertex3f(1.8f, 3.5f, -1.8f);
	glVertex3f(1.8f, 0.2f, -1.8f);

	glVertex3f(-1.8f, 3.5f, -2.0f);
	glVertex3f(-1.8f, 3.5f, -1.8f);
	glVertex3f(1.8f, 3.5f, -1.8f);
	glVertex3f(1.8f, 3.5f, -2.0f);
	glEnd();
	glPopMatrix();
}

void drawRoom() {
	//right wall
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(room_size / 2, 0, 0);
	drawWall(room_size);
	glPopMatrix();

	//front wall
	glPushMatrix();
	glColor3f(1, 1, 0);
	glTranslatef(0, 0, (room_size / 2));
	glRotatef(90, 0, 1, 0);
	drawWall(room_size);
	glPopMatrix();

	//back wall
	glPushMatrix();
	glColor3f(1, 1, 0);
	glTranslatef(0, 0, -(room_size / 2));
	glRotatef(90, 0, 1, 0);
	drawWall(room_size);
	glPopMatrix();

	//left wall
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(-room_size / 2, 0, 0);
	drawWall(room_size);
	glPopMatrix();

	//Floor
	glPushMatrix();
	glColor3f(0, 0, 0);
	glTranslatef(room_size / 2, 0, 0);
	glRotatef(90, 0, 0, 1);
	drawWall(room_size);
	glPopMatrix();
}

void drawDoor() {
	glPushMatrix();
	glColor3f(1, 0, 0);
	glTranslated(0, 4.5, 11);
	glScaled(3.5, 5, 0.2);
	glutSolidCube(1.0);
	glPopMatrix();
}

//Drawing Table 

void drawPockets(int table_length, int table_width, int table_height, float table_thickness, float pocket_radius) {
	glColor3f(1, 1, 1);
	//MR
	glPushMatrix();
	glTranslatef(3, table_height + 0.2, 0);
	glRotatef(90, 1, 0, 0);
	drawCircle(0, 0, pocket_radius, 32);
	glPopMatrix();

	//ML
	glPushMatrix();
	glTranslatef(-3, table_height + 0.2, 0);
	glRotatef(90, 1, 0, 0);
	drawCircle(0, 0, pocket_radius, 32);
	glPopMatrix();

	//TL
	glPushMatrix();
	glTranslatef(-3, table_height + 0.2, -4);
	glRotatef(90, 1, 0, 0);
	drawCircle(0, 0, pocket_radius, 32);
	glPopMatrix();

	//TR
	glPushMatrix();
	glTranslatef(3, table_height + 0.2, -4);
	glRotatef(90, 1, 0, 0);
	drawCircle(0, 0, pocket_radius, 32);
	glPopMatrix();

	//LB
	glPushMatrix();
	glTranslatef(-3, table_height + 0.2, 4);
	glRotatef(90, 1, 0, 0);
	drawCircle(0, 0, pocket_radius, 32);
	glPopMatrix();

	//RB
	glPushMatrix();
	glTranslatef(3, table_height + 0.2, 4);
	glRotatef(90, 1, 0, 0);
	drawCircle(0, 0, pocket_radius, 32);
	glPopMatrix();
}

void drawTableLegs(int table_length, int table_width, int table_height) {
	glPushMatrix();
	glTranslatef(0, 2, 0);
	glColor3f(0.2, 0, 0.4);
	glBegin(GL_POLYGON);
	glVertex3f(-3, 2.1, 4);
	glVertex3f(3, 2.2, 4);
	glVertex3f(2, 1, 4);
	glVertex3f(-2, 1, 4);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 2, -8);
	glColor3f(0.2, 0, 0.4);
	glBegin(GL_POLYGON);
	glVertex3f(-3, 2.1, 4);
	glVertex3f(3, 2.2, 4);
	glVertex3f(2, 1, 4);
	glVertex3f(-2, 1, 4);
	glEnd();
	glPopMatrix();
}

void drawTableTop(int table_length, int table_width, int table_height, float table_thickness) {
	glColor3f(0, 1, 0);
	//glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
	//glEnable(GL_TEXTURE_GEN_T);
	//glBindTexture(GL_TEXTURE_2D, tex);
	glPushMatrix();
	glTranslatef(0, table_height, 0);
	glScalef(table_width, table_thickness, table_length);
	glutSolidCube(1);
	glPopMatrix();
	//glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
	//glDisable(GL_TEXTURE_GEN_T);


}

void drawTableBorder(int table_length, int table_width, int table_height, float pocket_radius) {
	glColor3f(0.5, 0, 0);

	//front
	glPushMatrix();
	glTranslatef(0, table_height, table_length / 2);
	glScalef(table_width - (pocket_radius * 2), 0.4, 0.4);
	glutSolidCube(1);
	glPopMatrix();

	//back
	glPushMatrix();
	glTranslatef(0, table_height, -table_length / 2);
	glScalef(table_width - (pocket_radius * 2), 0.4, 0.4);
	glutSolidCube(1);
	glPopMatrix();

	//left top
	glPushMatrix();
	glTranslatef(-table_width / 2, table_height, -table_length / 4);
	glRotatef(90, 0, 1, 0);
	glScalef(table_length / 2 - (pocket_radius * 2), 0.4, 0.4);
	glutSolidCube(1);
	glPopMatrix();

	//left bottom
	glPushMatrix();
	glTranslatef(-table_width / 2, table_height, table_length / 4);
	glRotatef(90, 0, 1, 0);
	glScalef(table_length / 2 - (pocket_radius * 2), 0.4, 0.4);
	glutSolidCube(1);
	glPopMatrix();

	//right top
	glPushMatrix();
	glTranslatef(table_width / 2, table_height, -table_length / 4);
	glRotatef(90, 0, 1, 0);
	glScalef(table_length / 2 - (pocket_radius * 2), 0.4, 0.4);
	glutSolidCube(1);
	glPopMatrix();

	//right botttom
	glPushMatrix();
	glTranslatef(table_width / 2, table_height, table_length / 4);
	glRotatef(90, 0, 1, 0);
	glScalef(table_length / 2 - (pocket_radius * 2), 0.4, 0.4);
	glutSolidCube(1);
	glPopMatrix();
}

//Different shots

void drawBallsStart(float table_length, float table_width, float table_height, float table_thickness) {
	if (beforeHit == true) {
		setStartingPositions();
		cueBallPosition.before_x = 0;
		cueBallPosition.before_z = 2;
		cueBallPosition.after_x = 0;
		cueBallPosition.after_z = 0;


		cueStickPosition.before_x = 0;
		cueStickPosition.before_z = 0;
		cueStickPosition.after_x = 0;
		cueStickPosition.after_z = -1;

		beforeHit = false;
	}
	drawCueStick(cueStickPosition.before_x,cueStickPosition.before_z);
	drawCueBall(cueBallPosition.before_x, cueBallPosition.before_z, 0.1f, table_height, table_thickness);
	for (int i = 0; i < NUM_BALLS; ++i) {
		drawBall(balls[i].before_x, balls[i].before_z, 0.1f, table_height, table_thickness,balls[i].color); // Adjust radius as needed
	}
}

void streightShot(float table_length, float table_width, float table_height, float table_thickness) {
	if (beforeHit == true) {
		setStrightShot();
		beforeHit = false;
	}
	drawCueStick(cueStickPosition.before_x, cueStickPosition.before_z);
	drawCueBall(cueBallPosition.before_x, cueBallPosition.before_z, 0.1f, table_height, table_thickness);
	for (int i = 0; i < NUM_BALLS; ++i) {
		if (balls[i].before_x != 0 && balls[i].before_z != 0) {
			drawBall(balls[i].before_x, balls[i].before_z, 0.1f, table_height, table_thickness,balls[i].color); // Adjust radius as needed
		}
	}
}

void carromShot(float table_length, float table_width, float table_height, float table_thickness) {
	if (beforeHit == true) {
		setCarromShot();
		beforeHit = false;
	}
	drawCueStick(cueStickPosition.before_x, cueStickPosition.before_z);
	drawCueBall(cueBallPosition.before_x, cueBallPosition.before_z, 0.1f, table_height, table_thickness);
	for (int i = 0; i < NUM_BALLS; ++i) {
		if (balls[i].before_x != 0 && balls[i].before_z != 0) {
			drawBall(balls[i].before_x, balls[i].before_z, 0.1f, table_height, table_thickness, balls[i].color); // Adjust radius as needed
		}
	}
}

void updateBallPositions(float deltaTime) {

	// Move the cue ball based on its velocity
	cueBallPosition.before_x = lerp(cueBallPosition.before_x, cueBallPosition.after_x, deltaTime);
	cueBallPosition.before_z = lerp(cueBallPosition.before_z, cueBallPosition.after_z, deltaTime);
	cueStickPosition.before_x = lerp(cueStickPosition.before_x, cueStickPosition.after_x, deltaTime);
	cueStickPosition.before_z = lerp(cueStickPosition.before_z, cueStickPosition.after_z, deltaTime);

	drawCueBall(cueBallPosition.before_x, cueBallPosition.before_z, 0.1f, 4, 0.2);
	drawCueStick(cueStickPosition.before_x, cueStickPosition.before_z);

	//// Move the other balls based on their velocities
	for (int i = 0; i < NUM_BALLS; ++i) {
		if (balls[i].before_x != 0 && balls[i].before_z != 0) {
			//balls[i].before_x = balls[i].after_x;
			//balls[i].before_z = balls[i].after_z;
			/*printf("Before : %f", balls[i].before_x);*/
			balls[i].before_x = lerp(balls[i].before_x, balls[i].after_x, deltaTime);
			//printf("After : %f", balls[i].before_x);
			balls[i].before_z = lerp(balls[i].before_z, balls[i].after_z, deltaTime);

			drawBall(balls[i].before_x, balls[i].before_z, 0.1f, 4, 0.2, balls[i].color);
		}
	}
}

void hitByCue() {
	const int animationDuration = 3;  
	const float frameTime = 0.1f; 

	float elapsedTime;
	//updateBallPositions(frameTime);
	// Simulate animation for the specified duration
	for (elapsedTime = 0.1f; elapsedTime < animationDuration; elapsedTime += frameTime) {
		// Update ball positions with animation
		updateBallPositions(frameTime);

		//printf("%d %f \n", animationDuration, elapsedTime);
	}
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setLighting();

	glPushMatrix();
	// camera orientation (eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ)
	gluLookAt(0.0 + camX, 9.0 + camY, 5.0 + camZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


	// move the scene (all the rendered environment) using keyboard keys
	glTranslatef(sceTX, sceTY, sceTZ);
	glRotatef(sceRY, 0.0, 1.0, 0.0);

	//drawAxes();
	//drawGrid();

	int table_length = 8;
	int table_width = 6;
	int table_height = 4;
	float table_thickness = 0.2;
	float pocket_radius = 0.2;
	float ball_radius = 0.2;

	drawChair();
	drawTreeBunch();

	drawBoard();

	//drawGround();

	//drawRoof();
	//drawTableTopTexture();
	//drawFloorTexture();
	//drawPicture();
	drawDoor();

	glPushMatrix();
	glScalef(2, 0.8,1.5);
	//drawWallTexture();
	glPopMatrix();


	/*drawCeylingTexture();
	drawRoom();*/

	drawLamp();

	drawTableTop(table_length, table_width, table_height, table_thickness);
	drawPockets(table_length, table_width, table_height, table_thickness, pocket_radius);
	drawTableBorder(table_length, table_width, table_height, 0.2);
	drawTableLegs(table_length,table_width,table_height);

	if (hit == true) {
		hitByCue();
		method = 0;
	}
	if (method == 1) {
		drawBallsStart(table_length, table_width, table_height, table_thickness);
		
	}
	if (method == 2) {
		streightShot(table_length, table_width, table_height, table_thickness);
	}
	if (method == 3) {
		carromShot(table_length, table_width, table_height, table_thickness);
	}

	glPopMatrix();

	animationFactor++;
	glutSwapBuffers();
}

void Timer(int x) {
	rotation_factor += rotation_factor >= 360.0 ? -rotation_factor : 2;
	glutPostRedisplay();
	glutTimerFunc(60, Timer, 1);
}

void reshape(GLsizei w, GLsizei h) {
	glViewport(0, 0, w, h);
	GLfloat aspect_ratio = h == 0 ? w / 1 : (GLfloat)w / (GLfloat)h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//Define the Perspective projection frustum 
	// (FOV_in_vertical, aspect_ratio, z-distance to the near plane from the camera position, z-distance to far plane from the camera position)
	gluPerspective(120.0, aspect_ratio, 1.0, 100.0);

}

void keyboardSpecial(int key, int x, int y) {
	if (key == GLUT_KEY_UP)
		camY += 1;

	if (key == GLUT_KEY_DOWN)
		camY -= 1;

	if (key == GLUT_KEY_RIGHT)
		sceTX += 1;

	if (key == GLUT_KEY_LEFT)
		sceTX -= 1;

	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	if (key == 'l')
		objRY += 1;

	if (key == 'r')
		objRY -= 1;

	if (key == 'Z')
		sceTZ += 1;

	if (key == 'z')
		sceTZ -= 1;

	if (key == 'w')
		sceTX += 1;

	if (key == 's')
		sceTX -= 1;

	if (key == 'y')
		sceRY += 1;

	if (key == 'Y')
		sceRY -= 1;

	if (key == 'c')
		position = 0;

	if (key == 'o')
		position = 90;

	if (key == '8')
		camX += 1;

	if (key == '9')
		camX -= 1;

	if (key == '6')
		camZ += 1;

	if (key == '7')
		camZ -= 1;

	if (key == '!')
		glDisable(GL_LIGHT0); // Light at -x

	if (key == '1')
		glEnable(GL_LIGHT0);

	if (key == '@')
		glDisable(GL_LIGHT1); // Light at +x

	if (key == '2')
		glEnable(GL_LIGHT1);

	if (key == '1')
		method = 1;

	if (key == '2')
		method = 2;

	if (key == '3')
		method = 3;

	if (key == '4')
		method = 4;

	if (key == '`')
		hit = true;

	glutPostRedisplay();
}

int main(void) {
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Pool Game");
	glutTimerFunc(60.0, Timer, 1);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboardSpecial);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	init();
	glutMainLoop();
	return 0;
}