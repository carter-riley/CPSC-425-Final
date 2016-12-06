// A class for storing primitive objects: cylinder, cube, disk, Steiner surface

#include "Shapes.h"
#include "Globals.h"

Shapes::Shapes()
     : myCylinder(36, .5,1),myWireCylinder(36, .5,1),myDisk(18,.5), myWireDisk(18,.5), mySteiner(40,40),myWireSteiner(20,20)
{

}

Shapes::~Shapes()
{
    //dtor
}

void
Shapes::createVAO(GLint program) {

     // Create multiple vertex array objects
    GLuint vao[8];
    glGenVertexArrays( 8, vao );

    myCylinder.createVAO(vao[0],program);
    myWireCylinder.createVAO(vao[1],program);
    myCube.createVAO(vao[2],program);
    myWireCube.createVAO(vao[3],program);
    myDisk.createVAO(vao[4],program);
    myWireDisk.createVAO(vao[5],program);
    mySteiner.createVAO(vao[6],program);
    myWireSteiner.createVAO(vao[7],program);
}
// draws solid cube and wire cube together. The wire color is always black
void
Shapes::drawCube(vec4 color)
{
    glUniform4fv( model_color, 1,color );
    myCube.draw();
    glUniform4fv( model_color, 1,vec4(0,0,0,1) );
    myWireCube.draw();
}

// draws solid Cylinder and wire Cylinder together. The wire color is always black
void
Shapes::drawCylinder(vec4 color)
{
    glUniform4fv( model_color, 1,color );
    myCylinder.draw();
    glUniform4fv( model_color, 1,vec4(0,0,0,1) );
    myWireCylinder.draw();
}

// draws solid Disk and wire Disk together. The wire color is always black
void
Shapes::drawDisk(vec4 color)
{
    glUniform4fv( model_color, 1,color );
    myDisk.draw();
    glUniform4fv( model_color, 1,vec4(0,0,0,1) );
    myWireDisk.draw();
}

void
Shapes::drawSteiner(vec4 color)
{
    glUniform4fv( model_color, 1,color );
    mySteiner.draw();

    glUniform4fv( model_color, 1,vec4(0,0,0,1) );
    myWireSteiner.draw();

}

