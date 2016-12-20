#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

//#include "boilerplateClass.h"	//include our own class
#include <iostream>
//#include <cstdlib>
#include <ctime>
#include <vector>
#include <cmath>
#include <math.h>       /* fabs */

using namespace std;
using std::vector;

/*
Student Name: Juan Carlos Santana Penner
Student #: 001411625
Student ID: santanjc
3GC3 Assignment 2
-implemented hypsometric, fault algorithm, and camera(bonus) moving for extras
*/

//algorithms for generating map are from http://www.lighthouse3d.com/

//heightMap 2d array
vector< vector<float> > heightMap;
int height = 50;
int width = 50;

//camera positiong
float pos[] = {10,1,10};
float rot[] = {0, 0, 0};
float headRot[] = {0, 10, 0};
float camPos[] = {-height -50 , 20 , -height- 50};
float angle = 0.0f;

bool useFaultAlg = false;	//for using the faut algorithm or not

//rotating the map
int mapY = 0; //rotate about the y-axis
int mapX = 0; //rotate about the x-axis

int maxY; 		// the maximum y value in the map

//define light properties
//light 0 
float light0pos[4] = {0, 10, 0, 1};
float light0amb[4] = {0, 0, 0, 1};  
float light0dif[4] = {1, 1, 1, 1};
float light0spc[4] = {1, 1, 1, 1};

//light 1
float light1pos[4] = {height, 10, height, 1};
float light1amb[4] = {0, 0, 0, 1};  
float light1dif[4] = {1, 1, 1, 1};
float light1spc[4] = {1, 1, 1, 1};

//showing the height map differently 
bool showWireFrame = false;
bool showSolidPoly = true;
bool showLighitng = false;
bool showHypsometric = false;
bool flatShading = true;
bool drawQuads = true;

//hypsometric colors
float darkGreen[3] =  {0, 0.392157, 0};	//dark green
float lightGreen[3] = {0.498039, 1, 0};	//light green
float yellow[3] = { 1,1,0 };			//YELLOW
float orange[3] = {1, 0.647059, 0};		//orange
float red[3] = {1,0,0};					//red


//get the distance from the circle center
float distCircleCenter(float x0,  float z0, float x1,  float z1 ){
	float distance = 0.0;
	distance = sqrt( pow((x1-x0),2.0) + pow( (z1-z0),2.0)  );
	return distance;
}

//make a circle or 'bump' on the map
void genCircle(){
	//random position
	int xRan = rand() % height;		//random x
	int zRan = rand() % height; 	//random z

	int circleSize = rand() % 15 + 8;		//defines the circle size
	int disp = rand()% 2 +1;			//defines the maximum height variation

	float dist;						//distance

	//the center of the circle
	int centerX = xRan;			
	int centerZ = zRan;

	//for each terrain point (tx, tz)  
	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){

			//x0,z0,x1,z1
			dist = distCircleCenter(i,j,centerX,centerZ);	//distance from circle center
			float pd = (dist * 2)/circleSize;

			if (fabs(pd) <= 1.0 ){
				heightMap[i][j] += (disp/2) + cos(pd*3.14)*(disp/2) ;
			}
		}

	}
}


//get the maximum y of the height map
void calcMaxY(){

	maxY = 0;

	for (int i = 0; i<height; i++){
		for (int j = 0; j < height; j++){

			if (heightMap[i][j] > maxY){
				maxY = heightMap[i][j];
			}

		}
	}

}

void createMap(){

	//make the size of the height map
	heightMap.resize(height);			

	for (int i = 0; i < height; i++){
		heightMap[i].resize(width);		
	}

	//initialize values for heightMap
	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){
			heightMap[i][j] = 0.0;
		}
	}

	//printf("initialize done \n");

	if (useFaultAlg == false) {

		//printf("start terrain point algorithm \n");
		//run terrain point algorithm
		srand((int) time(0));	//randomizer seed
		int iterations = 4*height;

		for (int i = 0; i < iterations; i++){
			genCircle();
		}

	}else if (useFaultAlg == true){

		srand((int) time(0));	//randomizer seed
		int numIter = height*2;
		for (int t= 0; t < numIter; ++t){

			float v = (float)rand();
			float a = sin(v);
			float b = cos(v);
			float d = sqrt(height*height + height*height);
			// rand() / RAND_MAX gives a random number between 0 and 1.
			// therefore c will be a random number between -d/2 and d/2
			float c = ( (float) rand() / RAND_MAX) * d - d/2.0;
			float displacement = 1;

			for (int i = 0; i < height; i++ ){
				for (int j = 0; j < height; j++){
					if ( (a*i) + (b*j) - c > 0){
						//dist = a*i + b* - c;

						heightMap[i][j] += displacement;
					}else {
						heightMap[i][j] -= displacement;
					}

				}

			}
		}

	}

	//printf("start calculating: \n");

	calcMaxY();

}

vector<float> calcNorm(float x1, float y1, float z1, float x2, float y2, float z2,float x3, float y3, float z3  ){

	//cross product between vectors that meet at a vertex
	//vector U, point 2 - point 1
	float Ux = x2 - x1;
	float Uy = y2 - y1;
	float Uz = z2 - z1;

	//vecotor V, point 3 - point 1
	float Vx = x3 - x1;
	float Vy = y3 - y1;
	float Vz = z3 - z1;

	//the normal values
	float noramlx =  Uy*Vz - Uz*Vy; 
	float noramly =   Uz*Vx - Ux*Vz;
	float noramlz =   Ux*Vy - Uy*Vx;

	//get the magnitude of the normal
	float magnitude = sqrt(noramlx*noramlx + noramly*noramly + noramlz*noramlz );

	//normalize
	noramlx = noramlx/magnitude;
	noramly = noramly/magnitude;
	noramlz = noramlz/magnitude;

	vector< float > normal;
	normal.resize(3);
	normal[0]= -1*noramlx;
	normal[1]= -1*noramly;
	normal[2]= -1*noramlz;
	return normal;				//return unit normal
}

//the method draws all the triangular maps
void drawMapTriag(){

	//if you want to show the wiremap
	if (showWireFrame == true){

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glColor3f(1,0,0);	//red

		glBegin(GL_TRIANGLES);
			for (int i = 0; i < height -1 ; i++){
				for (int j = 0; j < width -1; j++){
					//first triangle
					glVertex3f(i,heightMap[i][j],j);
					glVertex3f(i+1,heightMap[i+1][j+1],j+1);
					glVertex3f(i,heightMap[i][j+1],j + 1);
					
					glVertex3f(i,heightMap[i][j],j);
					glVertex3f(i+1,heightMap[i+1][j],j);
					glVertex3f(i+1,heightMap[i+1][j+1],j+1);
				}
			}

		glEnd();
	} 


	//for showing solid polygons
	if (showSolidPoly == true){


		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		
		//if there is no lighting
		if (showLighitng == false){

			glBegin(GL_TRIANGLES);

			//if the map is not hypsometri 	
			if (showHypsometric == false){

				//vector that will hold the color values of all the vertices
				vector< vector <float> > vertexColors;
				//initialize sizes
				vertexColors.resize(height);
				for (int i = 0; i< height; i++){
					vertexColors[i].resize(height);
				}

				for (int i = 0; i < height; i++){
					for (int j = 0; j < height; j++){
						vertexColors[i][j] = heightMap[i][j]/maxY;		//determines the color of the vertex
					}
				}

				//for each point on the map
				for (int i = 0; i < height -1 ; i++){
					for (int j = 0; j < width -1; j++){
						
						float colour1 = vertexColors[i][j];
						float colour2 = vertexColors[i+1][j];
						float colour3 = vertexColors[i+1][j+1];
						float colour4 = vertexColors[i][j+1];

						//vertices, triangle 1
						glColor3f(colour1, colour1, colour1);
						glVertex3f(i,heightMap[i][j],j);

						glColor3f(colour2, colour2, colour2);
						glVertex3f(i+1,heightMap[i+1][j],j);

						glColor3f(colour3, colour3, colour3);
						glVertex3f(i+1,heightMap[i+1][j+1],j+1);
						
						//vertices, triangle 2
						glColor3f(colour3, colour3, colour3);
						glVertex3f(i+1,heightMap[i+1][j+1],j+1);

						glColor3f(colour4, colour4, colour4);
						glVertex3f(i,heightMap[i][j+1],j + 1);

						glColor3f(colour1, colour1, colour1);
						glVertex3f(i,heightMap[i][j],j);
			
					}
				}
			
			}else if (showHypsometric == true){
				//if you do want to show the hypsometric map
				
				//vector that will hold the color values of all the vertices
				vector< vector <vector< float> > > vertexColors;
				//resize the vectors
				vertexColors.resize(height);
				for (int i = 0; i< height; i++){
					vertexColors[i].resize(height);
				}
				for (int i = 0; i < height; i++){
					for (int j = 0; j < height; j++){
						vertexColors[i][j].resize(3);
					}
				}

				//for each vertx in the map
				for (int i = 0; i < height; i++){
					for (int j = 0; j < height; j++){
						
						float height = heightMap[i][j]/maxY;

						//determine the colors of the map
						if (height < 0.2){
							//dark green
							vertexColors[i][j][0] = 0;
							vertexColors[i][j][1] = 0.392157;
							vertexColors[i][j][2] = 0;
						}else if (height < 0.4){
							//light green
							vertexColors[i][j][0] = 0.498039;
							vertexColors[i][j][1] = 1;
							vertexColors[i][j][2] = 0;
						}else if (height < 0.5){
							//yellow
							vertexColors[i][j][0] = 1;
							vertexColors[i][j][1] = 1;
							vertexColors[i][j][2] = 0;
						}else if (height < 0.6){
							//orange
							vertexColors[i][j][0] = 1;
							vertexColors[i][j][1] = 0.647059;
							vertexColors[i][j][2] = 0;
						}else if (height < 0.8){
							//red
							vertexColors[i][j][0] = 1;
							vertexColors[i][j][1] = 0;
							vertexColors[i][j][2] = 0;
						}else if (height <= 1){
							//grey
							vertexColors[i][j][0] = 0.5;
							vertexColors[i][j][1] = 0.5;
							vertexColors[i][j][2] = 0.5;
						}
					}
				}

				//for each vertex in the map
				for (int i = 0; i < height -1 ; i++){
					for (int j = 0; j < width -1; j++){

						//triangle 1
						glColor3f(vertexColors[i][j][0],vertexColors[i][j][1],vertexColors[i][j][2]);
						glVertex3f(i,heightMap[i][j],j);

						glColor3f(vertexColors[i+1][j][0],vertexColors[i+1][j][1],vertexColors[i+1][j][2]);
						glVertex3f(i+1,heightMap[i+1][j],j);

						glColor3f(vertexColors[i+1][j+1][0],vertexColors[i+1][j+1][1],vertexColors[i+1][j+1][2]);
						glVertex3f(i+1,heightMap[i+1][j+1],j+1);
						
						//triangle 2
						glColor3f(vertexColors[i][j][0],vertexColors[i][j][1],vertexColors[i][j][2]);
						glVertex3f(i,heightMap[i][j],j);
	
						glColor3f(vertexColors[i+1][j+1][0],vertexColors[i+1][j+1][1],vertexColors[i+1][j+1][2]);
						glVertex3f(i+1,heightMap[i+1][j+1],j+1);	

						glColor3f(vertexColors[i][j+1][0],vertexColors[i][j+1][1],vertexColors[i][j+1][2]);
						glVertex3f(i,heightMap[i][j+1],j + 1);	
			
					}
				}


			}


		glEnd();

		}else if (showLighitng == true){
			//if there is lighting
			if (flatShading == false){
				//if oyu want smooth shading

				//store all the normals for flat shading
				vector< vector < vector<float> > > vertexNormals;
				//initialize vertices
				vertexNormals.resize(height);
				for (int k = 0; k< height; k++){
					vertexNormals[k].resize(height);
				}
				for (int i = 0; i < height; i++){
					for ( int j = 0; j < height; j++){
						vertexNormals[i][j].resize(3);	//normal values
					}
				}

				//calculate all the normals
				for (int i = 0; i < height -1 ; i++){
					for (int j = 0; j < width -1; j++){

						vector<float> normal;

						normal = calcNorm(i,heightMap[i][j],j,i+1,heightMap[i+1][j],j,i+1,heightMap[i+1][j+1],j+1);

						//the normal of the vertex
						//keep adding all the normal value to the vertex, we will normalize all the normals later
						vertexNormals[i][j][0] += normal[0];	//vertexnormal.x += faceNormal.x
						vertexNormals[i][j][1] += normal[1];	//vertexnormal.y += faceNormal.y
						vertexNormals[i][j][2] += normal[2];	//vertexnormal.z += faceNormal.z
					}
				}

				//normalize all vertex normals
				for (int i = 0; i < height -1 ; i++){
					for (int j = 0; j < width -1; j++){

						//float magnitude = sqrt(noramlx*noramlx + noramly*noramly + noramlz*noramlz );
						float magnitude = sqrt(vertexNormals[i][j][0]*vertexNormals[i][j][0] + vertexNormals[i][j][1]*vertexNormals[i][j][1] + vertexNormals[i][j][2]*vertexNormals[i][j][2] );

						//normalize the normal
						vertexNormals[i][j][0] = vertexNormals[i][j][0]/magnitude;	// noramlx = noramlx/magnitude;
						vertexNormals[i][j][1] = vertexNormals[i][j][1]/magnitude;	// noramly = noramly/magnitude;
						vertexNormals[i][j][2] = vertexNormals[i][j][2]/magnitude;	// noramlz = noramlz/magnitude;
					}
				}


				glBegin(GL_TRIANGLES);

				for (int i = 0; i < height -1 ; i++){
					for (int j = 0; j < width -1; j++){
						//triangle 1
						glNormal3f(vertexNormals[i][j][0],vertexNormals[i][j][1],vertexNormals[i][j][2]);
						glVertex3f(i,heightMap[i][j],j);

						glNormal3f(vertexNormals[i+1][j][0],vertexNormals[i+1][j][1],vertexNormals[i+1][j][2]);
						glVertex3f(i+1,heightMap[i+1][j],j);

						glNormal3f(vertexNormals[i+1][j+1][0],vertexNormals[i+1][j+1][1],vertexNormals[i+1][j+1][2]);
						glVertex3f(i+1,heightMap[i+1][j+1],j+1);

						//triangle 2
						glNormal3f(vertexNormals[i+1][j+1][0],vertexNormals[i+1][j+1][1],vertexNormals[i+1][j+1][2]);
						glVertex3f(i+1,heightMap[i+1][j+1],j+1);

						glNormal3f(vertexNormals[i][j+1][0],vertexNormals[i][j+1][1],vertexNormals[i][j+1][2]);
						glVertex3f(i,heightMap[i][j+1],j + 1);

						glNormal3f(vertexNormals[i][j][0],vertexNormals[i][j][1],vertexNormals[i][j][2]);
						glVertex3f(i,heightMap[i][j],j);	
					}
				}
				glEnd();

			}else if (flatShading == true){
				// if you want flat shading
				glBegin(GL_TRIANGLES);

				for (int i = 0; i < height -1 ; i++){
					for (int j = 0; j < width -1; j++){

						//face normal
						vector<float> normal;
						normal = calcNorm(i,heightMap[i][j],j,i+1,heightMap[i+1][j],j,i+1,heightMap[i+1][j+1],j+1);

						//x, y, z						
						glNormal3f(normal[0],normal[1],normal[2]);
						
						glVertex3f(i+1,heightMap[i+1][j],j);
						glVertex3f(i+1,heightMap[i+1][j+1],j+1);
						glVertex3f(i,heightMap[i][j],j);

						//calc the face normal
						normal = calcNorm(i,heightMap[i][j+1],j+1, i,heightMap[i][j],j ,i+1,heightMap[i+1][j+1],j+1);

						///x, y, z
						glNormal3f(normal[0],normal[1],normal[2]);

						glVertex3f(i,heightMap[i][j+1],j + 1);
						glVertex3f(i,heightMap[i][j],j);
						glVertex3f(i+1,heightMap[i+1][j+1],j+1);
							
					}
				}

				glEnd();
			}
		}	
	}		//if solid drawing
}

//method for drawing all quads
void drawMapQuads(){

	//if you want to show wireframe
	if (showWireFrame == true){

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glColor3f(0,1,0);	//grean

		glBegin(GL_QUADS);
			for (int i = 0; i < height -1 ; i++){
				for (int j = 0; j < width -1; j++){
					//quad
					glVertex3f(i,heightMap[i][j],j);
					glVertex3f(i+1,heightMap[i+1][j],j);
					glVertex3f(i+1,heightMap[i+1][j+1],j+1);
					glVertex3f(i,heightMap[i][j+1],j + 1);
				}
			}

		glEnd();
	} 

	//show solid polygons
	if (showSolidPoly == true){

		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		
		//if there is no lighting
		if (showLighitng == false){

			glBegin(GL_QUADS);	

			if (showHypsometric == false){
				//show greyscale

				//store all the colors of each vertex
				vector< vector <float> > vertexColors;
				vertexColors.resize(height);

				for (int i = 0; i< height; i++){
					vertexColors[i].resize(height);
				}

				for (int i = 0; i < height; i++){
					for (int j = 0; j < height; j++){
						vertexColors[i][j] = heightMap[i][j]/maxY;	//determines color of vertex
					}
				}

				//for each vertex
				for (int i = 0; i < height -1 ; i++){
					for (int j = 0; j < width -1; j++){

						float colour1 = vertexColors[i][j];
						float colour2 = vertexColors[i+1][j];
						float colour3 = vertexColors[i+1][j+1];
						float colour4 = vertexColors[i][j+1];

						//Quad
						glColor3f(colour1,colour1,colour1);
						glVertex3f(i,heightMap[i][j],j);

						glColor3f(colour2,colour2,colour2);
						glVertex3f(i+1,heightMap[i+1][j],j);

						glColor3f(colour3,colour3,colour3);
						glVertex3f(i+1,heightMap[i+1][j+1],j+1);

						glColor3f(colour4,colour4,colour4);
						glVertex3f(i,heightMap[i][j+1],j + 1);
			
					}
				}
			
			}else if (showHypsometric == true){
				//if you want to show hypsometric map

				//store the colors of each vertex
				vector< vector <vector< float> > > vertexColors;
				vertexColors.resize(height);

				for (int i = 0; i< height; i++){
					vertexColors[i].resize(height);
				}

				for (int i = 0; i < height; i++){
					for (int j = 0; j < height; j++){
						vertexColors[i][j].resize(3);	//color values
					}
				}

				//for each vertex
				for (int i = 0; i < height; i++){
					for (int j = 0; j < height; j++){
						
						float height = heightMap[i][j]/maxY;

						//determin the color of each vertex
						if (height < 0.2){
							//dark green
							vertexColors[i][j][0] = 0;
							vertexColors[i][j][1] = 0.392157;
							vertexColors[i][j][2] = 0;
						}else if (height < 0.4){
							//light green
							vertexColors[i][j][0] = 0.498039;
							vertexColors[i][j][1] = 1;
							vertexColors[i][j][2] = 0;
						}else if (height < 0.5){
							//yellow
							vertexColors[i][j][0] = 1;
							vertexColors[i][j][1] = 1;
							vertexColors[i][j][2] = 0;
						}else if (height < 0.6){
							//orange
							vertexColors[i][j][0] = 1;
							vertexColors[i][j][1] = 0.647059;
							vertexColors[i][j][2] = 0;
						}else if (height < 0.8){
							//red
							vertexColors[i][j][0] = 1;
							vertexColors[i][j][1] = 0;
							vertexColors[i][j][2] = 0;
						}else if (height <= 1){
							//grey
							vertexColors[i][j][0] = 0.5;
							vertexColors[i][j][1] = 0.5;
							vertexColors[i][j][2] = 0.5;
						}
					}
				}

				//for each vertex
				for (int i = 0; i < height -1 ; i++){
					for (int j = 0; j < width -1; j++){

						//quad
						glColor3f(vertexColors[i][j][0],vertexColors[i][j][1],vertexColors[i][j][2]);
						glVertex3f(i,heightMap[i][j],j);

						glColor3f(vertexColors[i+1][j][0],vertexColors[i+1][j][1],vertexColors[i+1][j][2]);
						glVertex3f(i+1,heightMap[i+1][j],j);

						glColor3f(vertexColors[i+1][j+1][0],vertexColors[i+1][j+1][1],vertexColors[i+1][j+1][2]);
						glVertex3f(i+1,heightMap[i+1][j+1],j+1);

						glColor3f(vertexColors[i][j+1][0],vertexColors[i][j+1][1],vertexColors[i][j+1][2]);
						glVertex3f(i,heightMap[i][j+1],j+1);
					}
				}
			}

			glEnd();

		}else if (showLighitng == true){
			//if there is lighting

			//for smooth shading
			if (flatShading == false){

				//the vertex for each vertex
				vector< vector < vector<float> > > vertexNormals;
				vertexNormals.resize(height);
				for (int k = 0; k< height; k++){
					vertexNormals[k].resize(height);
				}

				for (int i = 0; i < height; i++){
					for ( int j = 0; j < height; j++){
						vertexNormals[i][j].resize(3);
					}
				}

				//calculate all the face normals
				for (int i = 0; i < height -1 ; i++){
					for (int j = 0; j < width -1; j++){

						vector<float> normal;

						normal = calcNorm(i,heightMap[i][j],j,i+1,heightMap[i+1][j],j,i+1,heightMap[i+1][j+1],j+1);

						//the normal of the vertex, same technique as triangles
						vertexNormals[i][j][0] += normal[0];	//vertexnormal.x += faceNormal.x
						vertexNormals[i][j][1] += normal[1];	//vertexnormal.y += faceNormal.y
						vertexNormals[i][j][2] += normal[2];	//vertexnormal.z += faceNormal.z
					}
				}

				//normalize all vertex normals
				for (int i = 0; i < height -1 ; i++){
					for (int j = 0; j < width -1; j++){

						//the magnitude
						float magnitude = sqrt(vertexNormals[i][j][0]*vertexNormals[i][j][0] + vertexNormals[i][j][1]*vertexNormals[i][j][1] + vertexNormals[i][j][2]*vertexNormals[i][j][2] );

						//normalize
						vertexNormals[i][j][0] = vertexNormals[i][j][0]/magnitude;	// noramlx = noramlx/magnitude;
						vertexNormals[i][j][1] = vertexNormals[i][j][1]/magnitude;	// noramly = noramly/magnitude;
						vertexNormals[i][j][2] = vertexNormals[i][j][2]/magnitude;	// noramlz = noramlz/magnitude;
					}
				}


				glBegin(GL_QUADS);

				
				for (int i = 0; i < height -1 ; i++){
					for (int j = 0; j < width -1; j++){

						//quad
						glNormal3f(vertexNormals[i][j][0],vertexNormals[i][j][1],vertexNormals[i][j][2]);
						glVertex3f(i,heightMap[i][j],j);

						glNormal3f(vertexNormals[i+1][j][0],vertexNormals[i+1][j][1],vertexNormals[i+1][j][2]);
						glVertex3f(i+1,heightMap[i+1][j],j);

						glNormal3f(vertexNormals[i+1][j+1][0],vertexNormals[i+1][j+1][1],vertexNormals[i+1][j+1][2]);
						glVertex3f(i+1,heightMap[i+1][j+1],j+1);

						glNormal3f(vertexNormals[i][j+1][0],vertexNormals[i][j+1][1],vertexNormals[i][j+1][2]);
						glVertex3f(i,heightMap[i][j+1],j + 1);
	
					}
				}
				glEnd();

			}else if (flatShading == true){
				//for flat shading

				glBegin(GL_QUADS);

				//for each vertex
				for (int i = 0; i < height -1 ; i++){
					for (int j = 0; j < width -1; j++){

						//face normal
						vector<float> normal;
						normal = calcNorm(i,heightMap[i][j],j,i+1,heightMap[i+1][j],j,i+1,heightMap[i+1][j+1],j+1);

						//quad
						glNormal3f(normal[0],normal[1],normal[2]);
						glVertex3f(i,heightMap[i][j],j);

						glVertex3f(i+1,heightMap[i+1][j],j);

						glVertex3f(i+1,heightMap[i+1][j+1],j+1);

						glVertex3f(i,heightMap[i][j+1],j + 1);
					}
				}

				glEnd();
			}
		}	
	}		//if solid drawing
}

//for reseting the map
void resetMap(){

	//initialize values for heightMap
	for (int i = 0; i < height; i++){
		for (int j = 0; j < width; j++){
			heightMap[i][j] = 0.0;
		}
	}

	//decide wich algorithm for generating map
	if (useFaultAlg == false) {
		//run terrain point algorithm
		//from http://www.lighthouse3d.com/
		srand((int) time(0));	//randomizer seed
		int iterations = 4*height;

		for (int i = 0; i < iterations; i++){
			genCircle();
		}

	}else if (useFaultAlg == true){
		//use fault algorithm
		//from http://www.lighthouse3d.com/
		srand((int) time(0));	//randomizer seed
		int numIter = height*2;
		for (int t= 0; t < numIter; ++t){

			float v = (float)rand();
			float a = sin(v);
			float b = cos(v);
			float d = sqrt(height*height + height*height);
			// rand() / RAND_MAX gives a random number between 0 and 1.
			// therefore c will be a random number between -d/2 and d/2
			float c = ( (float) rand() / RAND_MAX) * d - d/2.0;
			float displacement = 1;

			for (int i = 0; i < height; i++ ){
				for (int j = 0; j < height; j++){
					if ( (a*i) + (b*j) - c > 0){
						//dist = a*i + b* - c;

						heightMap[i][j] += displacement;
					}else {
						heightMap[i][j] -= displacement;
					}

				}

			}
		}

	}

	calcMaxY();		//calculate the maximum of y of the map
}

//drawing the terrain
void drawTerrain(){

	//for rotating the map
	glPushMatrix();

		glRotatef(10*mapY, 0, 1, 0);
		glRotatef(10*mapX, 1, 0, 0);

			//quads or triangles
			if (drawQuads == false){
				drawMapTriag();
			}else{
				drawMapQuads();
			}

	glPopMatrix();
}

int test = 0;

//keyboard presses
void keyboard(unsigned char key, int x, int y)
{

	/* key presses move the cube, if it isn't at the extents (hard-coded here) */
	switch (key)
	{
		case 'q':
		case 27:
			exit (0);
			break;
			//quit
		case 'y':
		case 'Y':

			if (drawQuads == false){
				drawQuads = true;		//draw quads
			}
			break;
		case 'i':
		case 'I':
			//move camera
			camPos[1] += 0.5;
			break;

		case 'o':
			//move camera
			camPos[1] -= 0.5;
			break;
		case 'w':
		case 'W':
			//toggle wirefarem
			//flags for showing wireframe and solid polygons for the height map
			if (showWireFrame == true && showSolidPoly == false){
				showSolidPoly = true;
				//both are true
			}else if (showWireFrame == true && showSolidPoly == true){
				showWireFrame = false;
			}else if (showWireFrame == false && showSolidPoly == true){
				showSolidPoly = false;
				showWireFrame = true;
			}
			break;

		case 'R':
		case 'r':
			resetMap();
			break;
		case 't':
		case 'T':
			//draw triangles
			if (drawQuads == true){
				drawQuads = false;
			}

			break;
		case 'x':
		case 'X':
			if (flatShading == true){
				flatShading = false;
				glShadeModel(GL_SMOOTH);

			}else if (flatShading == false){
				flatShading = true;
				glShadeModel(GL_FLAT);
			}
			break;
		case 'h':	//rotate about y scene
			camPos[0]-=0.5;
			break;
		case 'k':	//roatate about y scene
			camPos[0]+=0.5;
			break;
		case 'u':	//rotate about the x axis
			camPos[2] -= 0.5;
			break;
		case 'j':	//rotate about the x axis
			camPos[2] += 0.5;
			break;
		case 'l':
			//toggle lighting
			if (showLighitng == true){
				glDisable(GL_LIGHTING);
				showLighitng = false;
			}else {
				glEnable(GL_LIGHTING);
				showLighitng = true;
			}
			break;
		case 'd':
			//move light 1
			light1pos[2] ++;
			glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
			break;
		case 'a':
			//move light 1
			light1pos[2] --;
			glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
			break;
		case 'e':
			//move light 1	
			light1pos[0] ++;
			glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
			break;
		case 's':
			//move light 1
			light1pos[0] --;
			glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
			break;

		case 'b':
			//move light 0
			light0pos[2] ++;
			glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
			break;
		case 'c':
			//move light 0
			light0pos[2] --;
			glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
			break;

		case 'f':
			//move light 0
			light0pos[0] ++;
			glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
			break;
		case 'v':
			//move light 0
			light0pos[0] --;
			glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
			break;
		case 'n':
			//for showing hypsometric map or not
			if (showHypsometric == false){
				showHypsometric = true;
			}else {
				showHypsometric = false;
			}
			break;
		case 'z':
			//which algorithm to use for making th map
			if (useFaultAlg == false){
				//printf("use the fault algorithm \n");
				useFaultAlg = true;
			}else {
				useFaultAlg = false;
			}
			break;
		case 'm':
			//change the size of the map
			//not to TA: i can generate maps of 300 but its kind of slow
			if (height == 50){
				height = 200;
				width = 200;
				//resetMap();
				createMap();

			}else if (height == 200){
				height = 50;
				width =50;
				//resetMap();
				createMap();
			}
			break;
	}
	glutPostRedisplay();
}

void special(int key, int x, int y)
{
	/* arrow key presses move the camera */
	switch(key)
	{
		case GLUT_KEY_LEFT:
			mapY++;
			camPos[0]-=0.5;
			break;

		case GLUT_KEY_RIGHT:
			mapY--;
			camPos[0]+=0.5;
			break;

		case GLUT_KEY_UP:
			mapX ++;
			camPos[2] -= 0.5;
			break;

		case GLUT_KEY_DOWN:
			mapX --;
			camPos[2] += 0.5;
			break;
		
		case GLUT_KEY_HOME:
			camPos[1] += 0.5;
			break;

		case GLUT_KEY_END:
			camPos[1] -= 0.5;
			break;

	}
	glutPostRedisplay();
}

//instructions for user
void instructions(){
	printf("001411625, \n");
	printf("q: quit program\n");
	printf("y: draw quads \n");
	printf("t: draw triangles \n");
	printf("w: toggle wire frame and solid polygons  \n");
	printf("r: generate a new map \n");
	printf("x: toggle between flat and smooth shading \n");
	printf("n: toggle hypsometric map \n");
	printf("z: toggle fault algorithm (have to reset map after ('r')) \n");
	printf("m: change the size of the map \n");
	printf("l: toggle lighting \n");
	printf("easd: move light 1\n");
	printf("fcvb: move light 0 \n");
	printf("Arrow Keys: Move around axis' \n");
	printf("u,j,k,l and i,o: move camera \n");
}

//init function
void init(void)
{
	glClearColor(0, 0, 0, 0);
	
	//glColor3f(1, 1, 1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-2, 2, -2, 2, -2, 2);

	gluPerspective(45, 1, 1, 1000);


	glEnable(GL_DEPTH_TEST); //2nd: enable the depth test and enable lights

	
	//turn on light bulb 0
	glEnable(GL_LIGHT0);

	//upload light data to gpu
	glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0dif);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0amb);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0spc);

	//turn on light bulb 1
	glEnable(GL_LIGHT1);

	//upload light data to gpu
	glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1dif);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1amb);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1spc);
	

	//define and use a material
	float m_amb[] = {0.25, 0.20725, 0.20725	, 0.922};
	float m_dif[] = {1.0, 0.829, 0.829, 0.922};
	float m_spec[] = {0.296648,0.296648, 0.296648, 0.922};
	float shiny = 11.264; //10, 100

	//upload material data to gpu
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_dif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);

	//define smooth(guaroud) or flat shading
	glShadeModel(GL_FLAT);
	//glShadeModel(GL_SMOOTH);
	
	instructions();
	createMap();
}


/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */
void display(void)
{
	float origin[3] = {0,0,0};
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(camPos[0], camPos[1], camPos[2], 0,0,0, 0,1,0);
	glColor3f(1,1,1);

	drawTerrain();
	
	glutSwapBuffers();
}

/* main function - program entry point */
int main(int argc, char** argv)
{
	glutInit(&argc, argv);		//starts up GLUT
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	
	
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);

	glutCreateWindow("3GC3 Assignment 2");	//creates the window

	glutDisplayFunc(display);	//registers "display" as the display callback function
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	glEnable(GL_DEPTH_TEST);
	init();


	glFrontFace(GL_CCW);
	glCullFace(GL_FRONT);		//culling
	glEnable(GL_CULL_FACE);

	glutMainLoop();				//starts the event loop

	return(0);					//return may not be necessary on all compilers
}