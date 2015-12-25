#include "GL/freeglut.h"
#include "glextload.h"

#include <stdio.h>
#include <math.h>   // fabs


static double zoom = .01;
static int width = 0;
static int height = 0;
static float tx=0, ty=0;
static float thetax=0, thetay=0, thetaz=0;

enum DrawType
{
    kImmediate,
    kVertexArray,
    kVBO,
    kShader,
    kVAO
};
DrawType drawType = kImmediate;

GLuint* vboIds = NULL;
GLuint* vaoIds = NULL;
GLuint program = 0;
GLuint SHADER_VERTEX_LOC_INDEX = 1;
GLuint SHADER_COLOR_LOC_INDEX = 2;

const int nVertexComponents = 3;
const int nColorComponents = 3;
const int nLines = 3;
const int nVerticesPerLine = 2;
const int nFaces = 6;
const int nVerticesPerFace = 3;

// ===========================================================================

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

float ave[nLines*nVerticesPerLine*nVertexComponents];
void expandAxesVertices()
{
    for (int i=0; i<6; i++)
    {
        ave[i*3+0] = av[avi[i]*3+0];
        ave[i*3+1] = av[avi[i]*3+1];
        ave[i*3+2] = av[avi[i]*3+2];
    }
}

float ace[nLines*nVerticesPerLine*nColorComponents];
void expandAxesColors()
{
    for (int i=0; i<6; i++)
    {
        ace[i*3+0] = ac[aci[i]*3+0];
        ace[i*3+1] = ac[aci[i]*3+1];
        ace[i*3+2] = ac[aci[i]*3+2];
    }
}

// ===========================================================================

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

float pv[] = { 0.5, 0.5, 0.5,    // 0
               0.5, 1.5, 0.5,    // 1
               1.5, 1.5, 0.5,    // 2
               1.5, 0.5, 0.5,    // 3
               1.0, 1.0, 1.5 };  // 4

GLubyte pvi[] = {0, 1, 2,
                 2, 3, 0,
                 0, 3, 4,
                 3, 2, 4,
                 2, 1, 4,
                 1, 0, 4};

float pve[nFaces*nVerticesPerFace*nVertexComponents];
void expandPyramidVertices()
{
    for (int i=0; i<nFaces; i++)
    {
        for (int j=0; j<nVerticesPerFace; j++)
        {
            for (int k=0; k<nVertexComponents; k++)
            {
                pve[(i*3+j)*3+k] = pv[pvi[i*3+j]*3+k];
            }
        }
    }
}

float pc[] = { 0.3f, 0.30f, 0.3f,
               1.0f, 0.70f, 0.0f,
               1.0f, 0.62f, 0.0f,
               1.0f, 0.40f, 0.0f,
               1.0f, 0.48f, 0.0f};

GLubyte pci[] = { 0, 0, 0,
                  0, 0, 0,
                  1, 1, 1,
                  2, 2, 2,
                  3, 3, 3,
                  4, 4, 4 };

float pce[nFaces*nVerticesPerFace*nColorComponents];
void expandPyramidColors()
{
    for (int i=0; i<nFaces; i++)
    {
        for (int j=0; j<nVerticesPerFace; j++)
        {
            for (int k=0; k<nColorComponents; k++)
            {
                pce[(i*3+j)*3+k] = pc[pci[i*3+j]*3+k];
            }
        }
    }
}

float n[nFaces*nVerticesPerFace*nVertexComponents];

// ===========================================================================

bool double_equal(double a, double b)
{
    if (fabs(a-b) < 1e-3)
        return true;
    return false;
}

// ---------------------------------------------------------------------------
static void displayCommands()
{
    printf("----- Key commands -----\n");
    printf("\n");
    printf("1           : Immediate\n");
    printf("2           : Vertex array\n");
    printf("3           : Vertex Buffer Object (VBO)\n");
    printf("4           : Shader with Vertex Buffer Object (VBO)\n");
    printf("5           : Shader with Vertex Array Object (VBO and VAO)\n");
    printf("\n");
    printf("+/=         : Zoom in\n");
    printf("-/_         : Zoom out\n");
    printf("\n");
    printf("Left arrow  : Pan left\n");
    printf("Right arrow : Pan left\n");
    printf("Up arrow    : Pan left\n");
    printf("Down arrow  : Pan left\n");
    printf("\n");
    printf("X           : Rotate +ve X axis\n");
    printf("x           : Rotate -ve X axis\n");
    printf("Y           : Rotate +ve Y axis\n");
    printf("X           : Rotate +ve Y axis\n");
    printf("Z           : Rotate +ve Z axis\n");
    printf("z           : Rotate -ve Z axis\n");
    printf("\n");
    printf("SPACE       : Reset view\n");
    printf("Q/q/ESC     : Quit\n");
}

// ---------------------------------------------------------------------------
static void onKeyPressed(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27 :
    case 'Q':
    case 'q': 
        glutLeaveMainLoop () ; break;

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
        tx=0; ty=0;
        thetax=0; thetay=0; thetaz=0;
        break;

    case 'x':
        thetax -= 10; break;
    case 'X':
        thetax += 10; break; 
    case 'y':
        thetay -= 10; break;
    case 'Y':
        thetay += 10; break;
    case 'z':
        thetaz -= 10; break;
    case 'Z':
        thetaz += 10; break;

    case '1':
        drawType = kImmediate; break;
    case '2':
        drawType = kVertexArray; break;
    case '3':
        drawType = kVBO; break;
    case '4':
        drawType = kShader; break;
    case '5':
        drawType = kVAO; break;
    default:
        break;
    }

    glutPostRedisplay();
}

// ---------------------------------------------------------------------------
static void onSpecialKeyPressed(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:     // left arrow
        tx -=0.25; break;
    case GLUT_KEY_RIGHT:    // right arrow
        tx +=0.25; break;
    case GLUT_KEY_UP:       // up arrow
        ty +=0.25; break;
    case GLUT_KEY_DOWN:     // down arrow
        ty -=0.25; break;
    }

    glutPostRedisplay();
}

// ---------------------------------------------------------------------------
static void onResize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0, 0, width, height);
}

// ---------------------------------------------------------------------------
const char* vertex_shader =
    "attribute vec3 aVertex;"
    "attribute vec3 aColor;"
    "uniform mat4 umvMat;"
    "uniform mat4 upMat;"
    "varying vec3 vColor;"
    "void main () {"
        "gl_Position = upMat * umvMat * vec4(aVertex, 1.0);"
        "vColor = aColor;"
    "}";

const char* fragment_shader =
    "varying vec3 vColor;"
    "void main () {"
        "gl_FragColor = vec4 (vColor, 1.0);"
    "}";

// ---------------------------------------------------------------------------
static void initShaders()
{
    GLuint vs = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vs, 1, &vertex_shader, NULL);
    glCompileShader (vs);

    GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fs, 1, &fragment_shader, NULL);
    glCompileShader (fs);

    program = glCreateProgram();
    glAttachShader (program, fs);
    glAttachShader (program, vs);

    glBindAttribLocation(program, SHADER_VERTEX_LOC_INDEX, "aVertex");
    glBindAttribLocation(program, SHADER_COLOR_LOC_INDEX, "aColor");

    glLinkProgram (program);

    glUseProgram (program);
}

// ---------------------------------------------------------------------------
static void defineVAO()
{
    vaoIds = new GLuint[2];
    glGenVertexArrays(2, vaoIds);

    vboIds = new GLuint[4];
    glGenBuffers(4, vboIds);

    GLint shaderVertexLocIndex = glGetAttribLocation(program, "aVertex");
    GLint shaderColorLocIndex = glGetAttribLocation(program, "aColor");

    // set current (bind) VAO to define axes data
    glBindVertexArray(vaoIds[0]);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);  // vertex
    glBufferData(GL_ARRAY_BUFFER, sizeof(ave), ave, GL_STATIC_DRAW);
    glVertexAttribPointer(shaderVertexLocIndex, nVertexComponents, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(shaderVertexLocIndex);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);  // color
    glBufferData(GL_ARRAY_BUFFER, sizeof(ace), ace, GL_STATIC_DRAW);
    glVertexAttribPointer(shaderColorLocIndex, nColorComponents, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(shaderColorLocIndex);

    // Set current (bind) VAO to define vertex data
    glBindVertexArray(vaoIds[1]);
    
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[2]);  // vertex
    glBufferData(GL_ARRAY_BUFFER, sizeof(pve), pve, GL_STATIC_DRAW);
    glVertexAttribPointer(shaderVertexLocIndex, nVertexComponents, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(shaderVertexLocIndex);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[3]);  // color
    glBufferData(GL_ARRAY_BUFFER, sizeof(pce), pce, GL_STATIC_DRAW);
    glVertexAttribPointer(shaderColorLocIndex, nColorComponents, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(shaderColorLocIndex);

    // Disable VAO
    glBindVertexArray(0);
}

// ---------------------------------------------------------------------------
static void drawShaderWithVertexArrayObject()
{
    LoadGLExtensions();

    initShaders();
    defineVAO();

    // Get the variables from the shader to which data will be passed
    GLint mvloc = glGetUniformLocation(program, "umvMat");
    GLint ploc = glGetUniformLocation(program, "upMat");

    // Pass the model-view matrix to the shader
    GLfloat mvMat[16]; 
    glGetFloatv(GL_MODELVIEW_MATRIX, mvMat); 
    glUniformMatrix4fv(mvloc, 1, false, mvMat);

    // Pass the project matrix to the shader
    GLfloat pMat[16]; 
    glGetFloatv(GL_PROJECTION_MATRIX, pMat); 
    glUniformMatrix4fv(ploc, 1, false, pMat);

    // Enable VAO to set axes data
    glBindVertexArray(vaoIds[0]);
    
    // Draw axes
    glDrawArrays(GL_LINES, 0, nLines*nVerticesPerLine);

    // Enable VAO to set pyramid data
    glBindVertexArray(vaoIds[1]);

    // Draw pyramid
    glDrawArrays(GL_TRIANGLES, 0, nFaces*nVerticesPerFace);

    // Disable VAO
    glBindVertexArray(0);
}

// ---------------------------------------------------------------------------
static void drawShaderWithVertexBufferObject()
{
    LoadGLExtensions();

    initShaders();

    // Get the variables from the shader to which data will be passed
    GLint mvloc = glGetUniformLocation(program, "umvMat");
    GLint ploc = glGetUniformLocation(program, "upMat");
    GLint shaderVertexLocIndex = glGetAttribLocation(program, "aVertex");
    GLint shaderColorLocIndex = glGetAttribLocation(program, "aColor");

    // Pass the model-view matrix to the shader
    GLfloat mvMat[16]; 
    glGetFloatv(GL_MODELVIEW_MATRIX, mvMat); 
    glUniformMatrix4fv(mvloc, 1, false, mvMat);

    // Pass the project matrix to the shader
    GLfloat pMat[16]; 
    glGetFloatv(GL_PROJECTION_MATRIX, pMat); 
    glUniformMatrix4fv(ploc, 1, false, pMat);

    vboIds = new GLuint[4];
    glGenBuffers(4, vboIds);

    // Set axes data
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);  // vertex
    glBufferData(GL_ARRAY_BUFFER, sizeof(ave), ave, GL_STATIC_DRAW);
    glVertexAttribPointer(shaderVertexLocIndex, nVertexComponents, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(shaderVertexLocIndex);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);  // color
    glBufferData(GL_ARRAY_BUFFER, sizeof(ace), ace, GL_STATIC_DRAW);
    glVertexAttribPointer(shaderColorLocIndex, nColorComponents, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(shaderColorLocIndex);

    // Draw axes
    glDrawArrays(GL_LINES, 0, nLines*nVerticesPerLine);

    // Set pyramid data
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[2]);  // vertex
    glBufferData(GL_ARRAY_BUFFER, sizeof(pve), pve, GL_STATIC_DRAW);
    glVertexAttribPointer(shaderVertexLocIndex, nVertexComponents, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(shaderVertexLocIndex);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[3]);  // color
    glBufferData(GL_ARRAY_BUFFER, sizeof(pce), pce, GL_STATIC_DRAW);
    glVertexAttribPointer(shaderColorLocIndex, nColorComponents, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(shaderColorLocIndex);

    // Draw pyramid
    glDrawArrays(GL_TRIANGLES, 0, nFaces*nVerticesPerFace);

    // Disable the VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// ---------------------------------------------------------------------------
static void drawVertexBufferObject()
{
    LoadGLExtensions();

    vboIds = new GLuint[3];
    glGenBuffers(3, vboIds);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Set axes data
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);  // vertex
    glBufferData(GL_ARRAY_BUFFER, sizeof(ave), ave, GL_STATIC_DRAW);
    glVertexPointer(nVertexComponents, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[1]);  // color
    glBufferData(GL_ARRAY_BUFFER, sizeof(ace), ace, GL_STATIC_DRAW);
    glColorPointer(nColorComponents, GL_FLOAT, 0, 0);

    // Draw axes
    glDrawArrays(GL_LINES, 0, nLines*nVerticesPerLine);

    // Set pyramid data
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[2]);  // vertex
    glBufferData(GL_ARRAY_BUFFER, sizeof(pve), pve, GL_STATIC_DRAW);
    glVertexPointer(nVertexComponents, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[3]);  // color
    glBufferData(GL_ARRAY_BUFFER, sizeof(pce), pce, GL_STATIC_DRAW);
    glColorPointer(nColorComponents, GL_FLOAT, 0, 0);

    // Draw pyramid
    glDrawArrays(GL_TRIANGLES, 0, nFaces*nVerticesPerFace);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    // Disable the VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// ---------------------------------------------------------------------------
static void drawVertexArray()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Set axes data
    glVertexPointer(nVertexComponents, GL_FLOAT, 0, ave);
    glColorPointer(nColorComponents, GL_FLOAT, 0, ace);

    // Draw axes
    glDrawArrays(GL_LINES, 0, nLines*nVerticesPerLine);

    // Set pyramid data
    glVertexPointer(nVertexComponents, GL_FLOAT, 0, pve);
    glColorPointer(nColorComponents, GL_FLOAT, 0, pce);

    // Draw pyramid
    glDrawArrays(GL_TRIANGLES, 0, nFaces*nVerticesPerFace);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

// ---------------------------------------------------------------------------
static void drawImmediate()
{
    // Draw x-axis in red
    glColor3d(ac[0], ac[1], ac[2]);
    glBegin(GL_LINES);
        glVertex3f(av[0], av[1], av[2]);
        glVertex3f(av[3], av[4], av[5]);
    glEnd();

    // Draw y-axis in green
    glColor3d(ac[3], ac[4], ac[5]);
    glBegin(GL_LINES);
        glVertex3f(av[0], av[1], av[2]);
        glVertex3f(av[6], av[7], av[8]);
    glEnd();

    // Draw z-axis in blue
    glColor3d(ac[6], ac[7], ac[8]);
    glBegin(GL_LINES);
        glVertex3f(av[0], av[1], av[2]);
        glVertex3f(av[9], av[10], av[11]);
    glEnd();

    // Draw pyramid
    glBegin(GL_TRIANGLES);
        glColor3d(pc[0], pc[1], pc[2]);

        glVertex3f(pv[0], pv[1], pv[2]);       // 0
        glVertex3f(pv[3], pv[4], pv[5]);       // 1
        glVertex3f(pv[6], pv[7], pv[8]);       // 2

        glVertex3f(pv[6], pv[7],  pv[8]);      // 2
        glVertex3f(pv[9], pv[10], pv[11]);     // 3
        glVertex3f(pv[0], pv[1],  pv[2]);      // 0

        glColor3f(pc[3], pc[4], pc[5]);

        glVertex3f(pv[0],  pv[1],  pv[2]);     // 0
        glVertex3f(pv[9],  pv[10], pv[11]);    // 3
        glVertex3f(pv[12], pv[13], pv[14]);    // 4

        glColor3f(pc[6], pc[7], pc[8]);

        glVertex3f(pv[9],  pv[10], pv[11]);    // 3
        glVertex3f(pv[6],  pv[7],  pv[8]);     // 2
        glVertex3f(pv[12], pv[13], pv[14]);    // 4

        glColor3f(pc[9], pc[10], pc[11]);

        glVertex3f(pv[6],  pv[7],  pv[8]);     // 2
        glVertex3f(pv[3],  pv[4],  pv[5]);     // 1
        glVertex3f(pv[12], pv[13], pv[14]);    // 4

        glColor3f(pc[12],  pc[13], pc[14]);

        glVertex3f(pv[3],  pv[4],  pv[5]);     // 1
        glVertex3f(pv[0],  pv[1],  pv[2]);     // 0
        glVertex3f(pv[12], pv[13], pv[14]);    // 4
    glEnd();
}

// ---------------------------------------------------------------------------
static void onDisplay(void)
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

    glTranslatef(tx, ty, 0);

    if (drawType == kImmediate)
        drawImmediate();
    else if (drawType == kVertexArray)
        drawVertexArray();
    else if (drawType == kVBO)
        drawVertexBufferObject();
    else if (drawType == kShader)
        drawShaderWithVertexBufferObject();
    else if (drawType == kVAO)
        drawShaderWithVertexArrayObject();

    glutSwapBuffers();
}

// ---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(450,450);
    glutInitWindowPosition(40,40);
    
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutCreateWindow("Basic OpenGL using FreeGLUT");

    glutReshapeFunc(onResize);
    glutDisplayFunc(onDisplay);
    glutKeyboardFunc(onKeyPressed);
    glutSpecialFunc(onSpecialKeyPressed);

    glClearColor(0,0,0,.5);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST); // z-buffer test
    //glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);

    expandAxesVertices();
    expandAxesColors();
    expandPyramidVertices();
    expandPyramidColors();

    displayCommands();

    glutMainLoop();

    return 0;
}
