/////////////////////////////////////////////////////////////////////////////////
// RileyEricksonFinalProject
//
// Forward-compatible core GL 4.3 version of fieldAndSkyFiltered.cpp (except only
// one filter is implemented).

// Comparing different filtering techniques
//
// Interaction:
// Press the up and down arrow keys to move the viewpoint over the field.
//
// Sumanta Guha
//
// Texture Credits: See ExperimenterSource/Textures/TEXTURE_CREDITS.txt
/////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include <fstream>

#ifdef __APPLE__
#  include <GL/glew.h>
#  include <GLUT/freeglut.h>
#  include <OpenGL/glext.h>
#else
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#  include <GL/glext.h>
#pragma comment(lib, "glew32.lib")
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "shader.h"
#include "getbmp.h"
#include "vertex.h"

#define PI 3.14159265

using namespace std;
using namespace glm;

enum object {FIELD, SKY}; // VAO ids.
enum buffer {FIELD_VERTICES, SKY_VERTICES}; // VBO ids.

int width = 800;
int height = 800;

// Globals.
static Vertex fieldVertices[4] =
{
    {vec4(100.0, 0.0, 100.0, 1.0), vec2(40.0, 0.0)},
    {vec4(100.0, 0.0, -100.0, 1.0), vec2(40.0, 40.0)},
    {vec4(-100.0, 0.0, 100.0, 1.0), vec2(0.0, 0.0)},
    {vec4(-100.0, 0.0, -100.0, 1.0), vec2(0.0, 40.0)}
};

static Vertex skyVertices[4] =
{
    {vec4(100.0, -100.0, -100.0, 1.0), vec2(1.0, 0.0)},
    {vec4(100.0, 120.0, -100.0, 1.0), vec2(1.0, 1.0)},
    {vec4(-100.0, -100.0, -100.0, 1.0), vec2(0.0, 0.0)},
    {vec4(-100.0, 120.0, -100.0, 1.0), vec2(0.0, 1.0)}
};

static mat4 modelViewMat = mat4(1.0);
static mat4 projMat = mat4(1.0);

int lookX = 0, lookY = 0;
int posX = 0, posZ = 0;

static float vertexAngle = 0.0; // vertexAngle of the spacecraft.
static float xVal = 0, zVal = 0, yVal; // Co-ordinates of the spacecraft.

static unsigned int
programId,
vertexShaderId,
fragmentShaderId,
modelViewMatLoc,
projMatLoc,
grassTexLoc,
skyTexLoc[5],
objectLoc,
buffer[2],
vao[2],
texture[6];

static BitMapFile *image[6]; // Local storage for bmp image data.

void printControls()
{
    cout << "w ................. move forward" << "\n";
    cout << "a ................. move left" << "\n";
    cout << "s ................. move backward" << "\n";
    cout << "d ................. move right" << "\n";
    cout << "mouse ............. move camera" << "\n";
    cout << "Esc ............... quit" << "\n";

}

// Initialization routine.
void setup(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);

    // Create shader program executable.
    vertexShaderId = setShader("vertex", "vertexShader.glsl");
    fragmentShaderId = setShader("fragment", "fragmentShader.glsl");
    programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);
    glUseProgram(programId);

    // Create VAOs and VBOs...
    glGenVertexArrays(2, vao);
    glGenBuffers(2, buffer);

    // ...and associate data with vertex shader.
    glBindVertexArray(vao[FIELD]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[FIELD_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fieldVertices), fieldVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(fieldVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(fieldVertices[0]), (void*)(sizeof(fieldVertices[0].coords)));
    glEnableVertexAttribArray(1);

    // ...and associate data with vertex shader.
    glBindVertexArray(vao[SKY]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[SKY_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), skyVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(skyVertices[0]), 0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(skyVertices[0]), (void*)(sizeof(skyVertices[0].coords)));
    glEnableVertexAttribArray(3);

    // Obtain projection matrix uniform location and set value.
    projMatLoc = glGetUniformLocation(programId,"projMat");
    projMat = frustum(-5.0, 5.0, -5.0, 5.0, 5.0, 1000.0);
    glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));

    // Obtain modelview matrix uniform and object uniform locations.
    modelViewMatLoc = glGetUniformLocation(programId,"modelViewMat");
    objectLoc = glGetUniformLocation(programId, "object");

    // Load the images.
    image[0] = getbmp("grass1.bmp");
    image[1] = getbmp("front.bmp");
    image[2] = getbmp("back.bmp");
    image[3] = getbmp("left.bmp");
    image[4] = getbmp("right.bmp");
    image[5] = getbmp("top.bmp");
    image[6] = getbmp("bottom.bmp");
    // Create texture ids.
    glGenTextures(6, texture);

    // Bind grass image.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->sizeX, image[0]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Use mipmap texture filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    grassTexLoc = glGetUniformLocation(programId, "grassTex");
    glUniform1i(grassTexLoc, 0);

    // Bind sky image.
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->sizeX, image[1]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    skyTexLoc[0] = glGetUniformLocation(programId, "skyTex");
    glUniform1i(skyTexLoc[0], 1);
    printControls();
}

// Drawing routine.
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate and update modelview matrix.
    modelViewMat = mat4(1.0);
    //modelViewMat = lookAt(vec3(posX - 10 * sin( (PI / 180.0)), 10.0, posZ + 5 * cos( (PI / 180.0))), vec3(lookX * 0.1, (-lookY * 0.1) + 10, 0.0), vec3(0.0, 1.0, 0.0));
    modelViewMat = lookAt(vec3(xVal - 10 * sin( (PI / 180.0) * vertexAngle),10.0,zVal - 10 * cos( (PI / 180.0) * vertexAngle)), vec3((-lookX*0.01)*(xVal - 11 * sin( (PI / 180.0) * vertexAngle)), (-lookY*0.01) + 10.0,zVal - 11 * cos( (PI / 180.0) * vertexAngle)),vec3(0.0, 1.0,0.0));

    /*    gluLookAt(xVal - 10 * sin( (PI / 180.0) * angle),
               0.0,
               zVal - 10 * cos( (PI / 180.0) * angle),
               xVal - 11 * sin( (PI / 180.0) * angle),
               0.0,
               zVal - 11 * cos( (PI / 180.0) * angle),
               0.0,
               1.0,
               0.0);
     lookAt(vec3(posX,  10, posZ + 15), vec3(lookX * 0.1, (-lookY * 0.1) + 10, 0.0), vec3(0.0, 1.0, 0.0));
    */
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));

    // Draw field.
    glUniform1ui(objectLoc, FIELD);
    glBindVertexArray(vao[FIELD]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Draw sky.
    glUniform1ui(objectLoc, SKY);
    glBindVertexArray(vao[SKY]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glutSwapBuffers();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
    glViewport(0, 0, w, h);
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
    float tempxVal = xVal, tempzVal = zVal, tempvertexAngle = vertexAngle;
    switch(key)
    {
    case 'w':
        tempxVal = xVal - sin(vertexAngle * PI / 180.0);
        tempzVal = zVal - cos(vertexAngle * PI / 180.0);
        break;
    case 'a':
        tempxVal = xVal - cos(vertexAngle * PI / 180.0);
        tempzVal = zVal - sin(vertexAngle * PI / 180.0);
        break;
    case 's':
        tempxVal = xVal + sin(vertexAngle * PI / 180.0);
        tempzVal = zVal + cos(vertexAngle * PI / 180.0);
        break;
    case 'd':
        tempxVal = xVal + cos(vertexAngle * PI / 180.0);
        tempzVal = zVal + sin(vertexAngle * PI / 180.0);
        break;
    case 27:
        exit(0);
        break;
    default:
        break;
    }

    if (tempvertexAngle > 360.0) tempvertexAngle -= 360.0;
    if (tempvertexAngle < 0.0) tempvertexAngle += 360.0;

    xVal = tempxVal;
    zVal = tempzVal;
    vertexAngle = tempvertexAngle;
    glutPostRedisplay();
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
    glutPostRedisplay();
}

void mouse(  GLint x, GLint y )
{
    //glutWarpPointer(width /2 ,height /2); //To warp the cursor to a screen position
    //glutSetCursor(GLUT_CURSOR_NONE); //To hide the cursor
    //cout << "x = " << x << ", y = " << y << endl;
    lookX = x - (width / 2);
    lookY = y - (height / 2);


    //cout << "lookX = " << lookX << ", lookY = " << lookY << endl;
    glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
    cout << "Interaction:" << endl;
    cout << "Press the up and down arrow keys to move the viewpoint over the field." << endl;
}

// Main routine.
int main(int argc, char **argv)
{
    printInteraction();
    glutInit(&argc, argv);

    glutInitContextVersion(4, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(0,0);
    glutCreateWindow("a field and a sky");
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutPassiveMotionFunc(mouse);
    glutKeyboardFunc(keyInput);
    glutSpecialFunc(specialKeyInput);

    glewExperimental = GL_TRUE;
    glewInit();

    setup();

    glutMainLoop();
}
