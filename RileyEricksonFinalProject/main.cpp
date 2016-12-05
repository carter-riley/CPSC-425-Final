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
    projMat = frustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);
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
    modelViewMat = lookAt(vec3(0.0, 10.0, 15.0 + d), vec3(0.0, 10.0, 0.0), vec3(0.0, 1.0, 0.0));
    /*(xVal - 10 * sin( (PI / 180.0) * angle),
               0.0,
               zVal - 10 * cos( (PI / 180.0) * angle),
               xVal - 11 * sin( (PI / 180.0) * angle),
               0.0,
               zVal - 11 * cos( (PI / 180.0) * angle),
               0.0,
               1.0,
               0.0);
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
    case 27:
        exit(0);
        break;
    default:
        break;
    }
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
    if (key == GLUT_KEY_UP)
    {
        if (d > -50.0) d -= 0.1;
    }
    if (key == GLUT_KEY_DOWN)
    {
        if (d < 15.0) d += 0.1;
    }
    glutPostRedisplay();

/*
    void specialKeyInput(int key, int x, int y)
    {
        float tempxVal = xVal, tempzVal = zVal, tempAngle = angle;

        // Compute next position.
        if (key == GLUT_KEY_LEFT) tempAngle = angle + 5.0;
        if (key == GLUT_KEY_RIGHT) tempAngle = angle - 5.0;
        if( key == GLUT_KEY_UP)
        {
            tempxVal = xVal - sin(angle * PI / 180.0);
            tempzVal = zVal - cos(angle * PI / 180.0);
        }
        if( key == GLUT_KEY_DOWN)
        {
            tempxVal = xVal + sin(angle * PI / 180.0);
            tempzVal = zVal + cos(angle * PI / 180.0);
        }

        // Angle correction.
        if (tempAngle > 360.0) tempAngle -= 360.0;
        if (tempAngle < 0.0) tempAngle += 360.0;

        // Move spacecraft to next position only if there will not be collision with an asteroid.
        if (!asteroidCraftCollision(tempxVal, tempzVal, tempAngle, false) )
        {
            isCollision = 0;
            xVal = tempxVal;
            zVal = tempzVal;
            angle = tempAngle;
        }
        else isCollision = true;

        glutPostRedisplay();

    }
}
*/
/*
void
mouse( GLint button, GLint state, GLint x, GLint y )
{
    static GLint buttons_down = 0;

    if (state == GLUT_DOWN)
    {
        // cout << "mouse: button = " << button << "  state = " << state << "  x,y = " << x << "," << y << "\n";

        switch (button)
        {
        case GLUT_LEFT_BUTTON:
            // cout << "     mouse: GLUT_LEFT_BUTTON - TUMBLE\n";
            action = TUMBLE;
            xStart = x;
            yStart = y;
            break;
        case GLUT_MIDDLE_BUTTON:
            //  cout << "     mouse: GLUT_MIDDLE_BUTTON - DOLLY\n";
            action = DOLLY;
            xStart = x;
            yStart = y;
            break;
        case GLUT_RIGHT_BUTTON:
            //  cout << "     mouse: GLUT_RIGHT_BUTTON - TRACK\n";
            action = TRACK;
            xStart = x;
            yStart = y;
            break;
        }
    }
    if (state == GLUT_UP)
    {
        // printControls();
    }
}

//---------------------------------------------------------------------------- tumble
// tumble about to location tumblePoint in World Coordinate System.
void tumble(mat4 rx, mat4 ry, vec4 tumblePoint)
{
    // We want to rotate about the world coordinate system along a direction parallel to the
    // camera's x axis. We first determine the coordinates of the WCS origin expressed in the eye coordinates.
    // We then translate this point to the camera (origin in camera coordinates) and do a rotation about x.
    // We then translate back. The result is then composed with the view matrix to give a new view matrix.
   //  NEED TO IMPLEMENT !!!   When done, should have new value for eye, u, v, n

   // DO THIS CONTROL LAST - IT IS THE MOST DIFFICULT PART

   mat4 view  =  viewRotation * Translate( -eye );  // current view matrix

   // X Rotate about tumble point in Camera Coord Sys
   vec4 loc = view * tumblePoint;  // location of tumble point in cam coord
   mat4 t2 = Translate(loc[0],loc[1],loc[2]);
   mat4 t1 = Translate(-loc[0],-loc[1],-loc[2]);
   mat4 xtransform = t2 * rx * t1;

   // Y Rotate about tumble point in WCS
    t2 = Translate(tumblePoint[0],tumblePoint[1],tumblePoint[2]);
    t1 = Translate(-tumblePoint[0],-tumblePoint[1],-tumblePoint[2]);
    mat4 ytransform = t2 * ry * t1;

    mat4 newView = xtransform * view * ytransform;

    // need to get eye position back
    //  Here, rotInverse is the inverse of the rotational part of the view matrix.
    //  eye = -rotInverse*view*origin  -> this gives the location of the WCS origin in the eye coordinates
    mat4 rotInverse = transpose(newView);
    rotInverse[3][0] = 0;
    rotInverse[3][1] = 0;
    rotInverse[3][2] = 0; // zero out translational part
    rotInverse[3][3] = 1;
    eye = -(rotInverse * newView * vec4(0,0,0,1));

    viewRotation = newView;
    viewRileyEricksonFinalProject/bin/Debug/RileyEricksonFinalProject.exewRotation[0][3] = 0;
    viewRotation[1][3] = 0;
    viewRotation[2][3] = 0;
    viewRotation[3][3] = 1;
}

//---------------------------------------------------------------------------- motion
void
motion( GLint x, GLint y )
{
    float dx, dy;
    mat4 ry, rx;
    vec4 tumblePoint;

    switch (action)
    {
    case TUMBLE:
        dx = 0.05*(x - xStart);
        dy = 0.05*(y - yStart);

        ry = RotateY(10 * dx);
        rx = RotateX(10 * dy);

        // tumble about a point tumblePoint in WCS. Two options currently.
        if (t == 0)   // tumble about origin in  WCS
        {
            tumblePoint =  vec4(0,0,0,1);
        }
        else        // tumble about fixed distance in front of camera in  WCS
        {
            tumblePoint =   eye - d*viewRotation[2];
            tumblePoint[3] = 1;
        }
        tumble(rx, ry, tumblePoint);   //  <----  NEED TO IMPLEMENT this method!!!

        xStart = x;
        yStart = y;
        break;
    case TRACK:  // PAN
        dx = -0.05 *
        (x - xStart);
        dy = 0.05*(y - yStart);
         //  NEED TO IMPLEMENT !!!
        eye = eye + dx*viewRotation[0] + dy*viewRotation[1];
        eye[3]=1;
        xStart = x;
        yStart = y;
        break;
    case DOLLY:
        dy = 0.05*(y - yStart);
        dx = 0.05*(x - xStart);
         //  NEED TO IMPLEMENT !!!
         eye = eye - (dy+dx)*viewRotation[2];
        eye[3]=1;
        xStart = x;
        yStart = y;
        break;
    default:
        printf("unknown action %d\n", action);
    }
*/
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
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("a field and a sky");
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutSpecialFunc(specialKeyInput);

    glewExperimental = GL_TRUE;
    glewInit();

    setup();

    glutMainLoop();
}

