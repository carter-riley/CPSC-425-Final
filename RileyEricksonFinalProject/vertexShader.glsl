#version 430 core


layout(location=0) in vec4 fieldCoords;
layout(location=1) in vec2 fieldTexCoords;
layout(location=2) in vec4 sky1Coords;
layout(location=3) in vec2 sky1TexCoords;
layout(location=4) in vec4 sky2Coords;
layout(location=5) in vec2 sky2TexCoords;
layout(location=6) in vec4 sky3Coords;
layout(location=7) in vec2 sky3TexCoords;
layout(location=8) in vec4 sky4Coords;
layout(location=9) in vec2 sky4TexCoords;
layout(location=10) in vec4 cloudCoords;
layout(location=11) in vec2 cloudTexCoords;


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
        coords = sky1Coords;
        texCoordsExport = sky1TexCoords;
    }
    if (object == 2)
    {
        coords = sky2Coords;
        texCoordsExport = sky2TexCoords;
    }
    if (object == 3)
    {
        coords = sky3Coords;
        texCoordsExport = sky3TexCoords;
    }
    if (object == 4)
    {
        coords = sky4Coords;
        texCoordsExport = sky4TexCoords;
    }
    if  (object == 5)
    {
        coords = cloudCoords;
        texCoordsExport = cloudTexCoords;
    }

    gl_Position = projMat * modelViewMat * coords;
}
