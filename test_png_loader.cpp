#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <memory.h>
#include <assert.h>

#include "png_reader.cpp"

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float  f32;
typedef double f64;


u32
swapAB(u32 c)
{
    u32 result = ((c & 0xFF00FF00) |
                  ((c >> 16)&0xFF) | 
                  ((c & 0xFF) << 16));
    return result;
}


void
writeImage(u8 *pixels,u32 width, u32 height,char *outputFileName)
{
    
    u32 outputPixelSize = 4*width*height;
    
    bmp_header header = {};
    
    header.fileType = 0x4D42;
    header.fileSize = sizeof(header) + outputPixelSize;
    header.bitmapOffset = sizeof(header);
    header.size = sizeof(header) - 14;
    header.width = width;
    header.height = height;
    header.planes = 1;
    header.bitsPerPixel = 32;
    header.compression = 0;
    header.sizeOfBitmap = outputPixelSize;
    header.horzResolution = 0;
    header.vertResolution = 0;
    header.colorsUsed = 0;
    header.colorsImportant = 0;
    
    
    u32 centerPoint = (header.height + 1) / 2;
    u32 *row0 = (u32*)pixels;
    u32 *row1 = row0 + ((height - 1) * width);
    
    // RGBA -> -> ->
    // BGRA
    for(u32 y = 0; y < centerPoint; y++)
    {
        u32 *pix0 = row0;
        u32 *pix1 = row1;
        for(u32 x = 0; x < width; x++)
        {
            u32 c0 = swapAB(*pix0);
            u32 c1 = swapAB(*pix1);
            
            *pix0++ = c1;
            *pix1++ = c0;
        }
        row0 += width;
        row1 -= width;
    }
    
    FILE *file = fopen(outputFileName, "wb");
    if(file)
    {
        fwrite(&header, sizeof(header),1,file);
        fwrite(pixels, outputPixelSize,1,file);
        fclose(file);
    }
    else
    {
        fprintf(stderr, "[ERROR] Unable to write output file %s.\n", outputFileName);
    }
    
}


int main(int argCount, char *argv[])
{
    if(argCount == 3) 
    {
        Memory_arena arena;
        char *inFileName =  (char*)argv[1];
        char *outFileName =  (char*)argv[2];
        fprintf(stdout, "file %s is loading...:\n",inFileName);
        
        streaming_buffer fileContent = readEntireFile(inFileName);
        u32 width = 0;
        u32 height = 0;
        u8 *data = parsingPNG(&arena,&fileContent, &width, &height);
        writeImage(data,width,height,outFileName);
    }
    else
    {
        fprintf(stderr, "require file\n");
    }
    
    return 0;
}