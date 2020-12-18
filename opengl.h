#ifndef OPENGL_H
#define OPENGL_H

struct Opengl
{
    GLuint shaderProgram;
};

// TODO(shvayko): delete those globals!
global Opengl opengl;

global GLuint textures[256];

global GLuint vertexBufferCube;
global GLuint vertexBufferLine;

global GLuint vertexArrayCube;
global GLuint vertexArrayLine;

global GLuint vaoQuad;
global GLuint vboQuad;

#endif //OPENGL_H
