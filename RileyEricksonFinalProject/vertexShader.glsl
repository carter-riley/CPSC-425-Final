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

layout(location=12) in vec4 torCoords;
layout(location=13) in vec3 torNormal;

layout(location=14) in vec4 tentCoords;
layout(location=15) in vec3 tentNormal;

uniform mat4 modelViewMat;
uniform mat4 projMat;
uniform mat3 normalMat;
uniform uint object;

out vec2 texCoordsExport;
out vec4 colsExport;

vec4 coords;

struct Light
{
    vec4 ambCols;
    vec4 difCols;
    vec4 specCols;
    vec4 lCoords;
};
uniform Light light0;

uniform vec4 globAmb;

struct Material
{
    vec4 ambRefl;
    vec4 difRefl;
    vec4 specRefl;
    vec4 emitCols;
    float shininess;
};
uniform Material torus;
uniform Material tent;

vec3 normal, lightDirection, eyeDirection, halfway;
vec4 emit, torGlobAmb, amb, dif, spec;


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
    if (object == 6)
    {
        normal = torNormal;

        normal = normalize(normalMat * normal);
        lightDirection = normalize(vec3(light0.lCoords));
        eyeDirection = 1.0 * normalize(vec3(modelViewMat * torCoords));
        halfway = (length(lightDirection + eyeDirection) == 0.0) ?
                  vec3(0.0) : (lightDirection + eyeDirection)/length(lightDirection + eyeDirection);

        emit = torus.emitCols;
        torGlobAmb = globAmb * torus.ambRefl;
        amb = light0.ambCols * torus.ambRefl;
        dif = max(dot(normal, lightDirection), 0.0) * light0.difCols * torus.difRefl;
        spec = pow(max(dot(normal, halfway), 0.0), torus.shininess) *
               light0.specCols * torus.specRefl;
        coords = torCoords;
        colsExport =  vec4(vec3(min(emit + torGlobAmb + amb +
                                    dif + spec, vec4(1.0))), 1.0);
        //colsExport =  vec4(abs(torNormal), 1.0);
    }
//    if (object == 7)
//    {
//        normal = tentNormal;
//
//        normal = normalize(normalMat * normal);
//        lightDirection = normalize(vec3(light0.lCoords));
//        eyeDirection = 1.0 * normalize(vec3(modelViewMat * torCoords));
//        halfway = (length(lightDirection + eyeDirection) == 0.0) ?
//                  vec3(0.0) : (lightDirection + eyeDirection)/length(lightDirection + eyeDirection);
//
//        emit = torus.emitCols;
//        torGlobAmb = globAmb * torus.ambRefl;
//        amb = light0.ambCols * torus.ambRefl;
//        dif = max(dot(normal, lightDirection), 0.0) * light0.difCols * torus.difRefl;
//        spec = pow(max(dot(normal, halfway), 0.0), torus.shininess) *
//               light0.specCols * torus.specRefl;
//        coords = torCoords;
//        colsExport =  vec4(vec3(min(emit + torGlobAmb + amb +
//                                    dif + spec, vec4(1.0))), 1.0);
//        //colsExport =  vec4(abs(torNormal), 1.0);
//    }
    gl_Position = projMat * modelViewMat * coords;
}
