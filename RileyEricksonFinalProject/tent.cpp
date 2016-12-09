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
void fillTentVertexArray(Vertex tentVertices[(TENT_LONGS + 1) * (TENT_LATS + 1)])
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
}

void fillTentIndices(unsigned int tentIndices[TENT_LATS][2 * (TENT_LONGS + 1)])
{
   int i, j;
   for(j = 0; j < TENT_LATS; j++)
   {
      for (i = 0; i <= TENT_LONGS; i++)
      {
	     tentIndices[j][2 * i]     = (j + 1) * (TENT_LONGS + 1) + i;
	     tentIndices[j][2 * i + 1] =    j    * (TENT_LONGS + 1) + i;
      }
   }
}

// Fill the array tentCounts.
void fillTentCounts(int tentCounts[TENT_LATS])
{
   int j;
   for(j = 0; j < TENT_LATS; j++) tentCounts[j] = 2 * (TENT_LONGS + 1);
}

// Fill the array tentOffsets.
void fillTentOffsets(void* tentOffsets[TENT_LATS])
{
   int j;
   for(j = 0; j < TENT_LATS; j++) tentOffsets[j] = (GLvoid*)(2 * (TENT_LONGS + 1) * j * sizeof(unsigned int));
}

void fillTent(Vertex tentVertices[(TENT_LONGS + 1) * (TENT_LATS + 1)],
	         unsigned int tentIndices[TENT_LATS][2 * (TENT_LONGS+1)],
			 int tentCounts[TENT_LATS],
			 void* tentOffsets[TENT_LATS]);

{
   fillTentVertexArray(tentVertices);
   fillTentIndices(tentIndices);
   fillTentCounts(tentCounts);
   fillTentOffsets(tentOffsets);
}
