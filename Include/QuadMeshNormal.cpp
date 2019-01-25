#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>


#include "Vector3D.h"

// Data structure for a vertex
typedef struct MeshVertex
{
	Vector3D position;
	// Only if you use per vertex normals
	Vector3D normal;
} MeshVertex;

// Data structure for a quad (4-sided polygon)
typedef struct MeshQuad
{
	// index into vertex array for each quad
	int vertices[4];
} MeshQuad;

Vector3D makeQuadNormal(MeshQuad quad, MeshVertex *vertexArray)
{
	Vector3D vp, p0, p1;
	int i;
	Vector3D normal;
	normal.x = 0.0; normal.y = 0.0; normal.z = 0.0;

	vp = vertexArray[quad.vertices[0]].position;
	for (i = 0; i < 4; i++) {
		vp = vertexArray[quad.vertices[i]].position;
		p0 = vertexArray[quad.vertices[(i + 1) % 4]].position;
		p1 = vertexArray[quad.vertices[(i + 2) % 4]].position;

		normal.x += (p1.y - p0.y) * (p1.z + p0.z);
		normal.y += (p1.z - p0.z) * (p1.x + p0.x);
		normal.z += (p1.x - p0.x) * (p1.y + p0.y);
	};
	Normalize(&normal);
	return normal;
}