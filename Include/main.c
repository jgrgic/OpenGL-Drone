/*******************************************************************
           Multi-Part Model Construction and Manipulation
********************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gl/glut.h>
#include "Vector3D.h"
#include "QuadMesh.h"
#include "BuildingMesh.h"
#include "CubeMesh.h"
#include "TextureMap.h"

const int meshSize = 32;    // Default Mesh Size
GLint vWidth = 650;     // Viewport width in pixels
GLint vHeight = 500;    // Viewport height in pixelst viewWidth = vWidth;

static int currentButton;
static unsigned char currentKey;

//drone starting coordinates
static float xCoordinate = -20.0, yCoordinate = 15.0, zCoordinate = 0.0;

//starting coordinates for second drone
static float xCoordinate2 = 20.0, yCoordinate2 = 15.0, zCoordinate2 = 0.0;

//starting coordinates for the missile relative to the drone
static float xMissile = -20.0, yMissile = 15.0, zMissile = 0.0;

static float buildingX = -6.0, buildingY = 1.0, buildingZ = -6.0;

float radius = 22.0;
float cameraX = 0.0, cameraY = 6.0, cameraZ = 22.0;
float centerX = 0.0, centerY = 0.0, centerZ = 0.0;
float lookToFromDroneX = 0.0, lookToFromDroneY = 0.0, lookToFromDroneZ = 0.0;

static float scaleX = 1.0;
static float scaleY = 1.0;
static float scaleZ = 1.0;

static float propellerSpin = 0.0;
static float droneRotation = 0.0;
static float droneRotationOrigin = 0.0;
static float pi = 3.14159;

static bool propellerOn = false;
static bool spawnBuilding = false;
static bool moveBuilding = false;
static bool scaleHeight = false;
static bool scaleWidthDepth = false;
static bool doneMoving = false;
static bool loadFile = false;
static bool moveMissile = false;
static bool launchMissile = false;
static bool droneHitBuilding = false;
static bool missileHitDrone = false;
static bool droneHitGround = false;
static bool drawCollisionMissile = true;
static bool drawCollisionBuilding = true;
static bool drawCollisionGround = true;
static bool changeCamera = false;

// Lighting/shading and material properties for drone - upcoming lecture - just copy for now

// Light properties
static GLfloat light_position0[] = { -6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_position1[] = { 6.0F, 12.0F, 0.0F, 1.0F };
static GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };

// Material properties
static GLfloat drone_mat_ambient[] = { 0.4F, 0.2F, 0.0F, 1.0F };
static GLfloat drone_mat_specular[] = { 0.1F, 0.1F, 0.0F, 1.0F };
static GLfloat drone_mat_diffuse[] = { 0.9F, 0.5F, 0.0F, 1.0F };
static GLfloat drone_mat_shininess[] = { 0.0F };

// A quad mesh representing the ground
static QuadMesh groundMesh;

BuildingMesh *loadBuildings[50];
int buildingNum = 0;
int buildingHeight;

static Vector3D scale;
static Vector3D translate;

CubeMesh streets[20];
CubeMesh buildings[20];
CubeMesh drone[20];
CubeMesh drone2[20];

RGBpixmap roadMap;
RGBpixmap buildingMap[4];
RGBpixmap droneMap;
RGBpixmap droneMap2;

// Structure defining a bounding box, currently unused
//typedef struct BoundingBox {
	//  Vector3D min;
      //Vector3D max;
//} BBox;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void createBuilding();
void drawEntireDrone();
void drawEntireSecondDrone();
void newCamera();
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);

bool checkForDroneBuildingCollision(CubeMesh* drone, CubeMesh* building);
bool checkForDroneGroundCollision(CubeMesh* drone);
bool checkForShotDroneCollision(CubeMesh* drone, CubeMesh* shot);

float* calculateDroneBox(CubeMesh* drone);
float* calculateBuildingBox(CubeMesh* building);
float* calculateShotBox(CubeMesh* shot);

Vector3D ScreenToWorld(int x, int y);

int main(int argc, char **argv)
{
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(vWidth, vHeight);
    glutInitWindowPosition(200, 30);
    glutCreateWindow("Assignment 1");

    // Initialize GL
    initOpenGL(vWidth, vHeight);

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotionHandler);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(functionKeys);

    // Start event loop, never returns
    glutMainLoop();

    return 0;
}


// Set up OpenGL. For viewport and projection setup see reshape(). */
void initOpenGL(int w, int h)
{
    // Set up and enable lighting
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    //glEnable(GL_LIGHT1);   // This light is currently off

	//create streets for the city
	streets[0] = newCube(NewVector3D(0.0, 0.0, 0.0), NewVector3D(16.0, 0.5, 1.0), 0.0);
	streets[1] = newCube(NewVector3D(0.0, 0.0, 15.0), NewVector3D(16.0, 0.5, 1.0), 0.0);
	streets[2] = newCube(NewVector3D(0.0, 0.0, -15.0), NewVector3D(16.0, 0.5, 1.0), 0.0);
	streets[3] = newCube(NewVector3D(0.0, 0.0, 0.0), NewVector3D(16.0, 0.5, 1.0), 70.0);
	streets[4] = newCube(NewVector3D(-10.0, 0.0, 0.0), NewVector3D(16.0, 0.5, 1.0), 90.0);
	streets[5] = newCube(NewVector3D(6.0, 0.0, 0.0), NewVector3D(17.0, 0.5, 1.0), 120);

	//create buildings for the city
	buildings[0] = newCube(NewVector3D(-12.0, 0.0, -12.0), NewVector3D(1.0, 10.0, 1.0), 0.0);
	buildings[1] = newCube(NewVector3D(-12.0, 0.0, -9.0), NewVector3D(1.0, 7.0, 1.0), 0.0);
	buildings[2] = newCube(NewVector3D(-12.0, 0.0, -6.0), NewVector3D(1.0, 12.0, 1.0), 0.0);
	buildings[3] = newCube(NewVector3D(-12.0, 0.0, -3.0), NewVector3D(1.0, 4.0, 1.0), 0.0);
	buildings[4] = newCube(NewVector3D(-13.0, 0.0, 8.0), NewVector3D(2.0, 6.0, 5.0), 0.0);
	buildings[5] = newCube(NewVector3D(-5.0, 5.0, -10.0), NewVector3D(2.0, 14.0, 2.0), 45.0);
	buildings[6] = newCube(NewVector3D(-3.0, 5.0, -4.0), NewVector3D(2.0, 14.0, 2.0), 45.0);
	buildings[7] = newCube(NewVector3D(-7.0, 5.0, 8.0), NewVector3D(1.0, 6.0, 5.0), -10.0);
	buildings[8] = newCube(NewVector3D(3.0, 0.0, 11.0), NewVector3D(2.0, 3.0, 5.0), 90.0);
	buildings[9] = newCube(NewVector3D(3.0, 0.0, 5.0), NewVector3D(2.0, 6.0, 2.0), -45.0);
	buildings[10] = newCube(NewVector3D(13.0, 0.0, 5.0), NewVector3D(3.0, 8.0, 1.0), -40.0);
	buildings[11] = newCube(NewVector3D(12.0, 0.0, -4.0), NewVector3D(3.0, 13.0, 2.0), 0.0);
	buildings[12] = newCube(NewVector3D(12.0, 2.0, -10.0), NewVector3D(3.0, 15.0, 2.0), 0.0);
	buildings[13] = newCube(NewVector3D(7.0, 0.0, -8.0), NewVector3D(5.0, 8.0, 1.0), 90.0);

    // Other OpenGL setup
    glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
    glClearColor(0.6F, 0.6F, 0.6F, 0.0F);  // Color and depth for glClear
    glClearDepth(1.0f);
    glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	readBMPFile(&roadMap, "textures/asphalt.bmp");
	setTexture(&roadMap, 2000);
	readBMPFile(&buildingMap[0], "textures/gold.bmp");
	setTexture(&buildingMap[0], 2001);
	readBMPFile(&buildingMap[1], "textures/modric.bmp");
	setTexture(&buildingMap[1], 2002);
	readBMPFile(&buildingMap[2], "textures/silver.bmp");
	setTexture(&buildingMap[2], 2003);
	readBMPFile(&buildingMap[3], "textures/ryerson.bmp");
	setTexture(&buildingMap[3], 2004);
	readBMPFile(&droneMap, "textures/eagle.bmp");
	setTexture(&droneMap, 2005);
	readBMPFile(&droneMap2, "textures/eagle.bmp");
	setTexture(&droneMap2, 2006);

    // Set up ground quad mesh
    Vector3D origin = NewVector3D(-16.0f, 0.0f, 16.0f);
    Vector3D dir1v = NewVector3D(1.0f, 0.0f, 0.0f);
    Vector3D dir2v = NewVector3D(0.0f, 0.0f, -1.0f);
    groundMesh = NewQuadMesh(meshSize);
    InitMeshQM(&groundMesh, meshSize, origin, 32.0, 32.0, dir1v, dir2v);

    Vector3D ambient = NewVector3D(0.0f, 0.05f, 0.0f);
    Vector3D diffuse = NewVector3D(0.4f, 0.8f, 0.4f);
    Vector3D specular = NewVector3D(0.04f, 0.04f, 0.04f);
    SetMaterialQM(&groundMesh, ambient, diffuse, specular, 0.2);

    // Set up the bounding box of the scene
    // Currently unused. You could set up bounding boxes for your objects eventually.
    //Set(&BBox.min, -8.0f, 0.0, -8.0);
    //Set(&BBox.max, 8.0f, 6.0,  8.0);
}

float cameraRotation = 0;
int cameraRotationX = 0;

// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set drone material properties
    glMaterialfv(GL_FRONT, GL_AMBIENT, drone_mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, drone_mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, drone_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SHININESS, drone_mat_shininess);

	drawEntireDrone();
	drawEntireSecondDrone();
	
    // Draw ground mesh
    DrawMeshQM(&groundMesh, meshSize);

	if (propellerOn == true) {
		propellerSpin += 1.0;
		droneRotationOrigin += 0.01;
		glutPostRedisplay();
	}

	if (spawnBuilding == true) {
		createBuilding();
	}

	if (launchMissile == true) {
		xMissile += 0.01 * cosf(droneRotation * pi / 180);
		zMissile -= 0.01 * sinf(droneRotation * pi / 180);
		if (checkForShotDroneCollision(&drone2[0], &drone[10]) == true) {
			missileHitDrone = true;
		}
		glutPostRedisplay();
	}

	//draws and texture maps streets
	for (int i = 0; i < 6; i++) {
		drawCube(&streets[i]);
		setTextureMapID(&streets[i], 2000);
	}
	

	//draws and texture maps drone
	if (drawCollisionBuilding == true || drawCollisionGround == true) {
		for (int i = 0; i < 11; i++) {
			if (droneHitBuilding == true) {
				drawCollisionBuilding = false;
				printf("You hit a building! \n");
			}
			if (droneHitGround == true) {
				drawCollisionGround = false;
				printf("You crashed into the ground! \n");
			}
			setTextureMapID(&drone[i], 2005);
			drawDrone(&drone[i]);
		}
	}


	//draws and texture maps the second drone
	if (drawCollisionMissile == true) {
		for (int i = 0; i < 10; i++) {
			if (missileHitDrone == true) {
				drawCollisionMissile = false;
				printf("Nice shot! \n");
			}
			setTextureMapID(&drone2[i], 2006);
			drawDrone2(&drone2[i]);
		}
	}

	//draws and texture maps buildings
	for (int i = 0; i < 14; i++) {
		drawCube(&buildings[i]);
		if ((i % 2) == 0) {
			setTextureMapID(&buildings[i], 2001);
		}
		else if ((i % 3) == 0) {
			setTextureMapID(&buildings[i], 2003);
		}
		else if ((i % 4) == 1) {
			setTextureMapID(&buildings[i], 2004);
		}
		else {
			setTextureMapID(&buildings[i], 2002);
		}
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(cameraX, cameraY, cameraZ, centerX, centerY, centerZ, 0.0, 1.0, 0.0);
	glRotatef(cameraRotationX, 1.0, 0.0, 0.0);
	glRotatef(cameraRotation, 0.0, 1.0, 0.0);
	
	if (changeCamera == true) {
		newCamera();
	}

    glutSwapBuffers();   // Double buffering, swap buffers
}

void newCamera() {
	gluLookAt(xCoordinate, yCoordinate, zCoordinate, lookToFromDroneX, lookToFromDroneY, lookToFromDroneZ, 0.0, 1.0, 0.0);
}

void updateCameraPosition() {
	glRotatef(cameraRotation, 0.0, 1.0, 0.0);
}

void createBuilding()
{
	glPushMatrix();
	glTranslatef(buildingX, buildingY, buildingZ);
	glScalef(scaleX, scaleY, scaleZ);
	glutSolidCube(1.0);
	glPopMatrix();
}

void drawEntireDrone()
{
	drone[0] = newDronePart(NewVector3D(xCoordinate, yCoordinate, zCoordinate), NewVector3D(2.0, 0.5, 0.5), NewVector3D(0.0, 0.0, 0.0), droneRotation - 90, 0.0);
	drone[1] = newDronePart(NewVector3D(xCoordinate, yCoordinate, zCoordinate), NewVector3D(2.0, 0.5, 0.5), NewVector3D(0.0, 0.0, 0.0), droneRotation, 0.0);
	drone[2] = newDronePart(NewVector3D(xCoordinate, yCoordinate, zCoordinate), NewVector3D(0.5, 1.0, 0.5), NewVector3D(2.0, 0.0, 0.0), droneRotation, 0.0);
	drone[3] = newDronePart(NewVector3D(xCoordinate, yCoordinate, zCoordinate), NewVector3D(0.5, 1.0, 0.5), NewVector3D(-2.0, 0.0, 0.0), droneRotation, 0.0);
	drone[4] = newDronePart(NewVector3D(xCoordinate, yCoordinate, zCoordinate), NewVector3D(0.5, 1.0, 0.5), NewVector3D(0.0, 0.0, 2.0), droneRotation, 0.0);
	drone[5] = newDronePart(NewVector3D(xCoordinate, yCoordinate, zCoordinate), NewVector3D(0.5, 1.0, 0.5), NewVector3D(0.0, 0.0, -2.0), droneRotation, 0.0);
	drone[6] = newDronePart(NewVector3D(xCoordinate, yCoordinate, zCoordinate), NewVector3D(0.5, 0.3, 1.5), NewVector3D(2.0, 1.0, 0.0), droneRotation, propellerSpin);
	drone[7] = newDronePart(NewVector3D(xCoordinate, yCoordinate, zCoordinate), NewVector3D(0.5, 0.3, 1.5), NewVector3D(-2.0, 1.0, 0.0), droneRotation, propellerSpin);
	drone[8] = newDronePart(NewVector3D(xCoordinate, yCoordinate, zCoordinate), NewVector3D(0.5, 0.3, 1.5), NewVector3D(0.0, 1.0, 2.0), droneRotation, propellerSpin);
	drone[9] = newDronePart(NewVector3D(xCoordinate, yCoordinate, zCoordinate), NewVector3D(0.5, 0.3, 1.5), NewVector3D(0.0, 1.0, -2.0), droneRotation, propellerSpin);
	drone[10] = newDronePart(NewVector3D(xMissile, yMissile, zMissile), NewVector3D(1.0, 0.5, 0.25), NewVector3D(0.0, 0.0, 0.0), droneRotation, 0.0);
}

void drawEntireSecondDrone() {
	drone2[0] = newDronePart2(NewVector3D(xCoordinate2, yCoordinate2, zCoordinate2), NewVector3D(2.0, 0.5, 0.5), NewVector3D(0.0, 0.0, 0.0), droneRotation - 90, 0.0, droneRotationOrigin);
	drone2[1] = newDronePart2(NewVector3D(xCoordinate2, yCoordinate2, zCoordinate2), NewVector3D(2.0, 0.5, 0.5), NewVector3D(0.0, 0.0, 0.0), droneRotation, 0.0, droneRotationOrigin);
	drone2[2] = newDronePart2(NewVector3D(xCoordinate2, yCoordinate2, zCoordinate2), NewVector3D(0.5, 1.0, 0.5), NewVector3D(2.0, 0.0, 0.0), droneRotation, 0.0, droneRotationOrigin);
	drone2[3] = newDronePart2(NewVector3D(xCoordinate2, yCoordinate2, zCoordinate2), NewVector3D(0.5, 1.0, 0.5), NewVector3D(-2.0, 0.0, 0.0), droneRotation, 0.0, droneRotationOrigin);
	drone2[4] = newDronePart2(NewVector3D(xCoordinate2, yCoordinate2, zCoordinate2), NewVector3D(0.5, 1.0, 0.5), NewVector3D(0.0, 0.0, 2.0), droneRotation, 0.0, droneRotationOrigin);
	drone2[5] = newDronePart2(NewVector3D(xCoordinate2, yCoordinate2, zCoordinate2), NewVector3D(0.5, 1.0, 0.5), NewVector3D(0.0, 0.0, -2.0), droneRotation, 0.0, droneRotationOrigin);
	drone2[6] = newDronePart2(NewVector3D(xCoordinate2, yCoordinate2, zCoordinate2), NewVector3D(0.5, 0.3, 1.5), NewVector3D(2.0, 1.0, 0.0), droneRotation, propellerSpin, droneRotationOrigin);
	drone2[7] = newDronePart2(NewVector3D(xCoordinate2, yCoordinate2, zCoordinate2), NewVector3D(0.5, 0.3, 1.5), NewVector3D(-2.0, 1.0, 0.0), droneRotation, propellerSpin, droneRotationOrigin);
	drone2[8] = newDronePart2(NewVector3D(xCoordinate2, yCoordinate2, zCoordinate2), NewVector3D(0.5, 0.3, 1.5), NewVector3D(0.0, 1.0, 2.0), droneRotation, propellerSpin, droneRotationOrigin);
	drone2[9] = newDronePart2(NewVector3D(xCoordinate2, yCoordinate2, zCoordinate2), NewVector3D(0.5, 0.3, 1.5), NewVector3D(0.0, 1.0, -2.0), droneRotation, propellerSpin, droneRotationOrigin);
}

// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
    // Set up viewport, projection, then change to modelview matrix mode - 
    // display function will then set up camera and do modeling transforms.
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLdouble)w / h, 0.2, 40.0);

    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();

    // Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
    //gluLookAt(cameraX, cameraY, cameraZ, centerX, centerY, centerZ, 0.0, 1.0, 0.0);
	//glRotatef(cameraRotation, 0.0, 1.0, 0.0);
}

// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'b':
		//updates new backward direction of drone
		xCoordinate -= 1.0 * cosf(droneRotation * pi / 180);
		zCoordinate += 1.0 * sinf(droneRotation * pi / 180);
		xMissile -= 1.0 * cosf(droneRotation * pi / 180);
		zMissile += 1.0 * sinf(droneRotation * pi / 180);
		lookToFromDroneX -= 1.0 * cosf(droneRotation * pi / 180);
		lookToFromDroneZ += 1.0 * sinf(droneRotation * pi / 180);
		for (int i = 0; i < 14; i++) {
			if (checkForDroneBuildingCollision(&drone[0], &buildings[i]) == true) {
				droneHitBuilding = true;
			}
		}
		break;
	
	case 's':
		//turns propellers on
		propellerOn = true;
		break;

	case 'f':
		//updates new forward direction of drone
		xCoordinate += 1.0 * cosf(droneRotation * pi / 180);
		zCoordinate -= 1.0 * sinf(droneRotation * pi / 180);
		xMissile += 1.0 * cosf(droneRotation * pi / 180);
		zMissile -= 1.0 * sinf(droneRotation * pi / 180);
		lookToFromDroneX += 1.0 * cosf(droneRotation * pi / 180);
		lookToFromDroneZ -= 1.0 * sinf(droneRotation * pi / 180);
		for (int i = 0; i < 14; i++) {
			if (checkForDroneBuildingCollision(&drone[0], &buildings[i]) == true) {
				droneHitBuilding = true;
			}
		}
		break;

	case 'm':
		launchMissile = true;
		moveMissile = true;
		break;
	
	case 'h':
		printf("Turn on propellers = s \n");
		printf("Forward movement = f \n");
		printf("Backward movement = b \n");
		printf("Shoot missile = m \n");
		printf("Turn right = right arrow key \n");
		printf("Turn left = left arrow key \n");
		printf("Ascend = up arrow key \n");
		printf("Descend = down arrow key \n");
		printf("Change camera = c \n");
		break;

	case 'c':
		changeCamera = true;
		break;

	/*case 't':
		moveBuilding = true;
		scaleHeight = false;
		scaleWidthDepth = false;
		break;

	case 'h':
		scaleHeight = true;
		moveBuilding = false;
		scaleWidthDepth = false;
		break;

	case 'm':
		scaleWidthDepth = true;
		scaleHeight = false;
		moveBuilding = false;
		break;

	case 'e':
		doneMoving = true;
		scaleWidthDepth = false;
		moveBuilding = false;
		scaleHeight = false;
		spawnBuilding = false;
		int func = 0;
		printf("Select the function you would like applied to this building: \n 1. Linear\n 2. Sin()\n 3. No function");
		scanf("%d", &func);
		printf("Building will be scaled by %d", func);
		buildings[buildingNum] = createBuildingMesh(NewVector3D(scaleX, scaleY, scaleZ), NewVector3D(buildingX, buildingY, buildingZ), 0.5, buildingY, func);
		buildingNum++;
		break;*/

	}

    glutPostRedisplay();   // Trigger a window redisplay
}

// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
    // Help key
    if (key == GLUT_KEY_F7)
    {
		//spawnBuilding = true;
		//moveBuilding = false;
		//scaleHeight = false;
		//scaleWidthDepth = false;
		//doneMoving = false;
	}
    // Do transformations with arrow keys
    else if (key == GLUT_KEY_DOWN)   // GLUT_KEY_DOWN, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_LEFT
    {
		yCoordinate -= 1.0;
		lookToFromDroneY -= 1.0;
		yMissile -= 1.0;
		for (int i = 0; i < 11; i++) {
			if (checkForDroneGroundCollision(&drone[i]) == true) {
				droneHitGround = true;
			}
		}
		if (moveBuilding == true) {
			buildingZ += 1.0;
		}

		if (scaleHeight == true) {
			//entire height would go down, must be greater than 0
			scaleY -= 1.0;
			buildingY -= 0.5;
		}

		if (scaleWidthDepth == true) {
			//face closest to the screen would be the only one moving
			scaleZ -= 1.0;
			buildingZ += 0.5;
		}
    }
	else if (key == GLUT_KEY_UP)
	{
		yCoordinate += 1.0;
		lookToFromDroneY += 1.0;
		yMissile += 1.0;
		if (moveBuilding == true) {
			buildingZ -= 1.0;
		}
		if (scaleHeight == true) {
			//entire height would go up
			scaleY += 1.0;
			buildingY += 0.5;
		}
		if (scaleWidthDepth == true) {
			//face farthest from the screen would be the only one moving
			scaleZ += 1.0;
			buildingZ -= 0.5;
		}
	}
	else if (key == GLUT_KEY_RIGHT)
	{
		droneRotation -= 1.0;
		if (moveBuilding == true) {
			buildingX += 1.0;
		}
		if (scaleWidthDepth == true) {
			//right most face would be the only one moving
			scaleX += 1.0;
			buildingX += 0.5;
		}
	}
	else if (key == GLUT_KEY_LEFT)
	{
		droneRotation += 1.0;
		if (moveBuilding == true) {
			buildingX -= 1.0;
		}
		if (scaleWidthDepth == true) {
			//left most face would be the only one moving
			scaleX -= 1.0;
			buildingX -= 0.5;
		}
	}

    glutPostRedisplay();   // Trigger a window redisplay
}

int oldX, oldY;

// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
    currentButton = button;

    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN)
        {
			oldX = x;
			oldY = y;
		}
        break;
    case GLUT_RIGHT_BUTTON:
        if (state == GLUT_DOWN)
        {
            ;
        }
        break;
    default:
        break;
    }

	if (button == 3) {
		cameraZ += 1.0;
	}
	if (button == 4) {
		cameraZ -= 1.0;
	}

    glutPostRedisplay();   // Trigger a window redisplay
}

// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{

    if (currentButton == GLUT_LEFT_BUTTON)
    {
		if (xMouse > oldX) {
			cameraRotation += 1.0;
		}
		if (xMouse < oldX) {
			cameraRotation -= 1.0;
		}
		if (yMouse > oldY) {
			cameraRotationX += 1;
		}
		if (yMouse < oldY) {
			cameraRotationX -= 1;
		}

		oldX = xMouse;
		oldY = yMouse;
    }


    glutPostRedisplay();   // Trigger a window redisplay
}

Vector3D ScreenToWorld(int x, int y)
{
    // you will need to finish this if you use the mouse
    return NewVector3D(0, 0, 0);
}

//checks for collision between drone and building
bool checkForDroneBuildingCollision(CubeMesh* drone, CubeMesh* building) {

	float* droneBox = calculateDroneBox(drone);
	float* buildingBox = calculateBuildingBox(building);

	float droneBounds[] = { droneBox[0], droneBox[1], droneBox[2], droneBox[3], droneBox[4], droneBox[5] };
	float buildingBounds[] = { buildingBox[0], buildingBox[1], buildingBox[2], buildingBox[3], buildingBox[4], buildingBox[5] };

	free(droneBox);
	free(buildingBox);
	
	bool collision = false;

	if (droneBounds[0] < buildingBounds[1] && droneBounds[1] > buildingBounds[0])
		if (droneBounds[4] < buildingBounds[5] && droneBounds[5] > buildingBounds[4])
			collision = true;

	return collision;
}

bool checkForDroneGroundCollision(CubeMesh* drone) {
	
	float* droneBox = calculateDroneBox(drone);

	float droneBounds[] = { droneBox[0], droneBox[1], droneBox[2], droneBox[3], droneBox[4], droneBox[5] };

	free(droneBox);

	bool collision = false;

	if (droneBounds[2] < 0)
		collision = true;

	return collision;
}

//checks for collision between fired shot and target
bool checkForShotDroneCollision(CubeMesh* drone, CubeMesh* shot) {

	float* droneBox = calculateDroneBox(drone);
	float* shotBox = calculateShotBox(shot);

	float droneBounds[] = { droneBox[0], droneBox[1], droneBox[2], droneBox[3], droneBox[4], droneBox[5] };
	float shotBounds[] = { shotBox[0], shotBox[1], shotBox[2], shotBox[3], shotBox[4], shotBox[5] };

	free(droneBox);
	free(shotBox);

	bool collision = false;

	if (droneBounds[0] < shotBounds[1] && droneBounds[1] > shotBounds[0])
		if (droneBounds[4] < shotBounds[5] && droneBounds[5] > shotBounds[4])
			collision = true;

	return collision;
}

//calculates drone bounding box
float* calculateDroneBox(CubeMesh* drone) {

	float xmin, xmax;
	float ymin, ymax;
	float zmin, zmax;

	xmin = drone->tx - drone->sfx;
	xmax = drone->tx + drone->sfx;
	
	ymin = drone->ty - drone->sfy;
	ymax = drone->ty + drone->sfy;

	zmin = drone->tz - drone->sfz;
	zmax = drone->tz + drone->sfz;

	float* boundaries = (float*)malloc(6 * sizeof(float));

	boundaries[0] = xmin;
	boundaries[1] = xmax;
	boundaries[2] = ymin;
	boundaries[3] = ymax;
	boundaries[4] = zmin;
	boundaries[5] = zmax;

	return boundaries;
}

//calculates building bounding box
float* calculateBuildingBox(CubeMesh* building) {

	float xmin, xmax;
	float ymin, ymax;
	float zmin, zmax;

	xmin = building->tx - building->sfx;
	xmax = building->tx + building->sfx;

	ymin = building->ty - building->sfy;
	ymax = building->ty + building->sfy;

	zmin = building->tz - building->sfz;
	zmax = building->tz + building->sfz;

	float* boundaries = (float*)malloc(6 * sizeof(float));

	boundaries[0] = xmin;
	boundaries[1] = xmax;
	boundaries[2] = ymin;
	boundaries[3] = ymax;
	boundaries[4] = zmin;
	boundaries[5] = zmax;

	return boundaries;
}

//calculates fired shot bounding box
float* calculateShotBox(CubeMesh* shot) {

	float xmin, xmax;
	float ymin, ymax;
	float zmin, zmax;

	xmin = shot->tx - shot->sfx;
	xmax = shot->tx + shot->sfx;

	ymin = shot->ty - shot->sfy;
	ymax = shot->ty + shot->sfy;

	zmin = shot->tz - shot->sfz;
	zmax = shot->tz + shot->sfz;

	float* boundaries = (float*)malloc(6 * sizeof(float));

	boundaries[0] = xmin;
	boundaries[1] = xmax;
	boundaries[2] = ymin;
	boundaries[3] = ymax;
	boundaries[4] = zmin;
	boundaries[5] = zmax;

	return boundaries;
}





