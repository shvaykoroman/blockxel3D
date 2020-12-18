#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <gl/gl.h>
#include "app.h"

#define GL_ARRAY_BUFFER    0x8892
#define GL_STATIC_DRAW     0x88E4
#define GL_VERTEX_SHADER   0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPILE_STATUS  0x8B81
#define GL_LINK_STATUS     0x8B82 
#define GL_MAJOR_VERSION   0x821B
#define GLsizeiptr size_t

typedef void WINAPI gl_gen_buffers(GLsizei n,GLuint *buffers);
global gl_gen_buffers *glGenBuffers;

typedef void WINAPI gl_bind_buffer(GLenum target,GLuint buffer);
global gl_bind_buffer *glBindBuffer;

typedef void gl_buffer_data(GLenum target,GLsizeiptr size,const void * data,GLenum usage);
global gl_buffer_data *glBufferData;

typedef void gl_vertex_attrib_pointer(GLuint index, GLint size,GLenum type,GLboolean normalized,GLsizei stride,const GLvoid *pointer);
global gl_vertex_attrib_pointer *glVertexAttribPointer;

typedef void gl_enable_vertex_attrib_array(GLuint index);
global gl_enable_vertex_attrib_array *glEnableVertexAttribArray;

typedef void gl_bind_vertex_array(GLuint array);
global gl_bind_vertex_array *glBindVertexArray;

typedef GLuint gl_create_shader(GLenum shaderType);
global gl_create_shader *glCreateShader;

typedef char GLchar;

typedef void gl_shader_source(GLuint shader, GLsizei count, GLchar **string, GLint *length);
global gl_shader_source *glShaderSource;

typedef void gl_compile_shader(GLuint shader);
global gl_compile_shader *glCompileShader;

typedef void gl_attach_shader(GLuint program,GLuint shader);
global gl_attach_shader *glAttachShader;

typedef void gl_link_program(GLuint program);
global gl_link_program *glLinkProgram;

typedef GLuint gl_create_program(void);
global gl_create_program *glCreateProgram;

typedef void gl_get_programiv(GLuint program, GLenum pname, GLint *params);
global gl_get_programiv *glGetProgramiv;

typedef void gl_get_program_info_log(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
global  gl_get_program_info_log *glGetProgramInfoLog;

typedef  void  gl_use_program(GLuint program);
global gl_use_program *glUseProgram;

typedef GLint gl_get_uniform_location(GLuint program, const GLchar *name);
global gl_get_uniform_location *glGetUniformLocation;

typedef void  gl_uniform_matrix_4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
global gl_uniform_matrix_4fv *glUniformMatrix4fv;

typedef HGLRC wgl_create_context_attribs_arb(HDC hDC, HGLRC hshareContext, const int *attribList);
global wgl_create_context_attribs_arb *wglCreateContextAttribsARB;

typedef void  gl_uniform_3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
global gl_uniform_3f *glUniform3f;

typedef void gl_gen_vertex_arrays(GLsizei n, GLuint *arrays);
global gl_gen_vertex_arrays *glGenVertexArrays;

typedef void gl_active_texture(GLenum texture);
global gl_active_texture *glActiveTexture;

typedef void  gl_disable_vertex_attrib_array(GLuint index);
global gl_disable_vertex_attrib_array *glDisableVertexAttribArray ;

#include "game.cpp"

global bool gAppIsRunning;
#define SOFTWARE_RENDERER 0

struct Framebuffer
{
    BITMAPINFO bitmapInfo;
    void *memory;
    s32 width;
    s32 height;
    s32 stride;
    s32 bytesPerPixel;
};

global Framebuffer gFramebuffer;

struct Window_dim
{
    s32 width;
    s32 height;
};


inline
LARGE_INTEGER getClockValue()
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter;
}

inline
f32 getDifferenceTime(LARGE_INTEGER endTime,LARGE_INTEGER beginTime)
{
    f32 result;
    
    result = (f32)(endTime.QuadPart - beginTime.QuadPart);
    
    return result;
}

void
freeFile(File_content *fileContent)
{
    if(fileContent->memory)
    {
        VirtualFree(fileContent->memory,0, MEM_RELEASE);
    }
}

File_content
readFile(char *filename)
{
    File_content result = {};
    HANDLE fileHandle = CreateFileA(filename,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
    
    if(fileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER fileSize;
        GetFileSizeEx(fileHandle, &fileSize);
        result.size = (u32)fileSize.QuadPart;
        result.memory = VirtualAlloc(0, (size_t)fileSize.QuadPart, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        u32 bytesToRead = result.size;
        DWORD bytesReaden;
        if(ReadFile(fileHandle,result.memory,bytesToRead, &bytesReaden,0)
           && bytesToRead == bytesReaden)
        {
            // NOTE(shvayko): file read success
        }
        else
        {	  
            VirtualFree(result.memory,0, MEM_RELEASE);
        }
    }
    else
    {
        // TODO(shvayko): logging
    }
    return result;
}

internal Window_dim
getWindowDim(HWND window)
{
    Window_dim result = {};
    RECT rect;
    GetClientRect(window, &rect);
    result.width  = rect.right -  rect.left;
    result.height = rect.bottom - rect.top;
    
    return result;
}

internal void
createFramebuffer(Framebuffer *framebuffer, s32 width, s32 height)
{
    if(framebuffer->memory)
    {
        VirtualFree(framebuffer->memory, 0, MEM_RELEASE);
    }
    
    framebuffer->width  = width;  
    framebuffer->height = height;
    framebuffer->bytesPerPixel = 4;
    
    framebuffer->bitmapInfo.bmiHeader = {};  
    
    framebuffer->bitmapInfo.bmiHeader.biSize = sizeof(framebuffer->bitmapInfo.bmiHeader);
    framebuffer->bitmapInfo.bmiHeader.biWidth = framebuffer->width;
    framebuffer->bitmapInfo.bmiHeader.biHeight = -framebuffer->height;
    framebuffer->bitmapInfo.bmiHeader.biPlanes = 1;
    framebuffer->bitmapInfo.bmiHeader.biBitCount = 32;
    framebuffer->bitmapInfo.bmiHeader.biCompression = BI_RGB;
    
    s32 framebufferSize = framebuffer->width * framebuffer->height * framebuffer->bytesPerPixel;
    
    framebuffer->memory = VirtualAlloc(0,(size_t)framebufferSize,MEM_COMMIT | MEM_RESERVE,PAGE_READWRITE);
    framebuffer->stride = framebuffer->width * framebuffer->bytesPerPixel;
    
    s8 *destRow = (s8*)framebuffer->memory;  
}

internal void
createOpenglContext(HWND window, HDC deviceContext)
{
    PIXELFORMATDESCRIPTOR pixelFormat = {};
    
    pixelFormat.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pixelFormat.nVersion = 1;
    pixelFormat.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pixelFormat.iPixelType = PFD_TYPE_RGBA;
    pixelFormat.cColorBits = 24;
    pixelFormat.cStencilBits = 8;
    pixelFormat.iLayerType = PFD_MAIN_PLANE;
    
    s32 pixelFormatIndex = ChoosePixelFormat(deviceContext, &pixelFormat);
    if(SetPixelFormat(deviceContext,pixelFormatIndex,&pixelFormat))
    {
        HGLRC renderingContext = wglCreateContext(deviceContext);
        if(renderingContext)
        {
            // NOTE(shvayko): SUCCESS
            // NOTE(shvayko): load opengl functions
            bool result = wglMakeCurrent(deviceContext, renderingContext);
            glGenBuffers = (gl_gen_buffers*)wglGetProcAddress("glGenBuffers");
            glBindBuffer = (gl_bind_buffer*)wglGetProcAddress("glBindBuffer");	  
            glBufferData = (gl_buffer_data*)wglGetProcAddress("glBufferData");	  
            glVertexAttribPointer = (gl_vertex_attrib_pointer*)wglGetProcAddress("glVertexAttribPointer");
            glEnableVertexAttribArray = (gl_enable_vertex_attrib_array*)wglGetProcAddress("glEnableVertexAttribArray");
            glBindVertexArray = (gl_bind_vertex_array*)wglGetProcAddress("glBindVertexArray");
            glCreateShader  = (gl_create_shader*)wglGetProcAddress("glCreateShader");
            glShaderSource  = (gl_shader_source*)wglGetProcAddress("glShaderSource");
            glCompileShader = (gl_compile_shader*)wglGetProcAddress("glCompileShader");
            glAttachShader  = (gl_attach_shader*)wglGetProcAddress("glAttachShader");
            glLinkProgram   = (gl_link_program*)wglGetProcAddress("glLinkProgram");
            glCreateProgram = (gl_create_program*)wglGetProcAddress("glCreateProgram");
            glGetProgramiv  =  (gl_get_programiv*)wglGetProcAddress("glGetProgramiv");
            glGetProgramInfoLog = (gl_get_program_info_log*)wglGetProcAddress("glGetProgramInfoLog");
            glUseProgram =  (gl_use_program*)wglGetProcAddress("glUseProgram");
            glGetUniformLocation = (gl_get_uniform_location*)wglGetProcAddress("glGetUniformLocation");
            glUniformMatrix4fv = (gl_uniform_matrix_4fv*)wglGetProcAddress("glUniformMatrix4fv");
            wglCreateContextAttribsARB = (wgl_create_context_attribs_arb*)wglGetProcAddress("wglCreateContextAttribsARB");
            glUniform3f = (gl_uniform_3f*)wglGetProcAddress("glUniform3f");
            glGenVertexArrays = (gl_gen_vertex_arrays*)wglGetProcAddress("glGenVertexArrays");
            glActiveTexture = (gl_active_texture*)wglGetProcAddress("glActiveTexture");
            glDisableVertexAttribArray = (gl_disable_vertex_attrib_array*)wglGetProcAddress("glDisableVertexAttribArray");
        }
        else
        {
            
        }
    }
    else
    {
        
    }
}

bool
DEBUGWriteEntireFile(char *filename, void *data, u32 bytesToWrite)
{
    bool result = false;
    HANDLE fileHandle = CreateFileA(filename,GENERIC_WRITE,0,0,CREATE_ALWAYS,0,0);
    
    if(fileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD bytesWritten;
        WriteFile(fileHandle,data,bytesToWrite, &bytesWritten,0);
        if(bytesToWrite == bytesWritten)
        {
            // NOTE(shvayko): success
            result = true;
        }
        else
        {
            // NOTE(shvayko): failure
            assert(!"Writting file error");
        }
    }
    
    return result;
}

internal void
blitBufferToScreen(Framebuffer *framebuffer, HDC deviceContext,s32 width,s32 height)
{
#if SOFTWARE_RENDERER 
    StretchDIBits(
                  deviceContext,
                  0,0, width, height,
                  0,0, framebuffer->width, framebuffer->height,
                  framebuffer->memory,
                  &framebuffer->bitmapInfo,
                  DIB_RGB_COLORS,
                  SRCCOPY
                  );
#else  
    SwapBuffers(deviceContext);
#endif
}

LRESULT handlingWindowMessages(HWND    window,
                               UINT    message,
                               WPARAM  wParam,
                               LPARAM  lParam
                               )
{
    LRESULT result = 0;
    
    switch(message)
    {
        case WM_CREATE:
        {
            OutputDebugStringA("WM_CREATE\n");
        }break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC deviceContext = BeginPaint(window, &ps);
            
            Window_dim windowDim = getWindowDim(window);
            blitBufferToScreen(&gFramebuffer, deviceContext,windowDim.width,windowDim.height);
            
            EndPaint(window, &ps);
        }break;
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            gAppIsRunning = false;
            OutputDebugStringA("WM_DESTROY\n");
        }break;
        case WM_QUIT:
        {
            gAppIsRunning = false;
            OutputDebugStringA("WM_QUIT\n");
        }break;
        case WM_KEYUP:
        case WM_KEYDOWN:
        {
            assert("Must not happen!");
        }break;
        default:
        {
            return DefWindowProc(window, message, wParam, lParam);
        }break;
    }
    
    return result;
}

internal void
handleInputMessages(Keyboard *input)
{
    MSG message;
    local_persist bool lButtonState = false;
    local_persist bool rButtonState = false;
    
    while(PeekMessageA(&message,0,0,0,PM_REMOVE))
    {
        switch(message.message)
        {
            case WM_LBUTTONUP:
            {
                input->LMouseButton.isDown = false;
            }break;
            case WM_LBUTTONDOWN: 
            {
                if(!input->LMouseButton.isDown) 
                {
                    input->LMouseButton.changed = true;
                }
                input->LMouseButton.isDown = true;
            }break;
            case WM_RBUTTONUP:
            {
                input->LMouseButton.isDown = false;
            }break;
            case WM_RBUTTONDOWN: 
            {
                if(!input->RMouseButton.isDown) 
                {
                    input->RMouseButton.changed = true;
                }
                input->RMouseButton.isDown = false;
            }break;
            case WM_KEYUP:
            case WM_KEYDOWN:
            {
                bool keyIsDown  = ((message.lParam & (1 << 31)) == 0);
                bool keyWasDown = ((message.lParam & (1 << 30)) != 0);
                if(keyIsDown != keyWasDown)
                {
                    switch(message.wParam)
                    {
                        case 'W':
                        {
                            input->buttonUp.isDown = keyIsDown;
                            input->buttonUp.changed = keyIsDown != keyWasDown;
                        }break;
                        case 'A':
                        {
                            input->buttonLeft.isDown = keyIsDown;
                            input->buttonLeft.changed = keyIsDown != keyWasDown;
                            
                        }break;
                        case 'S':
                        {
                            input->buttonDown.isDown = keyIsDown;
                            input->buttonDown.changed = keyIsDown != keyWasDown;
                            
                        }break;
                        case 'D':
                        {
                            input->buttonRight.isDown = keyIsDown;
                            input->buttonRight.changed = keyIsDown != keyWasDown;
                        }break;
                        case VK_SPACE:
                        {
                            input->buttonSpace.isDown = keyIsDown;
                            input->buttonSpace.changed = keyIsDown != keyWasDown;
                        }break;
#if 0 // NOTE(shvayko): is not working in that way
                        case VK_LBUTTON:
                        {
                            input->LMouseButton.isDown = keyIsDown;
                            input->LMouseButton.changed = keyIsDown != keyWasDown;
                        }break;
                        case VK_RBUTTON:
                        {
                            input->RMouseButton.isDown = keyIsDown;
                            input->RMouseButton.changed = keyIsDown != keyWasDown;
                        }break;
#endif
                    }
                }break;
            }
            default:
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }break;
        }	  
    }
}


int  WinMain(
             HINSTANCE hInstance,
             HINSTANCE hPrevInstance,
             LPSTR     lpCmdLine,
             int       nShowCmd
             )
{
    LARGE_INTEGER performanceFreqRes;
    QueryPerformanceFrequency(&performanceFreqRes);
    s64 performanceFreq = performanceFreqRes.QuadPart;
    
    createFramebuffer(&gFramebuffer, 1280, 980);
    
    char *className = "appClassName";
    
    WNDCLASS windowClass = {};
    windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
    windowClass.lpfnWndProc = &handlingWindowMessages;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = className;
    
    RegisterClassA(&windowClass);
    
    HWND window = CreateWindowExA(0,
                                  className,
                                  "app",
                                  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  1280,
                                  980,
                                  0,
                                  0,
                                  hInstance,
                                  0);
    if(window)
    {
        Game_memory gameMemory = {};
        gameMemory.permanentStorageSize = MEGABYTES(128);
        gameMemory.permanentStorage = VirtualAlloc(0, gameMemory.permanentStorageSize, MEM_COMMIT | MEM_RESERVE,
                                                   PAGE_READWRITE);
        
        assert(gameMemory.permanentStorage);
        
        gameMemory.transientStorageSize = MEGABYTES(64);
        gameMemory.transientStorage = VirtualAlloc(0, gameMemory.transientStorageSize, MEM_COMMIT | MEM_RESERVE,
                                                   PAGE_READWRITE);
        
        assert(gameMemory.transientStorage);
        
        MSG msg;
        gAppIsRunning = true;
        HDC deviceContext = GetDC(window);
        createOpenglContext(window,deviceContext);
        
        Input input[2] = {};
        Input *newInput = &input[0];
        Input *oldInput = &input[1];	  
        
        s32 gameUpdateHZ = 60;
        f32 targetSecondsPerFrame = 1.0f/(f32)gameUpdateHZ;
        LARGE_INTEGER lastTime = getClockValue();
        while(gAppIsRunning)
        {
            POINT mousePos = {};
            GetCursorPos(&mousePos);
            
            
            newInput->dtForFrame = targetSecondsPerFrame;
            newInput->mouseX = mousePos.x;
            newInput->mouseY = mousePos.y;
            
            Keyboard *oldKeyboardInput = &oldInput->controller;
            Keyboard *newKeyboardInput = &newInput->controller;
            Keyboard zeroController = {};
            *newKeyboardInput = zeroController;
            
            
            for(s32 buttonIndex = 0; buttonIndex < arrayCount(newKeyboardInput->buttons); buttonIndex++)
            {
                newKeyboardInput->buttons[buttonIndex].isDown = oldKeyboardInput->buttons[buttonIndex].isDown;
            }
            
            
            newKeyboardInput->LMouseButton.changed = false;
            newKeyboardInput->RMouseButton.changed = false;
            
            handleInputMessages(newKeyboardInput);
            
            
            Game_framebuffer gameFramebuffer = {};
            gameFramebuffer.width = gFramebuffer.width;
            gameFramebuffer.height = gFramebuffer.height;
            gameFramebuffer.stride = gFramebuffer.stride;
            gameFramebuffer.bytesPerPixel = gFramebuffer.bytesPerPixel;
            gameFramebuffer.memory = gFramebuffer.memory;
            SetCursorPos(1280.0f/2.0f, 980.0f/2.0f);
            gameUpdateAndRender(&gameFramebuffer, newInput, &gameMemory);
            
            
            LARGE_INTEGER currentTime = getClockValue();
            
            f32 delta = getDifferenceTime(currentTime,lastTime) / (f32)performanceFreq;	      
            f32 secondsElapsedSoFar = delta;
            
            if(secondsElapsedSoFar < targetSecondsPerFrame)
            {		  
                DWORD sleepMS =
                    (DWORD)(1000.0f * (targetSecondsPerFrame - secondsElapsedSoFar));		      
                currentTime = getClockValue();
                secondsElapsedSoFar =  getDifferenceTime(currentTime, lastTime) / (f32)performanceFreq;
                while(secondsElapsedSoFar < targetSecondsPerFrame)
                {
                    currentTime = getClockValue();
                    secondsElapsedSoFar =  getDifferenceTime(currentTime, lastTime) / (f32)performanceFreq;
                }
            }
            else
            {
                // TODO(shvayko): log missed frame
            }
            currentTime = getClockValue();
            delta = getDifferenceTime(currentTime,lastTime) / (f32)performanceFreq;
            Window_dim windowDim = getWindowDim(window);
            
            
            blitBufferToScreen(&gFramebuffer,deviceContext,windowDim.width,windowDim.height);
            
            Input *temp = newInput;
            newInput = oldInput;
            oldInput = temp;
        }      
    }
    return 0;
}
