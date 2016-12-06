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

using namespace std;
using namespace glm;

enum object {FIELD, SKY_F, SKY_B, SKY_R, SKY_L, SKY_T}; // VAO ids.
enum buffer {FIELD_VERTICES, SKY_VERTICES_F, SKY_VERTICES_B, SKY_VERTICES_R, SKY_VERTICES_L, SKY_VERTICES_T }; // VBO ids.

void drawSky(void);
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
};


static Vertex skyVerticesF[4] =
{
    {vec4(100.0, 0.0, -70.0, 1.0), vec2(1.0, 0.0)},
    {vec4(100.0, 120.0, -70.0, 1.0), vec2(1.0, 1.0)},
    {vec4(-100.0, 0.0, -70.0, 1.0), vec2(0.0, 0.0)},
    {vec4(-100.0, 120.0, -70.0, 1.0), vec2(0.0, 1.0)}
};

static Vertex skyVerticesB[4] =
{
    {vec4(-100.0, 0.0, 70.0, 1.0), vec2(1.0, 0.0)},
    {vec4(-100.0, 120.0, 70.0, 1.0), vec2(1.0, 1.0)},
    {vec4(100.0, 0.0, 70.0, 1.0), vec2(0.0, 0.0)},
    {vec4(100.0, 120.0, 70.0, 1.0), vec2(0.0, 1.0)}
};

static Vertex skyVerticesR[4] =
{
    {vec4(100.0, 0.0, 70.0, 1.0), vec2(1.0, 0.0)},
    {vec4(100.0, 120.0, 70.0, 1.0), vec2(1.0, 1.0)},
    {vec4(100.0, 0.0, -70.0, 1.0), vec2(0.0, 0.0)},
    {vec4(100.0, 120.0, -70.0, 1.0), vec2(0.0, 1.0)}
};

static Vertex skyVerticesL[4] =
{
    {vec4(-100.0, 0.0, -70.0, 1.0), vec2(1.0, 0.0)},
    {vec4(-100.0, 120.0, -70.0, 1.0), vec2(1.0, 1.0)},
    {vec4(-100.0, 0.0, 70.0, 1.0), vec2(0.0, 0.0)},
    {vec4(-100.0, 120.0, 70.0, 1.0), vec2(0.0, 1.0)}
};

static Vertex skyVerticesT[4] =
{
    {vec4(100.0, 120.0, 70.0, 1.0), vec2(1.0, 0.0)},
    {vec4(100.0, 120.0, -70.0, 1.0), vec2(1.0, 1.0)},
    {vec4(-100.0, 120.0, 70.0, 1.0), vec2(0.0, 0.0)},
    {vec4(-100.0, 120.0, -70.0, 1.0), vec2(0.0, 1.0)}
};


static mat4 modelViewMat = mat4(1.0);
static mat4 projMat = mat4(1.0);

int lookX = 0, lookY = 0;
int posX = 0, posZ = 0;

static unsigned int
programId,
vertexShaderId,
fragmentShaderId,
modelViewMatLoc,
projMatLoc,
grassTexLoc,
skyTexLoc,
objectLoc,
buffer[6],
vao[6],
texture[2];

static BitMapFile *image[6]; // Local storage for bmp image data.

static float d = 0.0; // Distance parameter in gluLookAt().


// Initialization routine.
void setup(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);
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
    glGenVertexArrays(6, vao);
    glGenBuffers(6, buffer);

    // ...and associate data with vertex shader.
    setupShaders();

    // Obtain projection matrix uniform location and set value.
    projMatLoc = glGetUniformLocation(programId,"projMat");
    projMat = frustum(-5.0, 5.0, -5.0, 5.0, 5.0, 1000.0);
    glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));

    // Obtain modelview matrix uniform and object uniform locations.
    modelViewMatLoc = glGetUniformLocation(programId,"modelViewMat");
    objectLoc = glGetUniformLocation(programId, "object");

    // Load the images.
    image[0] = getbmp("grass1.bmp");
    image[1] = getbmp("sky1.bmp");

    // Create texture ids.
    glGenTextures(2, texture);

    // Bind grass image.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->sizeX, image[0]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    grassTexLoc = glGetUniformLocation(programId, "grassTex");
    glUniform1i(grassTexLoc, 0);

    // Filter using mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Bind sky image.
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->sizeX, image[1]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    skyTexLoc = glGetUniformLocation(programId, "skyTex");
    glUniform1i(skyTexLoc, 1);
}

// Drawing routine.
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate and update modelview matrix.
   modelViewMat = mat4(1.0);
   modelViewMat = lookAt(vec3(posX,  10, posZ + 15), vec3(lookX * 0.1, (-lookY * 0.1) + 10, 0.0), vec3(0.0, 1.0, 0.0));

   /*LookAt(
            vec3(xVal - 10 * sin( (PI / 180.0) * angle), 0.0, zVal - 10 * cos( (PI / 180.0) * angle)),
            vec3( xVal - 11 * sin( (PI / 180.0) * angle), 0.0, zVal - 11 * cos( (PI / 180.0) * angle)),
            vec3(0.0, 1.0, 0.0);

   */

    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));

    // Draw field.
    glUniform1ui(objectLoc, FIELD);
    glBindVertexArray(vao[FIELD]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


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

}

void drawSky(void)
{


}
// OpenGL window reshape routine.
void resize(int w, int h)
{
    glViewport(0, 0, w, h);
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
   switch(key)
   {
      case 'w':
        posZ -= 1;
        break;
      case 'a':
        posX -= 1;
        break;
      case 's':
        posZ += 1;
        break;
      case 'd':
        posX += 1;
        break;
      case 27:
         exit(0);
         break;
      default:
         break;
   }
   glutPostRedisplay();
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
    glutPostRedisplay();
}

void mouse( GLint button, GLint state, GLint x, GLint y )
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

    setup();

    glutMainLoop();
}
