#include "GL/freeglut.h"

#include <stdio.h>
#include <math.h>   // fabs

static int slices = 16;
static int stacks = 16;

static double zoom = .01;
static int width = 0;
static int height = 0;
static float thetax=0, thetay=0, thetaz=0;

float a[] = { 0.0, 0.0, 0.0,    // origin
              2.0, 0.0, 0.0,    // x-axis
              0.0, 2.0, 0.0,    // y-axis
              0.0, 0.0, 2.0 };  // z-axis


//  (3,4,5)          (6,7,8)
//     1----------------2
//     | \            / |
//     |   \        /   |
//     |     \    /     |
//     |        4       | (12,13,14)
//     |     /    \     |
//     |   /        \   |
//     | /            \ |
//     0 ---------------3
//  (0,1,2)          (9,10,11)

float v[] = { 1.0, 1.0, 1.0,    // 0
              1.0, 2.0, 1.0,    // 1
              2.0, 2.0, 1.0,    // 2
              2.0, 1.0, 1.0,    // 3
              1.5, 1.5, 2.0 };  // 4

bool double_equal(double a, double b)
{
    if (fabs(a-b) < 1e-3)
        return true;
    return false;
}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27 :
    case 'Q':
    case 'q': glutLeaveMainLoop () ;      break;

    case '=':
    case '+':
        zoom=zoom-0.001; 
        if (double_equal(zoom, 0))
           zoom = 0.001;
        break;

    case '-':
    case '_': 
        zoom = zoom+0.001;
        if (double_equal(zoom, 0))
            zoom = 0.001;
        break;

    case ' ':
        zoom = 0.01;
        thetax=0;
        thetay=0;
        thetaz=0;
        break;

    case 'x':
        thetax -= 10;
        break;
    case 'X':
        thetax += 10;
        break;

    case 'y':
        thetay -= 10;
        break;
    case 'Y':
        thetay += 10;
        break;

    case 'z':
        thetaz -= 10;
        break;
    case 'Z':
        thetaz += 10;
        break;

    default:
        break;
    }

    glutPostRedisplay();
}

static void resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0, 0, width, height);
}

void setNormal(float v1x, float v1y, float v1z,
               float v2x, float v2y, float v2z, 
               float v3x, float v3y, float v3z)
{
    float ux = v2x - v1x;
    float uy = v2y - v1y;
    float uz = v2z - v1z;

    float vx = v3x - v1x;
    float vy = v3y - v1y;
    float vz = v3z - v1z;

    float nx = uy*vz - uz*vy;
    float ny = uz*vx - ux*vz;
    float nz = ux*vy - uy*vx;

    glNormal3f(nx, ny, nz);
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho((-width/2)*zoom, (width/2)*zoom, (-height/2)*zoom, (height/2)*zoom, -10, 10);

    gluLookAt(0, 0, 1,  0, 0, 0,  0, 1, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(thetax, 1, 0, 0);
    glRotatef(thetay, 0, 1, 0);
    glRotatef(thetaz, 0, 0, 1);

    // x-axis in red
    glColor3d(1, 0, 0);
    glBegin(GL_LINES);
    glVertex3f(a[0], a[1], a[2]);
    glVertex3f(a[3], a[4], a[5]);
    glEnd();

    // y-axis in green
    glColor3d(0, 1, 0);
    glBegin(GL_LINES);
    glVertex3f(a[0], a[1], a[2]);
    glVertex3f(a[6], a[7], a[8]);
    glEnd();

    // z-axis in blue
    glColor3d(0, 0, 1);
    glBegin(GL_LINES);
    glVertex3f(a[0], a[1], a[2]);
    glVertex3f(a[9], a[10], a[11]);
    glEnd();

    glBegin(GL_QUADS);
        glColor3d(1, 1, 1);
        glVertex3f(v[0], v[1], v[2]);   // 0
        glVertex3f(v[3], v[4], v[5]);   // 1
        glVertex3f(v[6], v[7], v[8]);   // 2
        glVertex3f(v[9], v[10], v[11]); // 3
        setNormal(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8]);
    glEnd();

    glBegin(GL_TRIANGLES);
        glColor3f(.5, .5, 0);
        glVertex3f(v[0], v[1], v[2]);       // 0
        glVertex3f(v[9], v[10], v[11]);     // 3
        glVertex3f(v[12], v[13], v[14]);    // 4
        setNormal(v[0], v[1], v[2], v[9], v[10], v[11],  v[12], v[13], v[14]);

        glColor3f(0, .5, .5);
        glVertex3f(v[9], v[10], v[11]);     // 3
        glVertex3f(v[6], v[7], v[8]);       // 2
        glVertex3f(v[12], v[13], v[14]);    // 4
        setNormal(v[9], v[10], v[11], v[6], v[7], v[8], v[12], v[13], v[14]);

        glColor3f(.5, 0, .5);
        glVertex3f(v[6], v[7], v[8]);       // 2
        glVertex3f(v[3], v[4], v[5]);       // 1
        glVertex3f(v[12], v[13], v[14]);    // 4
        setNormal(v[6], v[7], v[8], v[3], v[4], v[5], v[12], v[13], v[14]);

        glColor3f(.5, .5, .5);
        glVertex3f(v[3], v[4], v[5]);       // 1
        glVertex3f(v[0], v[1], v[2]);       // 0
        glVertex3f(v[12], v[13], v[14]);    // 4
        setNormal(v[3], v[4], v[5], v[0], v[1], v[2], v[12], v[13], v[14]);
    glEnd();

    glutSwapBuffers();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(800,600);
    glutInitWindowPosition(40,40);
    
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutCreateWindow("Basic OpenGL using FreeGLUT");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);

    glClearColor(0,0,0,.5);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST); // z-buffer test
    //glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);

    const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
    const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat light_specular[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    const GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glutMainLoop();

    return 0;
}

