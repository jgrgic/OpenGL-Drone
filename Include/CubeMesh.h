#include <stdbool.h>
#include "Vector3D.h"

typedef struct CubeMesh
{
	Vector3D center;
	Vector3D dim;
	float tx, ty, tz;		// Translatation Deltas
	float sfx, sfy, sfz;	// Scale Factors
	float angle;			// Angle around y-axis of cube coordinate system
	float aroundOrigin;

	float sx, sy, sz;
	float propeller;
	
	int textureID;

	bool selected;

	// Material properties for drawing
	float mat_ambient[4];
    float mat_specular[4];
    float mat_diffuse[4];
	float mat_shininess[1];

	// Material properties if selected
	float highlightMat_ambient[4];
    float highlightMat_specular[4];
    float highlightMat_diffuse[4];
	float highlightMat_shininess[1];

} CubeMesh;

CubeMesh newCube(Vector3D translate, Vector3D scale, float streetAngle);
CubeMesh newDronePart2(Vector3D entireTranslate, Vector3D scale, Vector3D setup, float droneRotation, float propellerSpin, float droneRotationOrigin);
CubeMesh newDronePart(Vector3D entireTranslate, Vector3D scale, Vector3D setup, float droneRotation, float propellerSpin);
void setTextureMapID(CubeMesh *cube, int textureID);
