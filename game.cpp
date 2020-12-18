#define IsDown(button)  (input->controller.button.isDown)
#define WasDown(button) (input->controller.button.changed)

#include "math.h"
#include "game.h"
#include "collision.h"
#include "collision.cpp"
#include "opengl.h"
#include "opengl.cpp"
#include "png_reader.cpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define ENTITY_MAX_COUNT 256

global u32 gTracersCount;


Bullet
makeBullet(v3 cameraFront,v3 startP, v3 endP)
{
    Bullet result = {};
    if(gTracersCount <= 127)
    {
        mat4 model = indentity();
        model = scale(model, v3(.2f,.2f,1.5f));
        v3 up = v3(0.0f,1.0f,0.0f);
        v3 right = normalize(cross(cameraFront,up));
        mat4 M = rows3x3(right,up,cameraFront);
        model = model * M;
        model = translate(model, startP + v3(0.0f,3.0f,0.0f));
        result.model = model;
        result.velocity = cameraFront;
        result.life = 5.0f;
        
        gTracersCount++;
    }
    return result;
}

void
addEntity(Game_state *gameState, v3 p) // NOTE(shvayko): currently takes only position
{
    Entity *entity = gameState->entityList + gameState->entityCount;
    
    entity->p = p;
    v3 aabbMin = v3(-0.5f,-0.5f,-0.5f);
    v3 aabbMax = v3(0.5f,0.5f,0.5f);
    
    entity->aabb = addAABB(aabbMin,aabbMax);
    
    assert(gameState->entityCount <= 256);
    ++gameState->entityCount;
}

void
deleteEntity(Game_state *gameState, u32 entityIndex)
{
    assert(entityIndex < 0);
    assert(entityIndex < gameState->entityCount);
    
    gameState->entityList[entityIndex] = gameState->entityList[gameState->entityCount - 1];
    
    Entity empty = {};
    
    gameState->entityList[gameState->entityCount - 1] = empty;
    gameState->entityCount;
}

void
drawHero(Hero *hero,v3 cameraFront,mat4 model,GLuint texture)
{
    cameraFront.y = 0;
    glBindVertexArray(vertexArrayCube);
    bindTexture(texture);
    glActiveTexture(texture);
    
    model = scale(model, hero->size);
    
    v3 up = v3(0.0f,1.0f,0.0f);
    
    v3 right = normalize(cross(cameraFront,up));
    
    mat4 M = 
    {
        {
            {right.x,up.x,cameraFront.x,0.0f},
            {right.y,up.y,cameraFront.y,0.0f},
            {right.z,up.z,cameraFront.z,0.0f},
            {0.0f,0.0f,0.0f,1.0f}
        }
    };
    model = model * M;
    model = translate(model, hero->p);
    
    passUniformMatrix(opengl.shaderProgram,model,false,"model");
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

f32 
approach(f32 goal,f32 current,f32 dt)
{
    // TODO(shvayko): I think it is doesn't work!
    f32 diff = goal - current;
    if(diff > dt) 
    {
        return current + dt;
    }
    if(diff < -dt) 
    {
        return  current - dt;
    }
    return goal;
}

void 
cameraTransform(Camera *camera, Input *input, Game_state *gameState)
{
    beginUseProgram(opengl.shaderProgram);
    if(camera->firstMouseMove)
    {
        camera->lastMouseX = input->mouseX;
        camera->lastMouseY = input->mouseY;
        
        camera->firstMouseMove = false;
    }
    
    s32 dtMouseX = input->mouseX - camera->lastMouseX;
    s32 dtMouseY = camera->lastMouseY - input->mouseY;
    
    dtMouseX *= camera->mouseSensitivity;
    dtMouseY *= camera->mouseSensitivity;
    
    camera->heading += dtMouseX;
    camera->pitch += dtMouseY;
    
    if(camera->pitch > 89.0f)
    {
        camera->pitch  = 89.0f;
    }
    if(camera->pitch < -89.0f)
    {
        camera->pitch = -89.0f;
    }
    
    gameState->cameraFront.x = cos(ToRadians(camera->heading)) * cos(ToRadians(camera->pitch));
    gameState->cameraFront.y = sin(ToRadians(camera->pitch));
    gameState->cameraFront.z = sin(ToRadians(camera->heading)) * cos(ToRadians(camera->pitch));
    
    gameState->cameraFront = normalize(gameState->cameraFront);
    
    mat4 viewMatrix = indentity();
    viewMatrix = lookAt(gameState->cameraP,
                        gameState->cameraP + gameState->cameraFront,
                        v3(0.0f,1.0f,0.0f));
    
    
    passUniformMatrix(opengl.shaderProgram,viewMatrix,false,"view");
    
    camera->lastMouseX = WINDOW_WIDTH  / 2;
    camera->lastMouseY = WINDOW_HEIGHT / 2;
    
    endUseProgram(opengl.shaderProgram);
}

void
loadTextureToTheGame(Game_state *gameState, char *filename)
{
#if 1
    // NOTE(shvayko): Our png loading
    File_content file = readFile(filename);
    streaming_buffer loadedFile = {};
    loadedFile.size = file.size;
    loadedFile.memory = file.memory;
    u32 width,height;
    u8 *data = parsingPNG(&gameState->worldArena,&loadedFile,&width,&height);
    
    imageFlipY(data,width,height);
    
    gameState->testTexture[gameState->testTextureCount] = data;
#else
    // NOTE(shvayko): STB library
    s32 width,height,n;
    u8 *data = stbi_load(filename, &width, &height, &n, 4);
    gameState->testTexture[0] = data;
#endif
    
    GLuint texture = 0;
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
    
    textures[gameState->testTextureCount] = texture;
    
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    
    if(gameState->testTexture[gameState->testTextureCount])
    {
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0, GL_RGBA,GL_UNSIGNED_BYTE,gameState->testTexture[gameState->testTextureCount++]);
    }
}

extern "C" void gameUpdateAndRender(Game_framebuffer *framebuffer, Input *input, Game_memory *memory)
{  
#if 1
    u32 map[5][10][10] =
    {
        {
            {1,1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,1,1}
        },
        
        {
            {1,1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,1,1}
        },
        {
            {1,1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,1,1}
        },
        {
            {1,1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,1,1}
        },
        {
            {1,1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,1,1}
        },
    };
    
#endif
    
    Game_state *gameState = (Game_state*)memory->permanentStorage;
    if(!memory->isInit)
    {
        arenaInit(&gameState->worldArena, memory->permanentStorageSize - sizeof(Game_state), (u8*)memory->permanentStorage + sizeof(Game_state));
        
        gameState->hero = PushStruct(&gameState->worldArena, Hero);
        Hero *hero = gameState->hero;
        gameState->entityList = PushArray(&gameState->worldArena, Entity, ENTITY_MAX_COUNT);
        
        hero->color = v3(1.0f,0.0f,0.0f);
        hero->velocity = v3(0.0f,0.0f,0.0f);
        hero->p = v3(0.0f,0.0f, -5.0f);
        hero->size = v3(1.5f,4.0f,1.0f);
        
        addEntity(gameState, v3(3.0f,1.0f, 2.0f));
        addEntity(gameState, v3(7.0f,0.0f, 4.0f));
        addEntity(gameState, v3(2.0f,3.0f,-3.0f));
        addEntity(gameState, v3(3.0f,0.5f,-1.0f));
        
        initOpenGL(gameState,&opengl);
        
        gameState->cameraFront =  v3(0.0f, 0.0f, -1.0f);
        
        char *texturePaths[] = 
        {
            "../data/test.png",
            "../data/grass.png"
                
        };
        
        loadTextureToTheGame(gameState, texturePaths[0]);
        loadTextureToTheGame(gameState, texturePaths[1]);
        
        gameState->gravity = v3(0.0f,-2.0f,0.0f);
        
        gameState->camera.mouseSensitivity = 0.4f;
        gameState->camera.firstMouseMove = true;
        gameState->camera.heading = -90.0f;
        memory->isInit = true;
    }
    
    Hero *hero = gameState->hero;
    
    glViewport(0,0,framebuffer->width,framebuffer->height);
    glClearColor(1.0f,0.6f,0.4f,0.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
    
    // NOTE(shvayko): Moving the mouse and camera
    cameraTransform(&gameState->camera,input, gameState);
    
    v3 ddp = v3(0.0f,0.0f,0.0f);
    
    // NOTE(shvayko): handling input
    if(WasDown(buttonSpace))
    {
        ddp.y = 10.0f;
        hero->velocity.y = ddp.y;
    }
    
    if(IsDown(buttonRight))
    {
        ddp.x = 1.0f;
    }  
    
    if(IsDown(buttonLeft))
    {
        ddp.x = -1.0f;
    }  
    
    if(IsDown(buttonDown))
    {
        ddp.z = 1.0f;
    }  
    
    if(IsDown(buttonUp))
    {
        ddp.z = -1.0f;
    }  
    
    // NOTE(shvayko): player movement
    f32 playerSpeed = 1.0f;
    ddp = ddp * playerSpeed;
    
    if((ddp.x > 0.0f) && (ddp.z > 0.0f))
    {
        ddp = v3(0.70710678118f,ddp.y,0.70710678118f);
    }
    
    {
        gameState->cameraP = hero->p - gameState->cameraFront * 10.0f;
        hero->move.x = approach(ddp.x, hero->move.x,input->dtForFrame);
        hero->move.z = approach(ddp.z, hero->move.z,input->dtForFrame);
        
        v3 up = v3(0.0f,1.0f,0.0f);
        
        v3 cameraRight = cross(gameState->cameraFront,up);
        f32 camYSafe = gameState->cameraFront.y;
        gameState->cameraFront.y = 0;
        cameraRight = normalize(cameraRight);
        
        f32 safeYVel = hero->velocity.y;
        hero->velocity = gameState->cameraFront * -hero->move.z + cameraRight * hero->move.x;
        hero->velocity.y = safeYVel;
        gameState->cameraFront.y = camYSafe;
        
        hero->p = hero->p + hero->velocity * input->dtForFrame;
        hero->velocity = hero->velocity + gameState->gravity * input->dtForFrame;
    }
    
    
    if(hero->p.y < 0) 
    {
        hero->p.y = 0;
    }
    
    // NOTE(shvayko): bullet 
    // TODO(shvayko): bug with the doblue click when mouse button pressed only once
    if(WasDown(LMouseButton))
    {
        v3 bulletStartP = hero->p;
        v3 bulletEndP = hero->p + gameState->cameraFront * 150.0f;
        
        // NOTE(shvayko): check collision for all entities in the world
        for(u32 entityIndex = 0; entityIndex < gameState->entityCount; entityIndex++)
        {
            Entity *entity = gameState->entityList + entityIndex;
            AABB entityAABB = entity->aabb;
            
            if(testLineSegmentCollision(entityAABB + entity->p, bulletStartP + v3(0.0,4.0f,0.0f), bulletEndP))
            {
                gameState->tracers[gTracersCount - 1] = makeBullet(gameState->cameraFront,bulletStartP, bulletEndP);
                entity->nonActive = true;
            }
            else 
            {
                gameState->tracers[gTracersCount - 1] = makeBullet(gameState->cameraFront,bulletStartP, bulletEndP);
            }
        }
    }
    
    // NOTE(shvayko):  "floor"
    {
        beginUseProgram(opengl.shaderProgram);
        glBindVertexArray(vaoQuad);
        
        mat4 model = indentity();
        model = scale(model, v3(100.0f,10.0f,100.0f));
        passUniformMatrix(opengl.shaderProgram,model,false,"model");
        glDrawArrays(GL_TRIANGLES, 0, 36);
        endUseProgram(opengl.shaderProgram);
    }
    
    // NOTE(shvayko): Render player cube
    beginUseProgram(opengl.shaderProgram);
    mat4 heroModel = indentity();
    drawHero(hero,gameState->cameraFront,heroModel,textures[blockType_grass]);
    
    // NOTE(shvayko): Render entities
    glActiveTexture(textures[blockType_grass]);
    for(u32 entityIndex = 0; entityIndex < gameState->entityCount; entityIndex++)
    {
        Entity *entity = gameState->entityList + entityIndex;
        if(!entity->nonActive)
        {
            mat4 entityModel = indentity();
            entityModel = translate(entityModel, entity->p);
            passUniformMatrix(opengl.shaderProgram,entityModel,false,"model");
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
    
    // NOTE(Shvayko): render and simulate all traces
    for(u32 tracerIndex = 0; tracerIndex < gTracersCount; tracerIndex++)
    {
        Bullet *tracer = gameState->tracers + tracerIndex;
        if(tracer->life > 0)
        {
            v3 p = getTranslationPart(tracer->model);
            
            v3 offset =  tracer->velocity * input->dtForFrame;
            tracer->model = translate(tracer->model, offset);
            
            passUniformMatrix(opengl.shaderProgram,tracer->model,false,"model");
            glDrawArrays(GL_TRIANGLES, 0, 36);
            tracer->life -= input->dtForFrame;
        }
    }
    
    // NOTE(shvayko): Render "something"
    for(s32 z = 0; z < 5; z++)
    {
        for(s32 y = 0; y < 10; y++)
        {
            for(s32 x = 0; x < 10; x++)
            {
                u32 *blocks = (u32*)map;
                u32 blockID = (u32)blocks[z * 100 + y * 10 + x];
                
                if(blockID)
                {
                    f32 zOffset = -15.0f;
                    mat4 modelMatrix = indentity();
                    modelMatrix = translate(modelMatrix, v3(x,y,zOffset - z));
                    
                    passUniformMatrix(opengl.shaderProgram,modelMatrix,false,"model");
                    bindTexture(textures[blockType_grass]);
                    glActiveTexture(textures[blockType_grass]);
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
            }
        }
    }
    
    glUseProgram(0);
}
