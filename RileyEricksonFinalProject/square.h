#ifndef SQUARE_H
#define SQUARE_H

class square
{
    public:
        square();
        square(const square& other);
        square& operator=(const square& rhs);

        GLint vao;
        void createVAO(GLint _vao, GLint _program );
        virtual ~square();
        vec4 points[36];
        vec4 colors[36];
        vec4 normals[36];
        int numVertices;
        void draw();
    protected:
    private:
        void quad( int a, int b, int c, int d , int col, int norm);
        void colorsquare();
        vec4 vertices[8];
        vec4 vertex_colors[8];
        vec4 face_normals[6];
        int Index;
};

#endif // CUBE_H

