#include "GL/freeglut.h"
#include "glextload.h"

#include <stdio.h>
#include <math.h>   // fabs

bool double_equal(double a, double b)
{
    if (fabs(a-b) < 1e-3)
        return true;
    return false;
}

static double zoom = .01;
static int width = 0;
static int height = 0;
static float thetax=0, thetay=0, thetaz=0;

//      Y
//      |           Z
//      |         /
//      |       /
//      |     /
//      |   /
//      | /
//      /--------------
//      O              X


float av[] = { 0.0, 0.0, 0.0,    // origin
               2.0, 0.0, 0.0,    // x-axis
               0.0, 2.0, 0.0,    // y-axis
               0.0, 0.0, 2.0 };  // z-axis

GLubyte avi[] = { 0, 1,
                  0, 2,
                  0, 3 };

float ac[] = { 1.0, 0.0, 0.0,    // red   x-axis
               0.0, 1.0, 0.0,    // green y-axis
               0.0, 0.0, 1.0 };  // blue  z-axis

GLubyte aci[] = { 0, 0,
                  1, 1,
                  2, 2 };

float ave[6*3];
void expandAxesVertices()
{
    for (int i=0; i<6; i++)
    {
        ave[i*3+0] = av[avi[i]*3+0];
        ave[i*3+1] = av[avi[i]*3+1];
        ave[i*3+2] = av[avi[i]*3+2];
    }
}

float ace[6*3];
void expandAxesColors()
{
    for (int i=0; i<6; i++)
    {
        ace[i*3+0] = ac[aci[i]*3+0];
        ace[i*3+1] = ac[aci[i]*3+1];
        ace[i*3+2] = ac[aci[i]*3+2];
    }
}

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

const int nFaces = 6;
const int nVerticesPerFace = 3;
const int nVertexComponents = 3;
const int nColorComponents = 3;

float v[] = { 1.0, 1.0, 1.0,    // 0
              1.0, 2.0, 1.0,    // 1
              2.0, 2.0, 1.0,    // 2
              2.0, 1.0, 1.0,    // 3
              1.5, 1.5, 2.0 };  // 4

GLubyte pvi[] = {0, 1, 2,
                 2, 3, 0,
                 0, 3, 4,
                 3, 2, 4,
                 2, 1, 4,
                 1, 0, 4};

float pve[nFaces*nVerticesPerFace*nVertexComponents];
void expandVertices()
{
    for (int i=0; i<nFaces; i++)
    {
        for (int j=0; j<nVerticesPerFace; j++)
        {
            for (int k=0; k<nVertexComponents; k++)
            {
                pve[(i*3+j)*3+k] = v[pvi[i*3+j]*3+k];
            }
        }
    }
}

float c[] = { 1.0, 1.0, 1.0,
              0.5, 0.5, 0.0,
              0.0, 0.5, 0.5,
              0.5, 0.0, 0.5,
              0.5, 0.5, 0.5 };

GLubyte pci[] = { 0, 0, 0,
                  0, 0, 0,
                  1, 1, 1,
                  2, 2, 2,
                  3, 3, 3,
                  4, 4, 4 };

float pce[nFaces*nVerticesPerFace*nColorComponents];
void expandColors()
{
    for (int i=0; i<nFaces; i++)
    {
        for (int j=0; j<nVerticesPerFace; j++)
        {
            for (int k=0; k<nColorComponents; k++)
            {
                pce[(i*3+j)*3+k] = c[pci[i*3+j]*3+k];
            }
        }
    }
}

float n[nFaces*nVerticesPerFace*nVertexComponents];

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

void calculateNormals()
{
    for (int i=0; i<6; i++)
    {
        float* v0=&v[pvi[i*3+0]*3];
        float v0x=v0[0], v0y=v0[1], v0z=v0[2];

        float* v1=&v[pvi[i*3+1]*3];
        float v1x=v1[0], v1y=v1[1], v1z=v1[2];

        float* v2=&v[pvi[i*3+2]*3];
        float v2x=v2[0], v2y=v2[1], v2z=v2[2];

        float ux = v1x - v0x;
        float uy = v1y - v0y;
        float uz = v1z - v0z;

        float vx = v2x - v0x;
        float vy = v2y - v0y;
        float vz = v2z - v0z;

        float nx = uy*vz - uz*vy;
        float ny = uz*vx - ux*vz;
        float nz = ux*vy - uy*vx;

        printf("\n ---------- calcNormal  ");
        printf("%g, %g, %g\n", nx, ny, nz);

        n[i*3*3+0]=nx;
        n[i*3*3+1]=ny;
        n[i*3*3+2]=nz;

        n[i*3*3+3]=nx;
        n[i*3*3+4]=ny;
        n[i*3*3+5]=nz;

        n[i*3*3+6]=nx;
        n[i*3*3+7]=ny;
        n[i*3*3+8]=nz;
    }
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

    printf("\n ------- setNormal ");
    printf("%g, %g, %g\n", nx, ny, nz);

    glNormal3f(nx, ny, nz);
}


static void drawVertexBufferObject()
{
    LoadGLExtensions();

    GLuint* aVboIds = new GLuint[3];
    glGenBuffers(3, aVboIds);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    //// Set axes data
    glBindBuffer(GL_ARRAY_BUFFER, aVboIds[0]);  // vertex
    glBufferData(GL_ARRAY_BUFFER, sizeof(ave), ave, GL_STATIC_DRAW);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, aVboIds[1]);  // color
    glBufferData(GL_ARRAY_BUFFER, sizeof(ace), ace, GL_STATIC_DRAW);
    glColorPointer(3, GL_FLOAT, 0, 0);
    glDrawArrays( GL_LINES, 0, 6 );

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    // Set pyramid data
    glBindBuffer(GL_ARRAY_BUFFER, aVboIds[0]);  // vertex
    glBufferData(GL_ARRAY_BUFFER, sizeof(pve), pve, GL_STATIC_DRAW);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, aVboIds[1]);  // color
    glBufferData(GL_ARRAY_BUFFER, sizeof(pce), pce, GL_STATIC_DRAW);
    glColorPointer(3, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, aVboIds[2]);  // normal
    glBufferData(GL_ARRAY_BUFFER, sizeof(n), n, GL_STATIC_DRAW);
    glNormalPointer(GL_FLOAT, 0, 0);

    // Draw pyramid
    glDrawArrays( GL_TRIANGLES, 0, 18 );

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

static void drawVertexArray()
{
    // Draw axes
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, ave);
    glColorPointer(3, GL_FLOAT, 0, ace);
    glDrawArrays( GL_LINES, 0, 6 );

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    // Draw pyramid
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, pve);
    glColorPointer(3, GL_FLOAT, 0, pce);
    glNormalPointer(GL_FLOAT, 0, n);

    glDrawArrays( GL_TRIANGLES, 0, 18 );

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

static void drawImmediate()
{
    calculateNormals();

    // x-axis in red
    glColor3d(1, 0, 0);
    glBegin(GL_LINES);
    glVertex3f(av[0], av[1], av[2]);
    glVertex3f(av[3], av[4], av[5]);
    glEnd();

    // y-axis in green
    glColor3d(0, 1, 0);
    glBegin(GL_LINES);
    glVertex3f(av[0], av[1], av[2]);
    glVertex3f(av[6], av[7], av[8]);
    glEnd();

    // z-axis in blue
    glColor3d(0, 0, 1);
    glBegin(GL_LINES);
    glVertex3f(av[0], av[1], av[2]);
    glVertex3f(av[9], av[10], av[11]);
    glEnd();

    glBegin(GL_TRIANGLES);
        glColor3d(1, 1, 1);
        glVertex3f(v[0], v[1], v[2]);   // 0
        glVertex3f(v[3], v[4], v[5]);   // 1
        glVertex3f(v[6], v[7], v[8]);   // 2
        //setNormal(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8]);
        glNormal3f(n[3*0+0], n[3*0+1], n[3*0+2]);

        glVertex3f(v[6], v[7], v[8]);   // 2
        glVertex3f(v[9], v[10], v[11]); // 3
        glVertex3f(v[0], v[1], v[2]);   // 0
        glNormal3f(n[3*1+0], n[3*1+1], n[3*1+2]);

        glColor3f(.5, .5, 0);
        glVertex3f(v[0], v[1], v[2]);       // 0
        glVertex3f(v[9], v[10], v[11]);     // 3
        glVertex3f(v[12], v[13], v[14]);    // 4
        //setNormal(v[0], v[1], v[2], v[9], v[10], v[11],  v[12], v[13], v[14]);
        glNormal3f(n[3*2+0], n[3*2+1], n[3*2+2]);

        glColor3f(0, .5, .5);
        glVertex3f(v[9], v[10], v[11]);     // 3
        glVertex3f(v[6], v[7], v[8]);       // 2
        glVertex3f(v[12], v[13], v[14]);    // 4
        //setNormal(v[9], v[10], v[11], v[6], v[7], v[8], v[12], v[13], v[14]);
        glNormal3f(n[3*3+0], n[3*3+1], n[3*3+2]);

        glColor3f(.5, 0, .5);
        glVertex3f(v[6], v[7], v[8]);       // 2
        glVertex3f(v[3], v[4], v[5]);       // 1
        glVertex3f(v[12], v[13], v[14]);    // 4
        //setNormal(v[6], v[7], v[8], v[3], v[4], v[5], v[12], v[13], v[14]);
        glNormal3f(n[3*4+0], n[3*4+1], n[3*4+2]);

        glColor3f(.5, .5, .5);
        glVertex3f(v[3], v[4], v[5]);       // 1
        glVertex3f(v[0], v[1], v[2]);       // 0
        glVertex3f(v[12], v[13], v[14]);    // 4
        //setNormal(v[3], v[4], v[5], v[0], v[1], v[2], v[12], v[13], v[14]);
        glNormal3f(n[3*5+0], n[3*5+1], n[3*5+2]);
    glEnd();
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

    //drawImmediate();
    //drawVertexArray();
    drawVertexBufferObject();

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

    expandAxesVertices();
    expandAxesColors();
    expandVertices();
    expandColors();
    calculateNormals();

    glutMainLoop();

    return 0;
}

