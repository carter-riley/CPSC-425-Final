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
#include <time.h>

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

enum object {FIELD, SKY1, SKY2, SKY3, SKY4, CLOUD}; // VAO ids.
enum buffer {FIELD_VERTICES, SKY1_VERTICES, SKY2_VERTICES, SKY3_VERTICES, SKY4_VERTICES, CLOUD_VERTICES }; // VBO ids.

void setupShaders(void);


int width = 800;
int height = 800;

// Globals.
static Vertex fieldVertices[4] =
{
    {vec4(200.0, 0.0, 200.0, 1.0), vec2(10.0, 0.0)},
    {vec4(200.0, 0.0, -200.0, 1.0), vec2(10.0, 10.0)},
    {vec4(-200.0, 0.0, 200.0, 1.0), vec2(0.0, 0.0)},
    {vec4(-200.0, 0.0, -200.0, 1.0), vec2(0.0, 10.0)}
};

//Front skybox image
static Vertex sky1Vertices[4] =
{
    {vec4(200.0, -200.0, -200.0, 1.0), vec2(0.0, 0.0)},
    {vec4(200.0, 200.0, -200.0, 1.0), vec2(0.0, 1.0)},
    {vec4(-200.0, -200.0, -200.0, 1.0), vec2(1.0, 0.0)},
    {vec4(-200.0, 200.0, -200.0, 1.0), vec2(1.0, 1.0)}
};

//Back skybox image
static Vertex sky2Vertices[4] =
{
    {vec4(200.0, -200.0, 200.0, 1.0), vec2(1.0, 0.0)},
    {vec4(200.0, 200.0, 200.0, 1.0), vec2(1.0, 1.0)},
    {vec4(-200.0, -200.0, 200.0, 1.0), vec2(0.0, 0.0)},
    {vec4(-200.0, 200.0, 200.0, 1.0), vec2(0.0, 1.0)}
};
//Right skybox image
static Vertex sky3Vertices[4] =
{
    {vec4(200.0, -200.0, -200.0, 1.0), vec2(1.0, 0.0)},
    {vec4(200.0, 200.0, -200.0, 1.0), vec2(1.0, 1.0)},
    {vec4(200.0, -200.0, 200.0, 1.0), vec2(0.0, 0.0)},
    {vec4(200.0, 200.0, 200.0, 1.0), vec2(0.0, 1.0)}
};
//Right skybox image
static Vertex sky4Vertices[4] =
{
    {vec4(-200.0, -200.0, 200.0, 1.0), vec2(1.0, 0.0)},
    {vec4(-200.0, 200.0, 200.0, 1.0), vec2(1.0, 1.0)},
    {vec4(-200.0, -200.0, -200.0, 1.0), vec2(0.0, 0.0)},
    {vec4(-200.0, 200.0, -200.0, 1.0), vec2(0.0, 1.0)}
};
//Top skybox image
static Vertex cloudVertices[4] =
{
    {vec4(200.0, 200.0, 200.0, 1.0), vec2(1.0, 0.0)},
    {vec4(200.0, 200.0, -200.0, 1.0), vec2(1.0, 1.0)},
    {vec4(-200.0, 200.0, 200.0, 1.0), vec2(0.0, 0.0)},
    {vec4(-200.0, 200.0, -200.0, 1.0), vec2(0.0, 1.0)}
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
sky1TexLoc,
sky2TexLoc,
sky3TexLoc,
sky4TexLoc,
cloudTexLoc,
objectLoc,
buffer[6],
vao[6],
texture[6];

static BitMapFile *image[6]; // Local storage for bmp image data.

void drawSky(void);

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
    glBindVertexArray(vao[FIELD]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[FIELD_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fieldVertices), fieldVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(fieldVertices[0]), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(fieldVertices[0]), (void*)(sizeof(fieldVertices[0].coords)));
    glEnableVertexAttribArray(1);

    // ...and associate data with vertex shader.
    glBindVertexArray(vao[SKY1]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[SKY1_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sky1Vertices), sky1Vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(sky1Vertices[0]), 0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(sky1Vertices[0]), (void*)(sizeof(sky1Vertices[0].coords)));
    glEnableVertexAttribArray(3);

    // ...and associate data with vertex shader.
    glBindVertexArray(vao[SKY2]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[SKY2_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sky2Vertices), sky2Vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(sky2Vertices[0]), 0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(sky2Vertices[0]), (void*)(sizeof(sky2Vertices[0].coords)));
    glEnableVertexAttribArray(5);

    // ...and associate data with vertex shader.
    glBindVertexArray(vao[SKY3]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[SKY3_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sky3Vertices), sky3Vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(sky3Vertices[0]), 0);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(sky3Vertices[0]), (void*)(sizeof(sky3Vertices[0].coords)));
    glEnableVertexAttribArray(7);

    // ...and associate data with vertex shader.
    glBindVertexArray(vao[SKY4]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[SKY4_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sky4Vertices), sky4Vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(sky4Vertices[0]), 0);
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, sizeof(sky4Vertices[0]), (void*)(sizeof(sky4Vertices[0].coords)));
    glEnableVertexAttribArray(9);

    // ...and associate data with vertex shader.
    glBindVertexArray(vao[CLOUD]);
    glBindBuffer(GL_ARRAY_BUFFER, buffer[CLOUD_VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cloudVertices), cloudVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(cloudVertices[0]), 0);
    glEnableVertexAttribArray(10);
    glVertexAttribPointer(11, 2, GL_FLOAT, GL_FALSE, sizeof(cloudVertices[0]), (void*)(sizeof(cloudVertices[0].coords)));
    glEnableVertexAttribArray(11);

    // Obtain projection matrix uniform location and set value.
    projMatLoc = glGetUniformLocation(programId,"projMat");
    projMat = frustum(-5.0, 5.0, -5.0, 5.0, 5.0, 2000.0);
    glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));

    // Obtain modelview matrix uniform and object uniform locations.
    modelViewMatLoc = glGetUniformLocation(programId,"modelViewMat");
    objectLoc = glGetUniformLocation(programId, "object");

    // Load the images.
    image[0] = getbmp("grass1.bmp");
    image[1] = getbmp("front.bmp");
    image[2] = getbmp("back.bmp");
    image[3] = getbmp("right.bmp");
    image[4] = getbmp("left.bmp");
    image[5] = getbmp("top.bmp");


    // Create texture ids.
    glGenTextures(6, texture);

    // Bind grass image.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[0]->sizeX, image[0]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[0]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


    // Filter using mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    grassTexLoc = glGetUniformLocation(programId, "grassTex");
    glUniform1i(grassTexLoc, 0);

    // Bind sky1 image.
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[1]->sizeX, image[1]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[1]->data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    sky1TexLoc = glGetUniformLocation(programId, "sky1Tex");
    glUniform1i(sky1TexLoc, 1);

    // Bind sky2 image.
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[2]->sizeX, image[2]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[2]->data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    sky2TexLoc = glGetUniformLocation(programId, "sky2Tex");
    glUniform1i(sky2TexLoc, 2);

    // Bind sky3 image.
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[3]->sizeX, image[3]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[3]->data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    sky3TexLoc = glGetUniformLocation(programId, "sky3Tex");
    glUniform1i(sky3TexLoc, 3);

    // Bind sky4 image.
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[4]->sizeX, image[4]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[4]->data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    sky4TexLoc = glGetUniformLocation(programId, "sky4Tex");
    glUniform1i(sky4TexLoc, 4);

    // Bind cloud image.
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[5]->sizeX, image[5]->sizeY, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image[5]->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    cloudTexLoc = glGetUniformLocation(programId, "cloudTex");
    glUniform1i(cloudTexLoc, 5);

    printControls();
}



// Drawing routine.
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate and update modelview matrix.
    modelViewMat = mat4(1.0);
    //modelViewMat = lookAt(vec3(posX - 10 * sin( (PI / 180.0)), 10.0, posZ + 5 * cos( (PI / 180.0))), vec3(lookX * 0.1, (-lookY * 0.1) + 10, 0.0), vec3(0.0, 1.0, 0.0));
    modelViewMat = lookAt(vec3(xVal - 10 * sin( (PI / 180.0) * vertexAngle),10.0,zVal - 10 * cos( (PI / 180.0) * vertexAngle)), vec3(xVal - 11 * sin( (PI / 180.0) * vertexAngle), 10.0 + -lookY*0.01,zVal - 11 * cos( (PI / 180.0) * vertexAngle)),vec3(0.0, 1.0,0.0));

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

    GLfloat angle = 90.0;

    drawSky();
//    // Draw sky.
//    glUniform1ui(objectLoc, SKY);
//    glBindVertexArray(vao[SKY]);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//
//    glUniform1ui(objectLoc, CLOUD);
//    glBindVertexArray(vao[CLOUD]);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);


//    modelViewMat = translate(modelViewMat, vec3(0.0, 0.0, 400.0));
//    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));
//    glUniform1ui(objectLoc, SKY);
//    glBindVertexArray(vao[SKY]);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//
//    modelViewMat = translate(modelViewMat, vec3(200.0, 0.0, 0.0));
//    modelViewMat = translate(modelViewMat, vec3(-200.0, 0.0, 0.0));
//    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));
//    glUniform1ui(objectLoc, SKY);
//    glBindVertexArray(vao[SKY]);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//
//    modelViewMat = rotate(modelViewMat, float(PI /2), vec3(0.0, 1.0, 0.0));
//    modelViewMat = translate(modelViewMat, vec3(0.0, 0.0, 400.0));
//    modelViewMat = translate(modelViewMat, vec3(400.0, 0.0, 0.0));
//
//    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));
//    glUniform1ui(objectLoc, SKY);
//    glBindVertexArray(vao[SKY]);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//
//    modelViewMat = translate(modelViewMat, vec3(0.0, 0.0, 0.0));
//    modelViewMat = translate(modelViewMat, vec3(0.0, 0.0, -400.0));
//    glUniformMatrix4fv(modelViewMatLoc, 1, GL_FALSE, value_ptr(modelViewMat));
//
//    glUniform1ui(objectLoc, SKY);
//    glBindVertexArray(vao[SKY]);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glutSwapBuffers();
}

void drawSky(void)
{
    glUniform1ui(objectLoc, SKY1);
    glBindVertexArray(vao[SKY1]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glUniform1ui(objectLoc, SKY2);
    glBindVertexArray(vao[SKY2]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glUniform1ui(objectLoc, SKY3);
    glBindVertexArray(vao[SKY3]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glUniform1ui(objectLoc, SKY4);
    glBindVertexArray(vao[SKY4]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glUniform1ui(objectLoc, CLOUD);
    glBindVertexArray(vao[CLOUD]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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

    xVal = tempxVal;
    zVal = tempzVal;
    vertexAngle = tempvertexAngle;
    glutPostRedisplay();
}

void mouse( GLint x, GLint y )
{
    float tempxVal = xVal, tempzVal = zVal, tempvertexAngle = vertexAngle;
    //glutWarpPointer(width /2 ,height /2); //To warp the cursor to a screen position
    //glutSetCursor(GLUT_CURSOR_NONE); //To hide the cursor
    //cout << "x = " << x << ", y = " << y << endl;

    if(x > width/2)
        tempvertexAngle = vertexAngle - 1.0;
    if(x < width/2)
        tempvertexAngle = vertexAngle + 1.0;
    if(y > width/2)
        lookY++;
    if(y < width/2)
        lookY--;
    if (tempvertexAngle > 360.0) tempvertexAngle -= 360.0;
    if (tempvertexAngle < 0.0) tempvertexAngle += 360.0;

    xVal = tempxVal;
    zVal = tempzVal;
    vertexAngle = tempvertexAngle;

    /*
    lookX = x - (width / 2);
    lookY = y - (height / 2);
    //cout << "lookX = " << lookX << ", lookY = " << lookY << endl;
    */
    glutPostRedisplay();
}

void idle()
{
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
    glutIdleFunc(idle);
    glewExperimental = GL_TRUE;
    glewInit();

    setup();

    glutMainLoop();
}
