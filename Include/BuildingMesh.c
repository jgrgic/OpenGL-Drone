#include "BuildingMesh.h"
#include "Vector3D.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <gl/glut.h>
#include <gl/gl.h>

float baseQuad[][3] = { {-1.0, 0.0, 1.0}, 
						{ 1.0, 0.0, 1.0}, 
						{ 1.0, 0.0,-1.0}, 
						{-1.0, 0.0,-1.0} };

void makeQuadNormal(float *qn, float *v1, float *v2, float *v3);

//TODO: pass in a third vector, one that specifies the starting positions of the 4 vertices that act as a base for extrusion; then replace all instances of baseQuad with it
//baseQuad is just for testing purposes
BuildingMesh *createBuildingMesh(Vector3D scale, Vector3D translate, float floorHeight, float totalNumberOfCopies, int func) {

	//allocate space for entire mesh, its vertex array, and quad array
	BuildingMesh *bm = (BuildingMesh *)calloc(1, sizeof(BuildingMesh));

	bm->vertices = (MeshVertexBuilding *)calloc(1000, sizeof(MeshVertexBuilding));
	bm->quads = (MeshQuadBuilding *)calloc(1000, sizeof(MeshQuadBuilding));
	bm->quadNormals = (MeshNormalBuilding *)calloc(1000, sizeof(MeshNormalBuilding));

	//scale factors and translation factors of moved mesh
	bm->scale.x = scale.x;
	bm->scale.y = scale.y;
	bm->scale.z = scale.z;
	bm->translate.x = translate.x;
	bm->translate.y = translate.y;
	bm->translate.z = translate.z;

	//sets the starting point as the near, bottom right vertex
	bm->startingPoint = NewVector3D(translate.x, translate.y, translate.z);

	bm->floorHeight = floorHeight;
	bm->totalNumberOfCopies = totalNumberOfCopies;

	bm->mat_ambient[0] = 0.0;
	bm->mat_ambient[1] = 0.05f;
	bm->mat_ambient[2] = 0.0;
	bm->mat_ambient[3] = 1.0;
	bm->mat_specular[0] = 0.0;
	bm->mat_specular[1] = 0.0;
	bm->mat_specular[2] = 0.004f;
	bm->mat_specular[3] = 1.0;
	bm->mat_diffuse[0] = 0.5;
	bm->mat_diffuse[1] = 0.5;
	bm->mat_diffuse[2] = 0.5;
	bm->mat_diffuse[3] = 1.0;
	bm->mat_shininess[0] = 0;

	bm->highlightMat_ambient[0] = 0.0;
	bm->highlightMat_ambient[1] = 0.00;
	bm->highlightMat_ambient[2] = 0.0;
	bm->highlightMat_ambient[3] = 1.0;
	bm->highlightMat_specular[0] = 0.0;
	bm->highlightMat_specular[1] = 0.0;
	bm->highlightMat_specular[2] = 0.0;
	bm->highlightMat_specular[3] = 1.0;
	bm->highlightMat_diffuse[0] = 1.0;
	bm->highlightMat_diffuse[1] = 0.0;
	bm->highlightMat_diffuse[2] = 0.0;
	bm->highlightMat_diffuse[3] = 1.0;
	bm->highlightMat_shininess[0] = 0.0;

	//set up base quad; the starting vertices
	for (int i = 0; i < 4; i++) {
		bm->vertices[i].position[0] = baseQuad[i][0];
		bm->vertices[i].position[1] = baseQuad[i][1] - 1;
		bm->vertices[i].position[2] = baseQuad[i][2];
	}
	
	int initialQuad = 0;
	int initialVertex = 4;
	int previousVertex = 0;


	for (int i = 1; i <= totalNumberOfCopies; i++) {

		float height = i * floorHeight;
		
		//random numbers generated to be applied to sin function
		float rand1 = (float)rand() / (float)(RAND_MAX / 10);
		float rand2 = (float)rand() / (float)(RAND_MAX / 10);

		//increasing by height each time, x and z values stay the same
		if (func == 1) {
			for (int j = 0; j < 4; j++) {
				bm->vertices[initialVertex + j].position[0] = 0.5 * baseQuad[j][0];
				bm->vertices[initialVertex + j].position[2] = 0.5 * baseQuad[j][2];
				bm->vertices[initialVertex + j].position[1] = height;
			}
		}
		if (func == 2) {
			for (int j = 0; j < 4; j++) {
				bm->vertices[initialVertex + j].position[0] = sin(rand1) * baseQuad[j][0];
				bm->vertices[initialVertex + j].position[2] = sin(rand2) * baseQuad[j][2];
				bm->vertices[initialVertex + j].position[1] = height;
			}
		}
		if (func == 3) {
			for (int j = 0; j < 4; j++) {
				bm->vertices[initialVertex + j].position[0] = baseQuad[j][0];
				bm->vertices[initialVertex + j].position[2] = baseQuad[j][2];
				bm->vertices[initialVertex + j].position[1] = height;
			}
		}

		//creating the side quads between vertices
		for (int j = 0; j < 4; j++) {

			//fills in last side
			if (j == 3) {
				bm->quads[initialQuad + j].vertices[0] = previousVertex;
				bm->quads[initialQuad + j].vertices[1] = previousVertex - 3;
				bm->quads[initialQuad + j].vertices[2] = initialVertex - 3;
				bm->quads[initialQuad + j].vertices[3] = initialVertex;
			}
			else {
				//fills in 3 sides
				bm->quads[initialQuad + j].vertices[0] = previousVertex;
				bm->quads[initialQuad + j].vertices[1] = previousVertex + 1;
				bm->quads[initialQuad + j].vertices[2] = initialVertex + 1;
				bm->quads[initialQuad + j].vertices[3] = initialVertex;
			}
			//compute normals here
			makeQuadNormal(&bm->quadNormals[initialQuad + j].v[0],
							&bm->vertices[bm->quads[initialQuad + j].vertices[0]].position[0],
							&bm->vertices[bm->quads[initialQuad + j].vertices[1]].position[0],
							&bm->vertices[bm->quads[initialQuad + j].vertices[2]].position[0]);

			previousVertex++;
			initialVertex++;
		}
		previousVertex = i * 4;
		initialVertex = (i + 1) * 4;
		initialQuad = i * 4;
	}

	bm->numVertices = initialVertex;

	//bottom cap
	bm->quads[initialQuad].vertices[0] = 3;
	bm->quads[initialQuad].vertices[1] = 2;
	bm->quads[initialQuad].vertices[2] = 1;
	bm->quads[initialQuad].vertices[3] = 0;
	bm->quadNormals[initialQuad].v[0] = 0;
	bm->quadNormals[initialQuad].v[1] = -1;
	bm->quadNormals[initialQuad].v[2] = 0;
	initialQuad++;

	//top cap
	bm->quads[initialQuad].vertices[0] = initialVertex - 4;
	bm->quads[initialQuad].vertices[1] = initialVertex - 3;
	bm->quads[initialQuad].vertices[2] = initialVertex - 2;
	bm->quads[initialQuad].vertices[3] = initialVertex - 1;
	bm->quadNormals[initialQuad].v[0] = 0;
	bm->quadNormals[initialQuad].v[1] = 1;
	bm->quadNormals[initialQuad].v[2] = 0;

	bm->numQuads = ++initialQuad;

	return bm;
}

void drawBuildingMesh(BuildingMesh *bm) {

	bm->startingPoint = NewVector3D(bm->translate.x, bm->translate.y, bm->translate.z);
	
	if (bm->selected) {
		glMaterialfv(GL_FRONT, GL_AMBIENT, bm->highlightMat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, bm->highlightMat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, bm->highlightMat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, bm->highlightMat_shininess);
	}
	else {
		glMaterialfv(GL_FRONT, GL_AMBIENT, bm->mat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, bm->mat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, bm->mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, bm->mat_shininess);
	}

	glPushMatrix();
	glTranslatef(bm->translate.x, bm->translate.y, bm->translate.z);
	glScalef(bm->scale.x, bm->scale.y, bm->scale.z);

	glBegin(GL_QUADS);
	//draws the 4 vertices that correspond to some quad
	for (int i = 0; i < bm->numQuads; i++) {

		glNormal3f(bm->quadNormals[i].v[0], bm->quadNormals[i].v[1], bm->quadNormals[i].v[2]);
		glVertex3f(bm->vertices[bm->quads[i].vertices[0]].position[0],
					bm->vertices[bm->quads[i].vertices[0]].position[1],
					 bm->vertices[bm->quads[i].vertices[0]].position[2]);

		glVertex3f(bm->vertices[bm->quads[i].vertices[1]].position[0],
					bm->vertices[bm->quads[i].vertices[1]].position[1],
					 bm->vertices[bm->quads[i].vertices[1]].position[2]);

		glVertex3f(bm->vertices[bm->quads[i].vertices[2]].position[0],
					bm->vertices[bm->quads[i].vertices[2]].position[1],
					 bm->vertices[bm->quads[i].vertices[2]].position[2]);

		glVertex3f(bm->vertices[bm->quads[i].vertices[3]].position[0],
					bm->vertices[bm->quads[i].vertices[3]].position[1],
					 bm->vertices[bm->quads[i].vertices[3]].position[2]);
	}
	glEnd();
	glPopMatrix();
}

void makeQuadNormal(float *qn, float *v1, float *v2, float *v3) {

	float vec1[3];
	float vec2[3];

	vec1[0] = v1[0] - v2[0];
	vec1[1] = v1[1] - v2[1];
	vec1[2] = v1[2] - v2[2];

	vec2[0] = v3[0] - v2[0];
	vec2[1] = v3[1] - v2[1];
	vec2[2] = v3[2] - v2[2];

	qn[0] = vec2[1]*vec1[2] - vec2[2]*vec1[1];
	qn[1] = vec2[2]*vec1[0] - vec2[0]*vec1[2];
	qn[2] = vec2[0]*vec1[1] - vec2[1]*vec1[0];

	float normal = (float)sqrt(qn[0]*qn[0]+qn[1]*qn[1]+qn[2]*qn[2]);
	if (normal > 0) {
		qn[0] /= normal;
		qn[1] /= normal;
		qn[2] /= normal;
	}
}

void saveCity(BuildingMesh buildings[], int numOfItems, const char *fileName) {
	FILE *scene = fopen("city.txt", "a");
	fwrite(buildings, sizeof(BuildingMesh), numOfItems, scene);
	fclose(scene);
}

int loadCity(BuildingMesh buildings[], const char *fileName, int numOfItems) {
	FILE *scene = fopen("city.txt", "rb");
	fread(buildings, sizeof(BuildingMesh), numOfItems, scene);
	fclose(scene);
}

