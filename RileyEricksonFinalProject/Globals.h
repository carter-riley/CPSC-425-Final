#ifndef SHADER_H
#define SHADER_H

#include "MatrixStack.h"
#include "Shapes.h"

extern GLuint  projection; // projection matrix uniform shader variable location
extern GLuint  model_view;  // model-view matrix uniform shader variable location
extern GLuint  model_color;  // model-view matrix uniform shader variable location
extern GLuint program;  // shader program

extern MatrixStack mvMatrixStack;

extern Shapes shapes;

#endif // SHADER_H
