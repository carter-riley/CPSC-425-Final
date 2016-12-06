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
using namespace std;

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

enum object {FIELD, SKY_F, SKY_B, SKY_R, SKY_L, SKY_T}; // VAO ids.
enum buffer {FIELD_VERTICES, SKY_VERTICES_F, SKY_VERTICES_B, SKY_VERTICES_R, SKY_VERTICES_L, SKY_VERTICES_T }; // VBO ids.

void setupShaders(void);


int width = 800;
int height = 800;

// Globals.
static Vertex fieldVertices[4] =
{
    {vec4(100.0, 0.0, 100.0, 1.0), vec2(40.0, 0.0)},
    {vec4(100.0, 0.0, -100.0, 1.0), vec2(40.0, 40.0)},
    {vec4(-100.0, 0.0, 100.0, 1.0), vec2(0.0, 0.0)},
    {vec4(-100.0, 0.0, -100.0, 1.0), vec2(0.0, 40.0)}
<<<<<<< HEAD
=======
};


static Vertex skyVerticesF[4] =
{
    {vec4(100.0, 0.0, -100.0, 1.0), vec2(1.0, 0.0)},
    {vec4(100.0, 120.0, -100.0, 1.0), vec2(1.0, 1.0)},
    {vec4(-100.0, 0.0, -100.0, 1.0), vec2(0.0, 0.0)},
    {vec4(-100.0, 120.0, -100.0, 1.0), vec2(0.0, 1.0)}
};

static Vertex skyVerticesL[4] =
{
    {vec4(-100.0, 0.0, -100.0, 1.0), vec2(1.0, 0.0)},
    {vec4(-100.0, 120.0, -100.0, 1.0), vec2(1.0, 1.0)},
    {vec4(-100.0, 0.0, 100.0, 1.0), vec2(0.0, 0.0)},
    {vec4(-100.0, 120.0, 100.0, 1.0), vec2(0.0, 1.0)}
};

static Vertex skyVerticesB[4] =
{
    {vec4(-100.0, 0.0, 100.0, 1.0), vec2(1.0, 0.0)},
    {vec4(-100.0, 120.0, 100.0, 1.0), vec2(1.0, 1.0)},
    {vec4(100.0, 0.0, 100.0, 1.0), vec2(0.0, 0.0)},
    {vec4(100.0, 120.0, 100.0, 1.0), vec2(0.0, 1.0)}
};

static Vertex skyVerticesR[4] =
{
    {vec4(100.0, 0.0, 100.0, 1.0), vec2(1.0, 0.0)},
    {vec4(100.0, 120.0, 100.0, 1.0), vec2(1.0, 1.0)},
    {vec4(100.0, 0.0, -100.0, 1.0), vec2(0.0, 0.0)},
    {vec4(100.0, 120.0, -100.0, 1.0), vec2(0.0, 1.0)}
>>>>>>> refs/remotes/origin/master
};

static Vertex skyVerticesT[4] =
{
<<<<<<< HEAD
    {vec4(100.0, -100.0, -100.0, 1.0), vec2(1.0, 0.0)},
    {vec4(100.0, 100.0, -100.0, 1.0), vec2(1.0, 1.0)},
    {vec4(-100.0, -100.0, -100.0, 1.0), vec2(0.0, 0.0)},
    {vec4(-100.0, 100.0, -100.0, 1.0), vec2(0.0, 1.0)}
=======
    {vec4(100.0, 120.0, 100.0, 1.0), vec2(1.0, 0.0)},
    {vec4(100.0, 120.0, -100.0, 1.0), vec2(1.0, 1.0)},
    {vec4(-100.0, 120.0, 100.0, 1.0), vec2(0.0, 0.0)},
    {vec4(-100.0, 120.0, -100.0, 1.0), vec2(0.0, 1.0)}
>>>>>>> refs/remotes/origin/master
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
skyTexLoc,
<<<<<<< HEAD
sky1TexLoc,
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
=======
objectLoc,
buffer[6],
vao[6],
texture[2];

static BitMapFile *image[6]; // Local storage for bmp image data.

static float d = 0.0; // Distance parameter in gluLookAt().

>>>>>>> refs/remotes/origin/master

// Initialization routine.
void setup(void)
{
<<<<<<< HEAD
    glClearColor(0.0, 0.0, 0.0, 0.0);
=======
    glClearColor(1.0, 1.0, 1.0, 0.0);
>>>>>>> refs/remotes/origin/master
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
<<<<<<< HEAD
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
=======
    glGenVertexArrays(6, vao);
    glGenBuffers(6, buffer);

    // ...and associate data with vertex shader.
    setupShaders();
>>>>>>> refs/remotes/origin/master

    // Obtain projection matrix uniform location and set value.
    projMatLoc = glGetUniformLocation(programId,"projMat");
    projMat = frustum(-5.0, 5.0, -5.0, 5.0, 5.0, 1000.0);
    glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));

    // Obtain modelview matrix uniform and object uniform locations.
    modelViewMatLoc = glGetUniformLocation(programId,"modelViewMat");
    objectLoc = glGetUniformLocation(programId, "object");

    // Load the images.
    image[0] = getbmp("grass1.bmp");
<<<<<<< HEAD
    image[1] = getbmp("front.bmp");
    image[2] = getbmp("back.bmp");
    image[3] = getbmp("left.bmp");
    image[4] = getbmp("right.bmp");
    image[5] = getbmp("top.bmp");
    image[6] = getbmp("bottom.bmp");
    // Create texture ids.
    glGenTextures(6, texture);
=======
    image[1] = getbmp("sky1.bmp");

    // Create texture ids.
    glGenTextures(2, texture);
>>>>>>> refs/remotes/origin/master

    // Bind grass image.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->sizeX, image[0]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

<<<<<<< HEAD
    // Use mipmap texture filtering.
=======
    grassTexLoc = glGetUniformLocation(programId, "grassTex");
    glUniform1i(grassTexLoc, 0);

    // Filter using mipmap
>>>>>>> refs/remotes/origin/master
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

<<<<<<< HEAD
    grassTexLoc = glGetUniformLocation(programId, "grassTex");
    glUniform1i(grassTexLoc, 0);

=======
>>>>>>> refs/remotes/origin/master
    // Bind sky image.
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->sizeX, image[1]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
<<<<<<< HEAD
    skyTexLoc = glGetUniformLocation(programId, "skyTex");
    glUniform1i(skyTexLoc, 1);

    // Bind sky image.
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[2]->sizeX, image[2]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[2]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    sky1TexLoc = glGetUniformLocation(programId, "sky1Tex");
    glUniform1i(sky1TexLoc, 2);
    printControls();
=======


    skyTexLoc = glGetUniformLocation(programId, "skyTex");
    glUniform1i(skyTexLoc, 1);
>>>>>>> refs/remotes/origin/master
}

// Drawing routine.
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate and update modelview matrix.
<<<<<<< HEAD
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
=======
   modelViewMat = mat4(1.0);
   modelViewMat = lookAt(vec3(posX,  10, posZ + 15), vec3(lookX * 0.1, (-lookY * 0.1) + 10, 0.0), vec3(0.0, 1.0, 0.0));

   /*LookAt(
            vec3(xVal - 10 * sin( (PI / 180.0) * angle), 0.0, zVal - 10 * cos( (PI / 180.0) * angle)),
            vec3( xVal - 11 * sin( (PI / 180.0) * angle), 0.0, zVal - 11 * cos( (PI / 180.0) * angle)),
            vec3(0.0, 1.0, 0.0);

   */

>>>>>>> refs/remotes/origin/master
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));

    // Draw field.
    glUniform1ui(objectLoc, FIELD);
    glBindVertexArray(vao[FIELD]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

<<<<<<< HEAD
    GLfloat angle = 90.0;
    // Draw sky.

    glUniform1ui(objectLoc, SKY);
    glBindVertexArray(vao[SKY]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    modelViewMat = translate(modelViewMat, vec3(0.0, 0.0, 200.0));
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));
    glUniform1ui(objectLoc, SKY);
    glBindVertexArray(vao[SKY]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    modelViewMat = translate(modelViewMat, vec3(100.0, 0.0, 0.0));
    modelViewMat = rotate(modelViewMat, angle, vec3(0.0, 1.0, 0.0));
    modelViewMat = translate(modelViewMat, vec3(-100.0, 0.0, 0.0));
    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));
    glUniform1ui(objectLoc, SKY);
    glBindVertexArray(vao[SKY]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

//    modelViewMat = translate(modelViewMat, vec3(100.0, 0.0, 0.0));
//    //modelViewMat = rotate(modelViewMat, angle, vec3(0.0, 1.0, 0.0));
//    modelViewMat = translate(modelViewMat, vec3(0.0, 0.0, 100.0));
//    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));
//    glUniform1ui(objectLoc, SKY);
//    glBindVertexArray(vao[SKY]);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glutSwapBuffers();
=======

    // Draw sky.

    // Front
    glUniform1ui(objectLoc, SKY_F);
    glBindVertexArray(vao[SKY_F]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Back
    glUniform1ui(objectLoc, SKY_B);
    glBindVertexArray(vao[SKY_B]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Right
    glUniform1ui(objectLoc, SKY_R);
    glBindVertexArray(vao[SKY_R]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Left
    glUniform1ui(objectLoc, SKY_L);
    glBindVertexArray(vao[SKY_L]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Top
    glUniform1ui(objectLoc, SKY_T);
    glBindVertexArray(vao[SKY_T]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glutSwapBuffers();
}

void setupShaders(void)
{
    glBindVertexArray(vao[FIELD]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[FIELD_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fieldVertices), fieldVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(fieldVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(fieldVertices[0]), (void*)(sizeof(fieldVertices[0].coords)));
    glEnableVertexAttribArray(1);

    // Front of skybox
    glBindVertexArray(vao[SKY_F]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[SKY_VERTICES_F]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVerticesF), skyVerticesF, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(skyVerticesF[0]), 0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(skyVerticesF[0]), (void*)(sizeof(skyVerticesF[0].coords)));
    glEnableVertexAttribArray(3);

    // Back of skybox
    glBindVertexArray(vao[SKY_B]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[SKY_VERTICES_B]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVerticesB), skyVerticesB, GL_STATIC_DRAW);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(skyVerticesB[0]), 0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(skyVerticesB[0]), (void*)(sizeof(skyVerticesB[0].coords)));
    glEnableVertexAttribArray(5);

    // Right of skybox
    glBindVertexArray(vao[SKY_R]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[SKY_VERTICES_R]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVerticesR), skyVerticesR, GL_STATIC_DRAW);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(skyVerticesR[0]), 0);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(skyVerticesR[0]), (void*)(sizeof(skyVerticesR[0].coords)));
    glEnableVertexAttribArray(7);

    // Left of skybox
    glBindVertexArray(vao[SKY_L]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[SKY_VERTICES_L]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVerticesL), skyVerticesL, GL_STATIC_DRAW);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(skyVerticesL[0]), 0);
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, sizeof(skyVerticesL[0]), (void*)(sizeof(skyVerticesL[0].coords)));
    glEnableVertexAttribArray(9);

    // Top of skybox
    glBindVertexArray(vao[SKY_T]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[SKY_VERTICES_T]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVerticesT), skyVerticesT, GL_STATIC_DRAW);
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(skyVerticesT[0]), 0);
    glEnableVertexAttribArray(10);
    glVertexAttribPointer(11, 2, GL_FLOAT, GL_FALSE, sizeof(skyVerticesT[0]), (void*)(sizeof(skyVerticesT[0].coords)));
    glEnableVertexAttribArray(11);
>>>>>>> refs/remotes/origin/master
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

<<<<<<< HEAD
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

=======
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
   glutInitWindowSize(width, height);
   glutInitWindowPosition(0,0);
   glutCreateWindow("a field and a sky");
   glutDisplayFunc(drawScene);
   glutReshapeFunc(resize);
   glutMouseFunc(mouse);
   glutKeyboardFunc(keyInput);
   glutSpecialFunc(specialKeyInput);
    glewExperimental = GL_TRUE;
    glewInit();

>>>>>>> refs/remotes/origin/master
    setup();

    glutMainLoop();
}
