#include "stdafx.h"
//#include "..\SmartV_IISystem.h"
//#include "..\LidarDlg.h"
#include "afxdialogex.h"
#include <math.h>

#define PI 3.14159
static void
	draw_letter_N ()
{
	glBegin (GL_QUADS);
	
	glVertex3f (-1.0, -1.0, 0);
	glVertex3f (-0.5, -1.0, 0);
	glVertex3f (-0.5, 1.0, 0);
	glVertex3f (-1.0, 1.0, 0);

	glVertex3f (1.0, -1.0, 0);
	glVertex3f (0.5, -1.0, 0);
	glVertex3f (0.5, 1.0, 0);
	glVertex3f (1.0, 1.0, 0);

	glVertex3f (-0.5, 1.0, 0);
	glVertex3f (-1.0, 1.0, 0);
	glVertex3f (0.5, -1.0, 0);
	glVertex3f (1.0, -1.0, 0);

	glEnd ();
}
void CLidarDlg::Draw_compass(double x,double y,double z)
{
    int viewport[4];
    int pos[2] = { -50, -50 };
    int radius = 35;

    double carpos[3] = { 0, 0, 0 };
    double heading = 0, variance = 0;


    /* Find the world x,y coordinates for the compass, given we want it
     * near the side of the viewport. */
    glGetIntegerv (GL_VIEWPORT, viewport);

    /* Find the world x,y coordinates for the edge of the compass, so
     * we can compute how large to make it in world coordinates. */
    double x2=0, y2=0;

    glDisable (GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Draw the compass at the right place, size, and orientation */
    glPushMatrix ();
    glTranslatef (x, y, z);

//  double scale = sqrt ((x2-x)*(x2-x) + (y2-y)*(y2-y));

	double scale = 3.0;
    glScalef (scale, scale, scale);

    glRotatef (heading * 180 / M_PI + 90, 0, 0, 1);

    GLUquadricObj * q = gluNewQuadric ();

    /* Draw an error bar indicating one standard deviation of the
     * heading error.  If it's above the threshold, make it red. */

    glColor4f (0.8, 0.2, 0.2, 0.7);
    double std_deg = sqrt (variance) * 180 / M_PI;
    gluPartialDisk (q, 1.0, 1.1, 50, 1,
            90 - std_deg, 2 * std_deg);

    /* Draw the circle for the compass */
    glColor4f (1.0, 1.0, 1.0, 0.2);
    gluDisk (q, 0, 1.0, 20, 1);
    gluDeleteQuadric (q);

    /* Draw the arrow on the compass */
    glLineWidth (2.0);
    glColor4f (1.0, 1.0, 1.0, 1.0);
    glBegin (GL_LINES);
    glVertex3f (-0.5, 0, 0);
    glVertex3f ( 0.5, 0, 0);

    glVertex3f (0, 0.5, 0);
    glVertex3f (0, -0.5, 0);

    glVertex3f (0.25, 0.25, 0);
    glVertex3f (0.5, 0, 0);

    glVertex3f (0.25, -0.25, 0);
    glVertex3f (0.5, 0, 0);
    glEnd ();

    /* Draw tick marks around the compass (15 degree increments) */
    glLineWidth (1.0);
    int i;
    for (i = 30; i <= 330; i += 15) 
	{
        glPushMatrix ();
        glRotatef (i, 0, 0, 1);
        glBegin (GL_LINES);
        glVertex3f (0.95, 0, 0.0);
        glVertex3f (((i % 45) == 0) ? 0.75 : 0.85, 0, 0.0);
        glEnd ();
        glPopMatrix ();
    }

    /* Draw the letter N */
    glColor4f (0.2, 1.0, 0.6, 1.0);
    glTranslatef (0.75, 0, 0);
    glScalef (0.2, 0.2, 0.2);
    glRotatef (90, 0, 0, 1);
    draw_letter_N();

    glDisable (GL_BLEND);

    glPopMatrix ();
}
void CLidarDlg::Draw_grid (int meters_per_grid)
{
	double pos[3]={0,0,0};
	double grids_per_screen = 10;

	int num_lines = 300;

	double grid_ox = ceil (pos[0] / meters_per_grid) * meters_per_grid;
	double grid_oy = ceil (pos[1] / meters_per_grid) * meters_per_grid;
	double grid_oz = pos[2];

	glPushMatrix ();
	glTranslatef (grid_ox, grid_oy, grid_oz);

	glColor4f(0.1, 0.2, 0.1, 0.5);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);
	glBegin(GL_TRIANGLES);
	glVertex2f((-num_lines/2)*meters_per_grid, (-num_lines/2)*meters_per_grid);
	glVertex2f((+num_lines/2)*meters_per_grid, (-num_lines/2)*meters_per_grid);
	glVertex2f((+num_lines/2)*meters_per_grid, (+num_lines/2)*meters_per_grid);
	glVertex2f((-num_lines/2)*meters_per_grid, (-num_lines/2)*meters_per_grid);
	glVertex2f((+num_lines/2)*meters_per_grid, (+num_lines/2)*meters_per_grid);
	glVertex2f((-num_lines/2)*meters_per_grid, (+num_lines/2)*meters_per_grid);
	glEnd();

	glLineWidth (1);
	glEnable (GL_DEPTH_TEST);
	glBegin (GL_LINES);
	glColor3f (0.3, 0.3, 0.3);

	for (int i=0; i<num_lines; i++) 
	{
		glVertex2f ((-num_lines/2 + i) * meters_per_grid,
			- num_lines/2 * meters_per_grid);
		glVertex2f ((-num_lines/2 + i) * meters_per_grid,
			num_lines/2 * meters_per_grid);

		glVertex2f (- num_lines/2 * meters_per_grid,
			(-num_lines/2 + i) * meters_per_grid);
		glVertex2f (num_lines/2 * meters_per_grid,
			(-num_lines/2 + i) * meters_per_grid);
	}
	glEnd();
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_BLEND);

	glPopMatrix ();
}
void CLidarDlg::Draw_Road_Center_Point() //坐标系
{
	glLineWidth(1);
	glBegin(GL_LINES);
	glColor3f(1,1,1);
	glVertex3d(-10,0,0);
	glVertex3d(10,0,0);
	glVertex3d(0,-10,0);
	glVertex3d(0,10,0);
	glEnd();
}

void CLidarDlg::Draw_current_vehicle(float x, float y , float tht)
{
	tht=tht*PI/180.0;
	////画车点
	glPointSize(10.0);
	glBegin(GL_POINTS);
	glColor3f(0.4,0.5,0.0);
	glVertex3d(x,y,0.0);
	glEnd();

	//   //车头方向
	glLineWidth(3.0);
	glBegin(GL_LINES);
	glVertex3d(x,y,0.0);
	glVertex3d(x+10*cos(tht),y+10*sin(tht),0.0);	//cos参数为弧度
	glEnd();

	glLineWidth(3.0);
	glColor3f(0.1,1.0,0.0);
	glBegin(GL_POLYGON);
	glVertex2f(x-0.5*VehicleWidth*sin(tht),y+0.5*VehicleWidth*cos(tht));
	glVertex2f(x+0.5*VehicleWidth*sin(tht),y-0.5*VehicleWidth*cos(tht));
	glVertex2f(x+0.5*VehicleWidth*sin(tht)-VehicleLength*cos(tht),y-0.5*VehicleWidth*cos(tht)-VehicleLength*sin(tht));
	glVertex2f(x-0.5*VehicleWidth*sin(tht)-VehicleLength*cos(tht),y+0.5*VehicleWidth*cos(tht)-VehicleLength*sin(tht));
	glEnd();

	glColor3f(1.0,0.0,0.0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x-0.5*VehicleWidth*sin(tht),y+0.5*VehicleWidth*cos(tht));
	glVertex2f(x+0.5*VehicleWidth*sin(tht),y-0.5*VehicleWidth*cos(tht));
	glVertex2f(x+0.5*VehicleWidth*sin(tht)-VehicleLength*cos(tht),y-0.5*VehicleWidth*cos(tht)-VehicleLength*sin(tht));
	glVertex2f(x-0.5*VehicleWidth*sin(tht)-VehicleLength*cos(tht),y+0.5*VehicleWidth*cos(tht)-VehicleLength*sin(tht));
	glEnd();
}