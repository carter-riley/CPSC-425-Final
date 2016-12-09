#version 430 core

in vec2 texCoordsExport;
in vec4 colsExport;

uniform sampler2D grassTex;
uniform sampler2D sky1Tex;
uniform sampler2D sky2Tex;
uniform sampler2D sky3Tex;
uniform sampler2D sky4Tex;
uniform sampler2D cloudTex;
uniform uint object;

out vec4 colorsOut;

vec4 fieldTexColor, sky1TexColor, sky2TexColor, sky3TexColor, sky4TexColor, cloudTexColor;

void main(void)
{
    fieldTexColor = texture(grassTex, texCoordsExport);
    sky1TexColor = texture(sky1Tex, texCoordsExport);
    sky2TexColor = texture(sky2Tex, texCoordsExport);
    sky3TexColor = texture(sky3Tex, texCoordsExport);
    sky4TexColor = texture(sky4Tex, texCoordsExport);
    cloudTexColor = texture(cloudTex, texCoordsExport);

    if (object == 0) colorsOut = fieldTexColor;
    if (object == 1) colorsOut = sky1TexColor;
    if (object == 2) colorsOut = sky2TexColor;
    if (object == 3) colorsOut = sky3TexColor;
    if (object == 4) colorsOut = sky4TexColor;
    if (object == 5) colorsOut = cloudTexColor;
    if (object == 6) colorsOut = colsExport;

}
