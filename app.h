#define internal static
#define global static
#define local_persist static

#define assert(expression) if(!(expression)) {*(s32*)0 = 0;}
#define arrayCount(n) (sizeof(n) / sizeof(n[0]))

#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a < b ? a : b)

#define KILOBYTES(x) (1024LL*x)
#define MEGABYTES(x) (KILOBYTES(x) * 1024LL)
#define GIGABYTES(x) (MEGABYTES(x) * 1024LL)

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

typedef size_t sizet;

typedef struct v2
{  
    union
    {
        f32 e[2];
        struct
        {
            f32 x;
            f32 y;
        };
    };
}v2;

#define v2(x,y) v2_create(x,y)

v2 v2_create(f32 x, f32 y)
{
    v2 result;
    
    result = {x,y};
    
    return result;
}

typedef struct v3
{  
    union
    {
        f32 e[3];
        struct
        {
            f32 r;
            f32 g;
            f32 b;
        };
        struct
        {
            f32 x;
            f32 y;
            f32 z;
        };
    };
}v3;

#define v3(x,y,z) v3_create(x,y,z)

v3 v3_create(f32 x, f32 y, f32 z)
{
    v3 result;
    
    result = {x,y,z};
    
    return result;
}

struct Button_state
{
    bool isDown;
    bool changed;
};

struct Keyboard
{
    union
    {
        struct
        {     
            Button_state buttonUp;
            Button_state buttonDown;
            Button_state buttonLeft;
            Button_state buttonRight;
            Button_state buttonArrowLeft;
            Button_state buttonArrowRight;
            Button_state buttonArrowUp;
            Button_state buttonArrowDown;
            Button_state buttonEnter;
            Button_state buttonEscape;
            Button_state buttonSpace;
            
            Button_state LMouseButton;
            Button_state RMouseButton;
        };
        Button_state buttons[12];
    };  
};
struct Input
{
    s32 mouseX, mouseY;
    
    f32 dtForFrame;
    Keyboard controller;
};

struct Memory_arena
{
    sizet size;
    sizet used;
    u8 *base;
    
    // NOTE(shvayko): for debugging purposes
    s32 tempMemoryCount;
};

struct Temp_memory_arena
{
    Memory_arena *arena;
    u32 used;
    
};

Temp_memory_arena
beginTempMemory(Memory_arena *arena)
{
    Temp_memory_arena result = {};
    
    result.arena = arena;
    result.used = arena->used;
    
    arena->tempMemoryCount++;
    
    return result;
}

void
endTempMemory(Temp_memory_arena tempMemory)
{
    Memory_arena *arena = tempMemory.arena;
    assert(arena->used > tempMemory.used);
    
    arena->used = tempMemory.used;
    
}

void
arenaInit(Memory_arena *arena, sizet size, u8 *base)
{
    arena->size = size;
    arena->base = base;
    arena->used = 0;
}

#define PushStruct(arena,type) (type*)pushSize_(arena, sizeof(type))
#define PushArray(arena,type,count) (type*)pushSize_(arena, sizeof(type) * count)
#define PushSize(arena,size) (void*)pushSize_(arena, size)

void *
pushSize_(Memory_arena *arena, sizet size)
{
    void *result = 0;
    assert((arena->used + size) <= arena->size);
    result = arena->base + arena->used;
    arena->used += size;
    
    return result;
}


sizet
getArenaSizeRemaining(Memory_arena *arena)
{
    sizet result = 0;
    
    result =  arena->size - arena->used;
    
    return result;
}

struct File_content
{
    void *memory;
    size_t size;
};

struct Game_memory
{
    void *permanentStorage;
    s64 permanentStorageSize;
    bool isInit;
    
    void *transientStorage;
    u64   transientStorageSize;
    
};

File_content readFile(char *filename);

bool DEBUGWriteEntireFile(char *filename, void *data, u32 bytesToWrite);

struct Game_framebuffer
{
    void *memory;
    s32 width;
    s32 height;
    s32 stride;
    s32 bytesPerPixel;
};

extern "C" void gameUpdateAndRender(Game_framebuffer *gameFramebuffer, Input *input, Game_memory *memory);