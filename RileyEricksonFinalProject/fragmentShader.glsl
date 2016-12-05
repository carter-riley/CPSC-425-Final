#version 430 core

in vec2 texCoordsExport;

uniform sampler2D grassTex;
uniform sampler2D skyTex;
uniform uint object;

out vec4 colorsOut;

vec4 fieldTexColor, skyTexColor;

void main(void)
{
   fieldTexColor = texture(grassTex, texCoordsExport);
   skyTexColor = texture(skyTex, texCoordsExport);

   if (object == 0) colorsOut = fieldTexColor;
   if (object == 1) colorsOut = skyTexColor;
}
