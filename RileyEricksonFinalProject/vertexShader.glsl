#version 430 core


layout(location=0) in vec4 fieldCoords;
layout(location=1) in vec2 fieldTexCoords;
layout(location=2) in vec4 skyCoords;
layout(location=3) in vec2 skyTexCoords;
layout(location=4) in vec4 cloudCoords;
layout(location=5) in vec2 cloudTexCoords;


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
   if  (object == 2)
   {
      coords = cloudCoords;
      texCoordsExport = cloudTexCoords;
   }

   gl_Position = projMat * modelViewMat * coords;
}
