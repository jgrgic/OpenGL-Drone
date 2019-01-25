#include <windows.h>
#include <gl/glut.h>
#include <string.h>
#include <math.h>
#include "subdivisionCurve.h"

Point2D getPoint(Point2D *currentPoints, int numCurrentPoints, int j, bool curveClosed);
Point2D subdivide(Point2D v1, Point2D v2, Point2D v3, Point2D v4);

//sets the radius of the control point that is to be drawn
void setCtlPtMarkerRadius(SubdivisionCurve *subcurve, float radius)
{
	subcurve->ctlPtMarkerRadius = radius;
}

//sets 
void setDrawCtlPoints(SubdivisionCurve *subcurve, bool drawCtlPoints)
{
	subcurve->drawCtlPoints = drawCtlPoints;
}


void setDrawCurve(SubdivisionCurve *subcurve, bool drawCurve)
{
	subcurve->drawCurve = drawCurve;
}

//draws a closed curve, used for the base
void drawClosedCurve(SubdivisionCurve *subcurve)
{
	glLineWidth(1.0);
	glColor3f(1.0, 0.0, 0.0);

	if (subcurve->drawCurve)
	{
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < subcurve->numCurvePoints; i++)
			glVertex2f(subcurve->curvePoints[i].x, subcurve->curvePoints[i].y);
		glEnd();
	}

	if (subcurve->drawCtlPoints)
	{
		glColor3f(0.0, 1.0, 0.0);
		glLineWidth(2.0);
		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < subcurve->numControlPoints; i++)
			glVertex2f(subcurve->controlPoints[i].x, subcurve->controlPoints[i].y);
		glEnd();
		for (int i = 0; i < subcurve->numControlPoints; i++)
			drawMarker(subcurve->ctlPtMarkerRadius, subcurve->controlPoints[i].x, subcurve->controlPoints[i].y);
	}
}

//draws an open ended curve; used for side profile view
void drawOpenCurve(SubdivisionCurve *opensubcurve)
{
	glLineWidth(3.0);
	glColor3f(0.0, 0.0, 0.0);

	glBegin(GL_LINES);
	glVertex2f(0.0, 50.0);
	glVertex2f(0.0, 200.0);
	glVertex2f(0.0, 50.0);
	glVertex2f(200.0, 50.0);
	glEnd();

	glLineWidth(1.0);
	glColor3f(1.0, 0.0, 0.0);

	if (opensubcurve->drawCurve)
	{
		glBegin(GL_LINES);
		for (int i = 0; i < opensubcurve->numCurvePoints - 1; i++)
		{
			glVertex2f(opensubcurve->curvePoints[i].x, opensubcurve->curvePoints[i].y);
			glVertex2f(opensubcurve->curvePoints[i + 1].x, opensubcurve->curvePoints[i + 1].y);
		}
		glEnd();
	}

	if (opensubcurve->drawCtlPoints)
	{
		glColor3f(0.0, 1.0, 0.0);
		glLineWidth(2.0);
		glBegin(GL_LINES);
		for (int i = 0; i < opensubcurve->numControlPoints - 1; i++)
		{

			glVertex2f(opensubcurve->controlPoints[i].x, opensubcurve->controlPoints[i].y);
			glVertex2f(opensubcurve->controlPoints[i + 1].x, opensubcurve->controlPoints[i + 1].y);

		}
		glEnd();
		for (int i = 0; i < opensubcurve->numControlPoints; i++)
			drawMarker(opensubcurve->ctlPtMarkerRadius, opensubcurve->controlPoints[i].x, opensubcurve->controlPoints[i].y);
	}
}

//sets the position of control points for the curve when passed some x,y coordinate
void setControlPointPosition(SubdivisionCurve *subcurve, int cpIndex, double x, double y)
{
	subcurve->controlPoints[cpIndex].x = x;
	subcurve->controlPoints[cpIndex].y = y;
}

int findClosestControlPoint(SubdivisionCurve *curve, double wx, double wy)
{

	int    closestPoint = -1;
	double closestDistance = 1000000.0;

	for (int i = 0; i < curve->numControlPoints; i++)
	{
		Point2D *current = &curve->controlPoints[i];
		double distance = sqrt((wx - current->x)*(wx - current->x) + (wy - current->y)*(wy - current->y));
		if (distance < closestDistance && distance <= curve->ctlPtMarkerRadius)
		{
			closestPoint = i;
			closestDistance = distance;
		}
	}
	return closestPoint;
}




#define MAXSTEPS 6
// Compute the subdivision curve
void computeSubdivisionCurve(SubdivisionCurve *subcurve, bool curveClosed)
{
	Point2D *curvePoints[MAXSTEPS];
	int numCurrentPoints = subcurve->numControlPoints;
	int n;
	Point2D *cur = subcurve->controlPoints;
	Point2D *sub;

	if (subcurve->subdivisionSteps > MAXSTEPS) return;

	subcurve->numCurvePoints = subcurve->numControlPoints;
	curvePoints[0] = subcurve->controlPoints;

	for (int i = 1; i <= subcurve->subdivisionSteps; i++)
	{
		if (!curveClosed)
			subcurve->numCurvePoints += subcurve->numCurvePoints - 1;
		else
			subcurve->numCurvePoints += subcurve->numCurvePoints;

		curvePoints[i] = (Point2D *)malloc(subcurve->numCurvePoints * sizeof(Point2D));
	}

	sub = curvePoints[1];
	for (int i = 0; i < subcurve->subdivisionSteps; i++)
	{
		n = numCurrentPoints;

		// if it's open then 1 fewer point
		if (!curveClosed)
			n -= 1;

		for (int j = 0; j < n; j++)
		{
			sub[2 * j] = cur[j];
			sub[2 * j + 1] = subdivide(getPoint(cur, numCurrentPoints, j - 1, curveClosed),
				getPoint(cur, numCurrentPoints, j, curveClosed),
				getPoint(cur, numCurrentPoints, j + 1, curveClosed),
				getPoint(cur, numCurrentPoints, j + 2, curveClosed));
		}
		if (!curveClosed)
			sub[2 * n] = cur[n];

		cur = sub;
		sub = curvePoints[i + 2];
		numCurrentPoints += n;
	}
	subcurve->curvePoints = curvePoints[subcurve->subdivisionSteps];
	Point2D *cp;
	for (int i = 0; i < subcurve->numCurvePoints; i++)
	{
		cp = &subcurve->curvePoints[i];
		int x = 9;
	}
}

Point2D getPoint(Point2D *currentPoints, int numCurrentPoints, int j, bool curveClosed)
{
	if (curveClosed)
	{
		int n = numCurrentPoints;
		if (j >= 0 && j < n)
			return currentPoints[j];
		else if (j < 0)
			return currentPoints[n - 1];
		else if (j == n)
			return currentPoints[0];
		else if (j == n + 1)
			return currentPoints[1];
	}
	else
	{
		int n = numCurrentPoints;
		if (j >= 0 && j < n) return currentPoints[j];

		Point2D p0, p1;
		// boundary cases
		if (j == -1)
		{
			p0 = currentPoints[0];
			p1 = currentPoints[1];
		}
		if (j == n)
		{
			p1 = currentPoints[n - 2];
			p0 = currentPoints[n - 1];
		}

		Point2D linearCombo;
		linearCombo.x = 2 * p0.x + -1 * p1.x;
		linearCombo.y = 2 * p0.y + -1 * p1.y;
		return linearCombo;
	}
}

Point2D subdivide(Point2D v1, Point2D v2, Point2D v3, Point2D v4)
{
	Point2D result;
	result.x = (9.0*(v2.x + v3.x) - v1.x - v4.x) / 16.0;
	result.y = (9.0*(v2.y + v3.y) - v1.y - v4.y) / 16.0;
	return result;
}

#define NUMPOINTS 20

void drawMarker(double radius, double x, double y)
{

	double deltaTheta = 2 * M_PI / NUMPOINTS;
	double theta = 0.0;

	glLineWidth(1.0);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);

	for (int i = 0; i < NUMPOINTS; i++)
	{
		theta += deltaTheta;
		double px = x + radius * cos(theta);
		double py = y + radius * sin(theta);
		glVertex2f(px, py);
	}
	glEnd();
}




// Compute position along curve
void positionAlongCurve(SubdivisionCurve *subcurve, double u, GLdouble *x, GLdouble *y)
{
	Point2D *cp, *pcp;

	subcurve->curvePoints[0].u = 0.0;
	double arcLength = 0.0;
	if (u >= 1.0)
	{
		*x = subcurve->curvePoints[subcurve->numCurvePoints - 1].x;
		*y = subcurve->curvePoints[subcurve->numCurvePoints - 1].y;
		return;
	}
	if (u <= 0.0)
	{
		*x = subcurve->curvePoints[0].x;
		*y = subcurve->curvePoints[0].y;
		return;
	}

	for (int i = 1; i < subcurve->numCurvePoints; i++)
	{
		cp = &subcurve->curvePoints[i];
		if (u < cp->u)
		{
			pcp = &subcurve->curvePoints[i - 1];
			break;
		}
	}
	double ue = u - pcp->u;
	*x = (1 - ue)*pcp->x + ue * cp->x;
	*y = (1 - ue)*pcp->y + ue * cp->y;
}

void computeU(SubdivisionCurve *subcurve)
{
	Point2D *cp, *pcp;

	subcurve->curvePoints[0].u = 0.0;
	double arcLength = 0.0;

	for (int i = 1; i < subcurve->numCurvePoints; i++)
	{
		cp = &subcurve->curvePoints[i];
		pcp = &subcurve->curvePoints[i - 1];
		double elementLength = sqrt((cp->x - pcp->x)*(cp->x - pcp->x) + (cp->y - pcp->y)*(cp->y - pcp->y));
		cp->u = elementLength;
		arcLength += elementLength;
	}
	double length = 0.0;
	for (int i = 0; i < subcurve->numCurvePoints; i++)
	{
		cp = &subcurve->curvePoints[i];
		length += cp->u;
		cp->u = length / arcLength;
	}
}