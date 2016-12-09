#ifndef SHAPES_H
#define SHAPES_H

#include "WireCube.h"
#include "Disk.h"
#include "WireDisk.h"
#include "Cylinder.h"
#include "WireCylinder.h"
#include "WireSteiner.h"
#include "Steiner.h"
#include "Cube.h"

class Shapes
{
public:
    Shapes();
    virtual ~Shapes();

    WireCube myWireCube;
    Cylinder myCylinder;
    WireCylinder myWireCylinder;
    Disk myDisk;
    WireDisk myWireDisk;
    Steiner mySteiner;
    WireSteiner myWireSteiner;
    Cube myCube;

    void createVAO(GLint program);

    void drawCube(vec4 color);
    void drawCylinder(vec4 color);
    void drawDisk(vec4 color);
    void drawSteiner(vec4 color);

protected:
private:
};

#endif // SHAPES_H
