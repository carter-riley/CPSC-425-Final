#include <cmath>
#include <iostream>

#ifdef __APPLE__
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#  include <OpenGL/glext.h>
#else
#  include <GL/glew.h>
#  include <GL/freeglut.h>
#  include <GL/glext.h>
#pragma comment(lib, "glew32.lib")
#endif

#include "tent.h"

using namespace std;

// Fill the vertex array with co-ordinates of the sample points.

void fillTent(void)
{
	 int k = 0;
	 int x = 10;
	 int z = 0;
	 tentVertices[k].coords.x = x;
	 tentVertices[k].coords.y = 0.0;
	 tentVertices[k].coords.z = z;
	 tentVertices[k].coords.w = 1.0;
	 tentVertices[k].normal.x = 0.0;
	 tentVertices[k].normal.y = 0.0;
	 tentVertices[k].normal.z = 0.0;
	 k++
 	 tentVertices[k].coords.x = x + 50;
	 tentVertices[k].coords.y = 100.0;
	 tentVertices[k].coords.z = z;
	 tentVertices[k].coords.w = 1.0;
	 tentVertices[k].normal.x = 0.0;
	 tentVertices[k].normal.y = 0.0;
	 tentVertices[k].normal.z = 0.0;
	 k++
	 tentVertices[k].coords.x = x + 100;
	 tentVertices[k].coords.y = 0.0;
	 tentVertices[k].coords.z = z;
	 tentVertices[k].coords.w = 1.0;
	 tentVertices[k].normal.x = 0.0;
	 tentVertices[k].normal.y = 0.0;
	 tentVertices[k].normal.z = 0.0;
	 k++
	 tentVertices[k].coords.x = x;
	 tentVertices[k].coords.y = 0.0;
	 tentVertices[k].coords.z = z + 100;
	 tentVertices[k].coords.w = 1.0;
	 tentVertices[k].normal.x = 0.0;
	 tentVertices[k].normal.y = 0.0;
	 tentVertices[k].normal.z = 0.0;
	 k++
	 tentVertices[k].coords.x = x + 50.0;
	 tentVertices[k].coords.y = 100.0;
	 tentVertices[k].coords.z = z + 100.0;
	 tentVertices[k].coords.w = 1.0;
	 tentVertices[k].normal.x = 0.0;
	 tentVertices[k].normal.y = 0.0;
	 tentVertices[k].normal.z = 0.0;
	 k++
	 tentVertices[k].coords.x = x + 100.0;
	 tentVertices[k].coords.y = 0.0;
	 tentVertices[k].coords.z = z + 100.0;
	 tentVertices[k].coords.w = 1.0;
	 tentVertices[k].normal.x = 0.0;
	 tentVertices[k].normal.y = 0.0;
	 tentVertices[k].normal.z = 0.0;

	 tentIndices = {0, 4, 1, 5, 3, 6};
}
