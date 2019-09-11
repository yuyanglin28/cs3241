//============================================================
// STUDENT NAME: LIN YUYANG
// STUDENT NO.: A0207526H
// NUS EMAIL ADDRESS: e0445621@u.nus.edu
// COMMENTS TO GRADER: 
// 
// ============================================================

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <GL/freeglut.h>

/////////////////////////////////////////////////////////////////////////////
// CONSTANTS
/////////////////////////////////////////////////////////////////////////////

#define PI                  3.1415926535897932384626433832795

#define MAX_NUM_OF_DISCS    200     // Limit the number of discs.
#define MIN_RADIUS          10.0    // Minimum radius of disc.
#define MAX_RADIUS          50.0    // Maximum radius of disc.
#define NUM_OF_SIDES        18      // Number of polygon sides to approximate a disc.

#define MIN_X_SPEED         1.0     // Minimum speed of disc in X direction.
#define MAX_X_SPEED         20.0    // Maximum speed of disc in X direction.
#define MIN_Y_SPEED         1.0     // Minimum speed of disc in Y direction.
#define MAX_Y_SPEED         20.0    // Maximum speed of disc in Y direction.

#define DESIRED_FPS         30      // Approximate desired number of frames per second.


/////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
/////////////////////////////////////////////////////////////////////////////

typedef struct discType 
{
    double pos[2];          // The X and Y coordinates of the center of the disc.
    double speed[2];        // The velocity of the disc in X and Y directions. Can be negative.
    double radius;          // Radius of the disc.
    unsigned char color[3]; // R, G, B colors of the disc.
} discType;


int numDiscs = 0;                   // Number of discs that have been added.

discType disc[ MAX_NUM_OF_DISCS ];  // Array for storing discs.

bool drawWireframe = false;         // Draw polygons in wireframe if true, otherwise
                                    // otherwise polygons are filled.

int winWidth = 800;                 // Window width in pixels.
int winHeight = 600;                // Window height in pixels.

int viewWidth = 800;                //view volume width in pixels
int viewHeight = 600;               //view volume height in pixels

double ratio;                       //ratio = view volume size / window size

/////////////////////////////////////////////////////////////////////////////
// Draw the disc in its color using GL_TRIANGLE_FAN.
/////////////////////////////////////////////////////////////////////////////

void DrawDisc( const discType *d )
{
    static bool firstTime = true;
    static double unitDiscVertex[ NUM_OF_SIDES + 1 ][2];
    

    if ( firstTime )
    {
        // Pre-compute and store the vertices' positions of a unit-radius disc.
        firstTime = false;
        double angle = 2*PI / NUM_OF_SIDES;
        for (int i=0; i<=NUM_OF_SIDES; i++){
            unitDiscVertex[i][0] = cos(i*angle);
            unitDiscVertex[i][1] = sin(i*angle);
        }
    }
    
    // Draw the disc in its color as a GL_TRAINGLE_FAN.
    glColor3ub( (char) d->color[0], (char) d->color[1], (char) d->color[2]);
    double r = d->radius * ratio;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(d->pos[0], d->pos[1]);
    for (int i=0; i<=NUM_OF_SIDES; i++){
        glVertex2d(unitDiscVertex[i][0]*r + d->pos[0], unitDiscVertex[i][1]*r + d->pos[1]);
    }
    glEnd();
}





/////////////////////////////////////////////////////////////////////////////
// The mouse callback function.
// If mouse left button is pressed, a new disc is created with its center 
// at the mouse cursor position. The new disc is assigned the followings:
// (1) a random radius between MIN_RADIUS and MAX_RADIUS,
// (2) a random speed in X direction in the range 
//     from -MAX_X_SPEED to -MIN_X_SPEED, and from MIN_X_SPEED to MAX_X_SPEED.
// (3) a random speed in Y direction in the range 
//     from -MAX_Y_SPEED to -MIN_Y_SPEED, and from MIN_Y_SPEED to MAX_Y_SPEED.
// (4) R, G, B color, each ranges from 0 to 255.
/////////////////////////////////////////////////////////////////////////////

void MyMouse( int btn, int state, int x, int y )
{
    if ( btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        if ( numDiscs >= MAX_NUM_OF_DISCS )
            printf( "Already reached maximum number of discs.\n" );
        else
        {
            discType newDisc;
            newDisc.radius = ((double)rand() / (double)RAND_MAX) * (MAX_RADIUS - MIN_RADIUS) + MIN_RADIUS;
            
            double xs = ((double)rand() / (double)RAND_MAX) * (MAX_X_SPEED - MIN_X_SPEED) * 2 ;
            double midX = MAX_X_SPEED - MIN_X_SPEED;
            if (xs < midX){ xs = xs - MAX_X_SPEED; }else { xs = xs - midX + MIN_X_SPEED;}
            
            double ys = ((double)rand() / (double)RAND_MAX) * (MAX_Y_SPEED - MIN_Y_SPEED) * 2 ;
            double midY = MAX_Y_SPEED - MIN_Y_SPEED;
            if (ys < midY){ ys = ys - MAX_X_SPEED; }else { ys = ys - midY + MIN_Y_SPEED;}
            
            newDisc.speed[0] = xs;
            newDisc.speed[1] = ys;
            
            char R = rand() % 256;
            char G = rand() % 256;
            char B = rand() % 256;
            newDisc.color[0] = R;
            newDisc.color[1] = G;
            newDisc.color[2] = B;
            
             int winX = x;
             int winY = winHeight-1-y;
            
            //change to the position in view volume
            newDisc.pos[0] = viewWidth * winX / winWidth;
            newDisc.pos[1] = viewHeight * winY / winHeight;
            
            disc[numDiscs] = newDisc;
            numDiscs++;
           
            glutPostRedisplay();
        }
    }
}



/////////////////////////////////////////////////////////////////////////////
// The reshape callback function.
// It also sets up the viewing.
/////////////////////////////////////////////////////////////////////////////

void MyReshape( int w, int h )
{
    winWidth = w;
    winHeight = h;

    glViewport( 0, 0, w, h );

    glMatrixMode( GL_PROJECTION );

    glLoadIdentity();
    if (w >= 800 & h >= 600){
        glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
        viewWidth = w;
        viewHeight = h;
        ratio = 1;
    }else {
        if ((double)w/h > (double)4.0/3.0){
           glOrtho(0.0, (GLdouble)600*w/h, 0.0, (GLdouble)600, -1.0, 1.0);
            viewWidth = 600*w/h;
            viewHeight = 600;
            ratio = (double) 600/h;
        }
        else{
            glOrtho(0.0, (GLdouble)800, 0.0, (GLdouble)800*h/w, -1.0, 1.0);
            viewWidth = 800;
            viewHeight = 800*h/w;
            ratio = (double)800/w;
        }
    }

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}



/////////////////////////////////////////////////////////////////////////////
// The keyboard callback function.
/////////////////////////////////////////////////////////////////////////////

void MyKeyboard( unsigned char key, int x, int y )
{
    switch ( key )
    {
        // Quit program.
        case 'q':
        case 'Q': exit(0);
                  break;

        // Toggle wireframe or filled polygons.
        case 'w':
        case 'W': drawWireframe = !drawWireframe;
                  glutPostRedisplay();
                  break;

        // Reset and erase all discs.
        case 'r':
        case 'R': numDiscs = 0;
                  glutPostRedisplay();
                  break;
    }
}



/////////////////////////////////////////////////////////////////////////////
// Updates the positions of all the discs.
//
// Increments the position of each disc by its speed in each of the
// X and Y directions. Note that the speeds can be negative.
// At its new position, if the disc is entirely or partially outside the
// left window boundary, then shift it right so that it is inside the
// window and just touches the left window boundary. Its speed in the X 
// direction must now be reversed (negated). Similar approach is 
// applied for the cases of the right, top, and bottom window boundaries.
/////////////////////////////////////////////////////////////////////////////

void UpdateAllDiscPos( void )
{
    for ( int i = 0; i < numDiscs; i++ )
    {
        disc[i].pos[0] += disc[i].speed[0]*ratio;
        disc[i].pos[1] += disc[i].speed[1]*ratio;

        if (disc[i].pos[0] <= 0 ){ //left
            disc[i].speed[0] *= -1; //change x direction speed
            disc[i].pos[0] = 1;
        }else if (disc[i].pos[0] >= viewWidth){ //right
            disc[i].speed[0] *= -1; //change x direction speed
            disc[i].pos[0] = viewWidth - 1;
        }else if (disc[i].pos[1] >= viewHeight){ //top
             disc[i].speed[1] *= -1;//change y direction speed
            disc[i].pos[1] = viewHeight - 1;
        }else if (disc[i].pos[1] <= 0){ //tbottom
            disc[i].speed[1] *= -1; //change y direction speed
            disc[i].pos[1] = 1;
        }
    }
    glutPostRedisplay();
}



/////////////////////////////////////////////////////////////////////////////
// The timer callback function.
/////////////////////////////////////////////////////////////////////////////

void MyTimer( int v )
{
    UpdateAllDiscPos();
}



/////////////////////////////////////////////////////////////////////////////
// The init function.
// It initializes some OpenGL states.
/////////////////////////////////////////////////////////////////////////////

void MyInit( void )
{
    glClearColor( 0.0, 0.0, 0.0, 1.0 ); // Black background color.
    glShadeModel( GL_FLAT );
}

/////////////////////////////////////////////////////////////////////////////
// The display callback function.
/////////////////////////////////////////////////////////////////////////////

void MyDisplay( void )
{
    glClear( GL_COLOR_BUFFER_BIT );
    
    if ( drawWireframe )
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    else
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    
    for ( int i = 0; i < numDiscs; i++ ) DrawDisc( &disc[i] );
    
    glutTimerFunc(1000/DESIRED_FPS, MyTimer, 0);
    
    //glFlush();  //*** MODIFY THIS ***
    glutSwapBuffers();
}

 



/////////////////////////////////////////////////////////////////////////////
// The main function.
/////////////////////////////////////////////////////////////////////////////

int main( int argc, char** argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );  //*** MODIFY THIS ***
    glutInitWindowSize( winWidth, winHeight );
    glutCreateWindow( "main" );

    MyInit();

    // Register the callback functions.
    glutDisplayFunc( MyDisplay );
    glutReshapeFunc( MyReshape );
    glutMouseFunc( MyMouse );
    glutKeyboardFunc( MyKeyboard );
    //glutIdleFunc( UpdateAllDiscPos );  //*** MODIFY THIS ***
    
    // Display user instructions in console window.
    printf( "Click LEFT MOUSE BUTTON in window to add new disc.\n" );
    printf( "Press 'w' to toggle wireframe.\n" );
    printf( "Press 'r' to erase all discs.\n" );
    printf( "Press 'q' to quit.\n\n" );

    // Enter GLUT event loop.
    glutMainLoop();
    return 0;
}
