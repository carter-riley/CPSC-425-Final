#version 430 core


layout(location=0) in vec4 fieldCoords;
layout(location=1) in vec2 fieldTexCoords;
layout(location=2) in vec4 skyCoords;
layout(location=3) in vec2 skyTexCoords;

uniform mat4 modelViewMat;
uniform mat4 projMat;
uniform uint object;

out vec2 texCoordsExport;

vec4 coords;

void main(void)
{
   if (object == 0)
   {
      coords = fieldCoords;
      texCoordsExport = fieldTexCoords;
   }
   if (object == 1)
   {
      coords = skyCoords;
      texCoordsExport = skyTexCoords;
   }

   gl_Position = projMat * modelViewMat * coords;
}
