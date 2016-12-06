// Generates the vertices, colors, and normals of a unit square
// centered at origin, sides aligned with axes.
//
//    v5----- v6
//   /|      /|
//  v1------v2|              ^ y
//  | |     | |              |
//  | |v4---|-|v7            -->x
//  |/      |/              /
//  v0------v3              z


#include "square.h"
#include <iostream>
using std::cout;

square::square()
{
    Index = 0;


    vertices[0] = vec4( -0.5, -0.5,  0.5, 1.0 );   // v0
    vertices[1] = vec4( -0.5,  0.5,  0.5, 1.0 );   // v1
    vertices[2] = vec4(  0.5,  0.5,  0.5, 1.0 );   // v2
    vertices[3] = vec4(  0.5, -0.5,  0.5, 1.0 );   // v3
    vertices[4] = vec4( -0.5, -0.5, -0.5, 1.0 );   // v4
    vertices[5] = vec4( -0.5,  0.5, -0.5, 1.0 );   // v5
    vertices[6] = vec4(  0.5,  0.5, -0.5, 1.0 );   // v6
    vertices[7] = vec4(  0.5, -0.5, -0.5, 1.0 );    // v7

    vertex_colors[0] =   vec4( 0.0, 0.0, 0.0, 1.0 );
    vertex_colors[1] =   vec4( 1.0, 0.0, 0.0, 1.0 );
    vertex_colors[2] =   vec4( 1.0, 1.0, 0.0, 1.0 );
    vertex_colors[3] =   vec4( 0.0, 1.0, 0.0, 1.0 );
    vertex_colors[4] =  vec4( 0.0, 0.0, 1.0, 1.0 );
    vertex_colors[5] =  vec4( 1.0, 0.0, 1.0, 1.0 );
    vertex_colors[6] =  vec4( 1.0, 1.0, 1.0, 1.0 );
    vertex_colors[7] =  vec4( 0.0, 1.0, 1.0, 1.0 );

// normals
    face_normals[0] = vec4(  1.0,  0.0,  0.0, 0.0 ),  // +x  0
    face_normals[1] = vec4( -1.0,  0.0,  0.0, 0.0 ),  // -x  1
    face_normals[2] = vec4(  0.0,  1.0,  0.0, 0.0 ),  // +y  2
    face_normals[3] = vec4(  0.0, -1.0,  0.0, 0.0 ),  // -y  3
    face_normals[4] = vec4(  0.0,  0.0,  1.0, 0.0 ),  // +z  4
    face_normals[5] = vec4(  0.0,  0.0, -1.0, 0.0 ),  // -z  5

    colorsquare();

/*
    cout << "square  points:\n";
    for (int i = 0; i < 36; i++)
    {
        cout<< i << ":  " <<  points[i] << "\n";
    }
    cout << "colors:\n";
    for (int i = 0; i < 36; i++)
    {
        cout<< i << ":  " <<  colors[i] << "\n";
    }
    cout << "normals:\n";
    for (int i = 0; i < 36; i++)
    {
        cout<< i << ":  " <<  normals[i] << "\n";
    }
    */

}

square::square(const square& other)
{
    for (int i=0; i < 36; i++)
    {
        points[i]=other.points[i];
        colors[i]=other.colors[i];
        normals[i]=other.normals[i];
    }
    numVertices = other.numVertices;
}

square&
square::operator=(const square& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
    for (int i=0; i < 36; i++)
    {
        points[i]=rhs.points[i];
        colors[i]=rhs.colors[i];
        normals[i]=rhs.normals[i];
    }
    numVertices = rhs.numVertices;
}

square::~square()
{}




int Index = 0;

void
square::quad( int a, int b, int c, int d , int col, int norm)
{
    colors[Index] = vertex_colors[col];
    points[Index] = vertices[a];
    normals[Index] = norm;
    Index++;
    colors[Index] = vertex_colors[col];
    points[Index] = vertices[b];
    normals[Index] = norm;
    Index++;
    colors[Index] = vertex_colors[col];
    points[Index] = vertices[c];
    normals[Index] = norm;
    Index++;
    colors[Index] = vertex_colors[col];
    points[Index] = vertices[a];
    normals[Index] = norm;
    Index++;
    colors[Index] = vertex_colors[col];
    points[Index] = vertices[c];
    normals[Index] = norm;
    Index++;
    colors[Index] = vertex_colors[col];
    points[Index] = vertices[d];
    normals[Index] = norm;
    Index++;
}

// generate 12 triangles: 36 vertices and 36 colors
//    v5----- v6
//   /|      /|
//  v1------v2|              ^ y
//  | |     | |              |
//  | |v4---|-|v7            -->x
//  |/      |/              /
//  v0------v3              z
// Always use the Right Hand Rule to generate vertex sequence. We want outward facing normals.
void
square::colorsquare()
{
    // vertices   col norm
    quad( 1, 0, 3, 2 , 2,  4);   // front (+z) face
    quad( 2, 3, 7, 6 , 1,  0);   // +x
    quad( 3, 0, 4, 7 , 3,  3);   // -y
    quad( 6, 5, 1, 2 , 4,  2);   // +y
    quad( 4, 5, 6, 7 , 5,  5);   // -z
    quad( 5, 4, 0, 1 , 6,  1);   // -x

    numVertices = Index;
}


void
square::createVAO(GLint _vao, GLint program )
{
    vao = _vao;
    glBindVertexArray( vao );
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );

    int csize = sizeof(points);
    //cout << "square csize " << csize << "\n";
    glBufferData( GL_ARRAY_BUFFER, 2*csize,
                  NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, csize, points );
    glBufferSubData( GL_ARRAY_BUFFER, csize, csize, colors );
    glBufferSubData( GL_ARRAY_BUFFER, 2 * csize, csize, normals );

    // set up shader variables
    GLint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    GLint vColor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( vColor );
    glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(csize) );
    GLint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 4, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(2*csize) );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );
}

void square::draw()
{
    glBindVertexArray( vao );
    glDrawArrays( GL_TRIANGLES, 0, numVertices );
    glBindVertexArray( 0 );
}






