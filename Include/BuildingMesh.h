#include <stdbool.h>
#include "Vector3D.h"

typedef struct MeshVertexBuilding {
	float position[3];
} MeshVertexBuilding;

typedef struct MeshQuadBuilding {
	int vertices[4];
} MeshQuadBuilding;

typedef struct MeshNormalBuilding {
	float v[3];
} MeshNormalBuilding;

typedef struct BuildingMesh {

	MeshVertexBuilding *vertices;
	MeshQuadBuilding *quads;
	MeshNormalBuilding *quadNormals;

	int numVertices;
	int numQuads;

	//variables for the translations and scaling the building has gone through
	Vector3D scale;
	Vector3D translate;
	Vector3D startingPoint;

	float floorHeight; //distance between extruded copies
	float totalNumberOfCopies; //the upper bound; extrusion is finished when this point is reach; ie. the top of the building (its y value)

	bool selected;

	float mat_ambient[4];
	float mat_specular[4];
	float mat_diffuse[4];
	float mat_shininess[1];

	float highlightMat_ambient[4];
	float highlightMat_specular[4];
	float highlightMat_diffuse[4];
	float highlightMat_shininess[1];

} BuildingMesh;

BuildingMesh *createBuildingMesh(Vector3D scale, Vector3D translate, float floorHeight, float totalNumberOfCopies, int func);
void drawBuildingMesh(BuildingMesh *bm);
void saveCity(BuildingMesh buildings[], int numOfItems, const char *fileName);
int loadCity(BuildingMesh buildings[], const char *fileName);

