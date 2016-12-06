#ifndef TENT_H
#define TENT_H

class TENT
{
    public:
        tent();
        tent(const tent& other);
        tent& operator=(const tent& rhs);

        GLint vao;
        void createVAO(GLint _vao, GLint _program );
        virtual ~tent();
        vec4 points[36];
        vec4 colors[36];
        vec4 normals[36];
        int numVertices;
        void draw();
    protected:
    private:
        void quad( int a, int b, int c, int d , int col, int norm);
        void colortent();
        vec4 vertices[8];
        vec4 vertex_colors[8];
        vec4 face_normals[6];
        int Index;
};

#endif // tent_H

