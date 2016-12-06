#ifndef CUBE_H
#define CUBE_H

#include "Angel.h"

class Cube
{
    public:
        Cube();
        Cube(const Cube& other);
        Cube& operator=(const Cube& rhs);

        GLint vao;
        void createVAO(GLint _vao, GLint _program );
        virtual ~Cube();
        vec4 points[36];
        vec4 colors[36];
        vec4 normals[36];
        int numVertices;
        void draw();
    protected:
    private:
        void quad( int a, int b, int c, int d , int col, int norm);
        void colorcube();
        vec4 vertices[8];
        vec4 vertex_colors[8];
        vec4 face_normals[6];
        int Index;
};

#endif // CUBE_H
