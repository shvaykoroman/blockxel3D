#ifndef GAME_H
#define GAME_H

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 980


struct Camera
{
    f32 lastMouseX;
    f32 lastMouseY;
    
    f32 mouseSensitivity;
    bool firstMouseMove;
    
    f32 heading;
    f32 pitch;
    f32 bank;
    
};

enum
{
    blockType_roma = 0,
    blockType_grass,
};

struct Hero
{
    AABB aabb;
    // TODO(shvayko): v4 for color would be better
    v3 color;
    v3 velocity;
    v3 p;
    v3 size;
    v3 move;
    
    mat4 transform;
};

struct Entity 
{
    AABB aabb;
    
    v3 color;
    v3 velocity;
    v3 p;
    v3 size;
    
    bool nonActive;
    
    mat4 transform;
};

#define MAX_BULLET_TRACERS 128

struct Bullet
{
    v3 startP;
    v3 endP;
    f32 length;
    v3 velocity;
    f32 life;
    
    mat4 model;
};

struct Game_state
{
    Memory_arena worldArena;
    mat4 projection;
    
    v3 cameraP;
    v3 cameraFront;
    
    Camera camera;
    Hero *hero;
    
    Bullet tracers[MAX_BULLET_TRACERS];
    
    Entity *entityList;
    u32 entityCount;
    
    v3 gravity;
    
    u32 *map;
    
    s32 testTextureCount;
    u8 *testTexture[32];
};


#endif //GAME_H
