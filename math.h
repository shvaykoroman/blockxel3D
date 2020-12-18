#ifndef MATH_H
#define MATH_H

#include <math.h>

#define PI 3.14159f

#define ToRadians(x) ((x) * PI / 180.0f)
#define ToDegrees(x) ((x) * 180.0f / PI)

//NOTE
// rows matrix - camera axis
// 
// columns matrix object axis

struct AABB
{
    v3 min;
    v3 max;
};


union mat4
{
    f32 e[4][4]; // ROW major (e[row][column])
};


void
matrixContent(char *matrixName,mat4 matrix)
{
    OutputDebugStringA(matrixName);
    
    char buffer[256];
    
    for(s32 j = 0; j < 4; j++)
    {
        snprintf(buffer,sizeof(buffer),"%f %f %f %f\n",matrix.e[0][j],matrix.e[1][j],matrix.e[2][j],matrix.e[3][j]);
        OutputDebugStringA(buffer);
    }
    
}

mat4 
transpose(mat4 A)
{
    mat4 R;
    
    for(s32 j = 0; j < 4; j++)
    {
        for(s32 i = 0; i < 4; i++) 
        {
            R.e[j][i] = A.e[i][j];
        }
    }
    
    
    return R;
}


mat4 
operator*(mat4 a, mat4 b)
{
    
    mat4 result = {};
    
    for(s32 r = 0; r < 4; r++) // row of A
    {
        for(s32 c = 0; c < 4; c++) // columns
        {
            for(s32 i = 0; i < 4; i++) // columns a, rows b
            {
                result.e[r][c] += a.e[r][i] * b.e[i][c];
            }
        }
    }
    return result;
}


mat4 
orthoProjection(f32 width, f32 height, f32 z = 1.0f)
{
    mat4 R  = {};
    
    f32 widthRatio = 2.0f / width;
    f32 heightRatio = 2.0f / height;
    
    f32 top = height;
    f32 bottom = 0.0f;
    f32 left = 0.0f;
    f32 right = width;
    
    f32 f =  10.0f;
    f32 n =  -10.0f;
    
    f32 c = (-2.0f / (f - n));
    f32 d = -((f + n) / (f - n));
    f32 i = -((top + bottom) / (top - bottom));
    f32 g = -((right + left) / (right - left));
    
    R = 
    {
        widthRatio,  0.0f,  0.0f,   0.0f,
        0.0f,  heightRatio,  0.0f,  0.0f,
        0.0f,  0.0f, c,  0.0f,
        g, i,  d, 1.0f,
    };
    
    return R;
}


mat4
perspectiveProjection(f32 fov,f32 aspectRatio, f32 n, f32 f)
{
    mat4 R = {};
    
    f32 tanThetaOver2 = tan(fov * PI / 360);
    
    f32 x  = (1.0/tanThetaOver2);
    f32 y  = aspectRatio/tanThetaOver2;
    f32 z  = ((n + f) / (n - f));
    f32 z1 = ((2 * n * f)/(n - f));
    
    R = 
    {
        {
            {x,0.0f,0.0f,0.0f},
            {0.0f,y,0.0f,0.0f},
            {0.0f,0.0f,z,-1.0f},
            {0.0f,0.0f,z1,0.0f},
        }
    };
    
    
    return R;
}

mat4 
translate(mat4 model, v3 p)
{
    mat4 R;
    
    R = {
        {
            {1.0f, 0,    0,    0},
            {0,    1.0f, 0,    0},
            {0,    0,    1.0f, 0},
            {p.x, p.y,   p.z,  1.0f}
        },
    };
    
    
    
    R = model * R;
    
    return R;
}



inline mat4
indentity(void)
{
    mat4 m = 
    {
        {   {1,0,0,0},
            {0,1,0,0},
            {0,0,1,0},
            {0,0,0,1}}
    };
    return m;
}

mat4 
xRotation(mat4 A,f32 angle)
{
    f32 c = cos(angle);
    f32 s = sin(angle);
    
    mat4 r = {
        {   {1,0,0,0},
            {0,c,-s,0},
            {0,s,c,0},
            {0,0,0,1}},
    };
    
    r = r * A;
    
    return r;
}


mat4 
yRotation(mat4 A,f32 angle)
{
    f32 c = cos(angle);
    f32 s = sin(angle);
    
    mat4 r = {
        {   {c,0.0f,-s,0.0f},
            {0.0f,1.0f,0.0f,0.0f},
            {s,0.0f,c,0.0f},
            {0.0f,0.0f,0.0f,1.0f}
        },
    };
    
    r = r * A;
    
    matrixContent("yRotation\n",r);
    
    return r;
}

mat4 
zRotation(mat4 A,f32 angle)
{
    f32 c = cos(angle);
    f32 s = sin(angle);
    
    mat4 r = {
        {   {c,-s,0,0},
            {s,c,0,0},
            {0,0,1,0},
            {0,0,0,1}},
    };
    
    r = r * A;
    
    return r;
}



mat4
scale(mat4 matrix, v3 vec)
{
    
    matrix.e[0][0] *= vec.x;
    matrix.e[1][1] *= vec.y;
    matrix.e[2][2] *= vec.z;
    
    return matrix;
}

inline f32
square(f32 a)
{
    f32 result = 0;
    
    result = a * a;
    
    return result;
}

inline v3 
normalize(v3 a)
{
    v3 result;
    
    f32 length = sqrt(square(a.x) + square(a.y) + square(a.z));
    result.x = a.x / length;
    result.y = a.y / length;
    result.z = a.z / length;
    
    return result;
}


v3
operator*(v3 a,f32 b)
{
    v3 result;
    
    result.x = a.x * b;
    result.y = a.y * b;
    result.z = a.z * b;
    
    return result;
}

v3
operator-(v3 a, v3 b)
{
    v3 result;
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    
    return result;
}


v3
operator+(v3 a, v3 b)
{
    v3 result;
    
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    
    return result;
}


v3
operator*(f32 b,v3 a)
{
    v3 result;
    
    result.x = a.x * b;
    result.y = a.y * b;
    result.z = a.z * b;
    
    return result;
}

v3 
operator*(v3 a,v3 b)
{
    v3 result;
    
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    
    return result;
}


inline v3
cross(v3 a, v3 b)
{
    v3 result;
    
    result.x = a.y*b.z - a.z * b.y;
    result.y = a.z*b.x - a.x * b.z;
    result.z = a.x*b.y - a.y * b.x;
    
    return result;
}

inline f32
dot(v3 a, v3 b)
{
    f32 result = a.x * b.x + a.y * b.y + a.z * b.z;
    return result;
}

inline mat4
rows3x3(v3 X, v3 Y, v3 Z)
{
    mat4 R;
    
    R = 
    {
        {
            {X.x,Y.x,Z.x,0.0f},
            {X.y,Y.y,Z.y,0.0f},
            {X.z,Y.z,Z.z,0.0f},
            {0.0f,0.0f,0.0f,1.0f},
        }
    };
    
    return R;
}


inline mat4
columns3x3(v3 X, v3 Y, v3 Z)
{
    mat4 R;
    
    R = 
    {
        {
            {X.x,Y.y,Z.z,0.0f},
            {X.x,Y.y,Z.z,0.0f},
            {X.x,Y.y,Z.z,0.0f},
            {0.0f,0.0f,0.0f,1.0f},
        }
    };
    
    return R;
}


inline mat4
lookAt(v3 eye,v3 center, v3 upTmp)
{
    v3 forward = normalize(center - eye);
    v3 right = normalize(cross(forward,upTmp));
    v3 up = normalize(cross(right,forward));
    
    
    
    forward = v3(-forward.x,-forward.y,-forward.z);
    // NOTE(shvayko): we should pass to function negated forward vec
    mat4 lookAtM = rows3x3(right,up,forward);
    lookAtM.e[3][0] = -dot(right,eye);
    lookAtM.e[3][1] = -dot(up,eye);
    lookAtM.e[3][2] = -dot(forward,eye);
    
    return lookAtM;
}

inline AABB
operator+(AABB aabb, v3 point)
{
    AABB result = {};
    
    result.min = point + aabb.min;
    result.max = point + aabb.max;
    
    return result;
}

inline v3
getTranslationPart(mat4 M)
{
    v3 result = v3(0.0f,0.0f,0.0f);
    
    result.x = M.e[3][0];
    result.y = M.e[3][1];
    result.z = M.e[3][2];
    
    return result;
}


#endif //MATH_H
