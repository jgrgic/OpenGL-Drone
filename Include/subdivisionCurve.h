
#include <gl/glut.h>
#include <gl/gl.h>
#define M_PI 3.141592654


// Define a simple vector structure
typedef struct vector
{
	double x;
	double y;
} vector;

typedef  struct Point2D
{
	double x, y;
	double nx, ny;
	double u;
} Point2D;

typedef struct Vec2D
{
	double vx, vy;
} Vec2D;

typedef struct Line2D
{
	Point2D pt[2];
} Line2D;

typedef struct SubdivisionCurve
{
	int numControlPoints;
	Point2D controlPoints[20];
	Point2D originalPoints[20];

	double  ctlPtMarkerRadius;
	bool    drawCtlPoints;
	bool    drawCurve;

	int subdivisionSteps;

	int numCurvePoints;
	Point2D *curvePoints;

} SubivisionCurve;


void computeSubdivisionCurve(SubdivisionCurve *subcurve, bool curveClosed);
void setControlPointPosition(SubdivisionCurve *subcurve, int cpIndex, double x, double y);
int findClosestControlPoint(SubdivisionCurve *curve, double wx, double wy);
void positionAlongCurve(SubdivisionCurve *subcurve, double u, GLdouble *x, GLdouble *y);
void computeU(SubdivisionCurve *subcurve);

void setCtlPtMarkerRadius(SubdivisionCurve *subcurve, float radius);
void setDrawCtlPoints(SubdivisionCurve *subcurve, bool drawCtlPoints);
void setDrawCurve(SubdivisionCurve *subcurve, bool drawCurve);
void drawClosedCurve(SubdivisionCurve *subcurve);
void drawOpenCurve(SubdivisionCurve *opensubcurve);
void drawMarker(double radius, double x, double y);
