#ifndef TENT_H
#define TENT_H

#include "vertex.h"

#define PI 3.14159265
#define TENT_LONGS 6 // Number of longitudinal slices.
#define TENT_LATS 6 // Number of latitudinal slices.

void fillTentVertexArray(Vertex tentVertices[(TENT_LONGS + 1) * (TENT_LATS + 1)]);
void fillTentIndices(unsigned int tentIndices[TENT_LATS][2 * (TENT_LONGS + 1)]);
void fillTentCounts(int tentCounts[TENT_LATS]);
void fillTentOffsets(void* ctentOffsets[TENT_LATS]);

void fillTent(Vertex tentVertices[(TENT_LONGS + 1) * (TENT_LATS + 1)],
	         unsigned int tentIndices[TENT_LATS][2 * (TENT_LONGS+1)],
			 int tentCounts[TENT_LATS],
			 void* tentOffsets[TENT_LATS]);

#endif
