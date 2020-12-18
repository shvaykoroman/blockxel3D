void
beginUseProgram(GLuint program)
{
    glUseProgram(program);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

void
endUseProgram(GLuint program)
{
    glUseProgram(program);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glUseProgram(0);
}

void
bindTexture(GLuint texture)
{
    glBindTexture(GL_TEXTURE_2D,texture);
}

void
passUniformV3(GLuint shaderProgram,v3 vec, char *name)
{
    GLint loc = glGetUniformLocation(shaderProgram, name);
    
    glUniform3f(loc, vec.x,vec.y,vec.z);
}

void
passUniformMatrix(GLuint shaderProgram, mat4 mat, bool transpose, char *name)
{
    GLint loc =  glGetUniformLocation(shaderProgram, name);
    glUniformMatrix4fv(loc,1,transpose, &mat.e[0][0]);
}


internal GLuint 
createShaderProgram(char **vertexShaderCode, char **fragmentShaderCode)
{
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);  
    glShaderSource(vertexShaderID,1,vertexShaderCode,0);
    glCompileShader(vertexShaderID);
    
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);  
    glShaderSource(fragmentShaderID,1,fragmentShaderCode,0);
    glCompileShader(fragmentShaderID);
    
    GLuint shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertexShaderID);
    glAttachShader(shaderProgramID, fragmentShaderID);
    glLinkProgram(shaderProgramID);
    
    GLint linked = 0;
    GLint compiled = 0;
    glGetProgramiv(shaderProgramID,GL_LINK_STATUS, &linked);
    glGetProgramiv(shaderProgramID,GL_COMPILE_STATUS, &compiled);
    
    if(!(linked && compiled))
    {
        GLenum error = glGetError();
        GLchar log[512] = {};
        glGetProgramInfoLog(shaderProgramID,512,0,log);
        
        DEBUGWriteEntireFile("log.txt", log, sizeof(log));
        
        //assert(!"Error in shader program");
    }
    
    return shaderProgramID;
}


void 
initOpenGL(Game_state *gameState, Opengl *opengl)
{
    // NOTE(shvayko): opengl store texture upside down. Need revert Y
    // TODO(shvayko):rename this. It is not a positions! It is vertex buffer object
    f32 vertexPosittionsCube[] = {
        //     VERTICES       COLOR           TEXTURE UV
        // BACKWARD
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f,0.0f, 
        0.5f, -0.5f, -0.5f,   1.0f, 0.0f,0.0f,   1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,   1.0f, 1.0f,0.0f,   1.0f, 1.0f, 
        
        0.5f,  0.5f, -0.5f,   1.0f, 1.0f,0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,0.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,0.0f,   0.0f, 0.0f,
        // FORWARD
        0.5f, -0.5f,  0.5f,   1.0f, 1.0f,1.0f,    1.0f,0.0f,
        0.5f,  0.5f,  0.5f,   1.0f, 1.0f,0.0f,    1.0f,1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,0.0f,    0.0f,0.0f,
        
        0.5f,  0.5f,  0.5f,   0.0f, 1.0f,0.0f,   1.0f,1.0f, 
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,0.0f,   0.0f,1.0f, 
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,1.0f,   0.0f,0.0f, 
        
        // LEFT
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,0.0f,   1.0f,1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,0.0f,   0.0f,1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f,0.0f,   0.0f,0.0f,
        
        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f,0.0f,   0.0f,0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,0.0f,   1.0f,0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,0.0f,   0.0f,1.0f,
        
        // RIGHT
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,0.0f,    0.0f,1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,0.0f,    1.0f,1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,0.0f,    0.0f,0.0f,
        
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,0.0f,    1.0f,0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,0.0f,    0.0f,1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,0.0f,    1.0f,1.0f,
        // BOTTOM
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,0.0f,    0.0f,0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,0.0f,    1.0f,0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,0.0f,    0.0f,1.0f,
        
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,0.0f,    1.0f,1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,0.0f,   0.0f,0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,0.0f,   0.0f,1.0f,
        // UP
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,0.0f,   0.0f,1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,0.0f,    1.0f,0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,0.0f,    1.0f,1.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,0.0f,    0.0f,1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,0.0f,   1.0f,0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,0.0f,   1.0f,1.0f,
        
    };
    
    f32 vertexPositionsLine[] = 
    {
        0.0f,0.0f,0.0f,  1.0f,0.0f,0.0f,
        2.0f,0.0f,-2.0f, 1.0f,0.0f,0.0f,
    };
    
    f32 quadVertices[] = 
    {
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,0.0f,    0.0f,0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,0.0f,    1.0f,0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,0.0f,    0.0f,1.0f,
        
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,0.0f,    1.0f,1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,0.0f,   0.0f,0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,0.0f,   0.0f,1.0f,
    };
    
    char *vertexShaderCode = 
    {
        "#version 330\n"
            "layout (location = 0) in vec3 aPos;\n"
            "layout (location = 1) in vec3 aColor;\n"
            "layout (location = 2) in vec2 texIn;\n"
            "uniform mat4 view;\n"
            "uniform mat4 projection;\n"
            "uniform mat4 model;\n"
            "uniform vec3 uColor;\n"
            "out vec2 texOut;\n"
            "out vec3 ourColor;\n"
            "void main()\n"
            "{"
            "gl_Position =  projection * view * model * vec4(aPos,1.0f);\n"
            "ourColor = uColor;\n"
            "texOut = texIn;\n"
            "}"
    };
    
    char *fragmentShaderCode = 
        "#version 330\n"
        "out vec4 FragColor;\n"
        "in vec3 ourColor;\n"
        "in vec2 texOut;\n"
        "uniform sampler2D ourTex;\n"
        "void main()\n"
        "{\n"
        "FragColor = texture(ourTex, texOut);\n"
        "}";
    
    
    
    f32 aspectRatioWidthOverHeight = WINDOW_WIDTH / WINDOW_HEIGHT;
    gameState->projection = perspectiveProjection(90.0,aspectRatioWidthOverHeight, 0.1f, 100.0f);
    
    
    opengl->shaderProgram = createShaderProgram(&vertexShaderCode, &fragmentShaderCode);
    
    glGenVertexArrays(1,&vertexArrayCube);
    glBindVertexArray(vertexArrayCube);
    glGenBuffers(1,&vertexBufferCube);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferCube); 
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertexPosittionsCube),vertexPosittionsCube,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 8 * sizeof(f32),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 8 * sizeof(f32),(void*)(3*sizeof(f32)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE, 8 * sizeof(f32),(void*)(6*sizeof(f32)));
    glEnableVertexAttribArray(2);
    
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    
    
    glGenVertexArrays(1,&vertexArrayLine);
    glBindVertexArray(vertexArrayLine);
    glGenBuffers(1,&vertexBufferLine);
    glBindBuffer(GL_ARRAY_BUFFER,vertexBufferLine);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertexPositionsLine),vertexPositionsLine,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 6 * sizeof(f32),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 6 * sizeof(f32),(void*)(3*sizeof(f32)));
    glEnableVertexAttribArray(1);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    
    // NOTE(shvayko): For quad
    
    glGenVertexArrays(1,&vaoQuad);
    glBindVertexArray(vaoQuad);
    glGenBuffers(1, &vboQuad);
    glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 8 * sizeof(f32),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 8 * sizeof(f32),(void*)(3*sizeof(f32)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE, 8 * sizeof(f32),(void*)(6*sizeof(f32)));
    glEnableVertexAttribArray(2);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    
    
    
    beginUseProgram(opengl->shaderProgram);
    passUniformMatrix(opengl->shaderProgram,gameState->projection,false,"projection");
    
}