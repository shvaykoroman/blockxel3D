#ifndef PNG
#define PNG

#pragma pack(push,1)

#define FOURCC(string) (((u32)string[0] << 0) | ((u32)string[1] << 8) | ((u32)string[2] << 16) | ((u32)string[3] << 24))

struct bmp_header
{
    u16 fileType;
    u32 fileSize;
    u16 reserved1;
    u16 reserved2;
    u32 bitmapOffset;
    u32 size;            /* Size of this header in bytes */
    s32 width;           /* Image width in pixels */
    s32 height;          /* Image height in pixels */
    u16 planes;          /* Number of color planes */
    u16 bitsPerPixel;    /* Number of bits per pixel */
    u32 compression;     /* Compression methods used */
    u32 sizeOfBitmap;    /* Size of bitmap in bytes */
    s32 horzResolution;  /* Horizontal resolution in pixels per meter */
    s32 vertResolution;  /* Vertical resolution in pixels per meter */
    u32 colorsUsed;      /* Number of colors in the image */
    u32 colorsImportant; /* Minimum number of important colors */
};



struct png_header
{
    u8 signature[8];
};

struct png_chunk_header
{
    u32 length;
    union
    {
        u32 u32Type;
        char type[4];
    };
    
};

struct ihdr_chunk
{
    u32 width;
    u32 height;
    u8  bitDepth;
    u8  colorType;
    u8  compressionMethod;
    u8  filterMethod;
    u8  interlaceMethod;
};

struct png_idat_header
{
    u8 zLibMethodFlags;
    u8 additionalFlags;
    
};

struct png_idat_footer
{
    u32 checkValue;
};

struct png_chunk_footer
{
    u32 CRC; 
};

#pragma pack(pop)


struct streaming_chunk
{
    u32 size;
    void *memory;
    
    streaming_chunk *next;
};


struct streaming_buffer
{
    u32 size;
    void *memory;
    
    u32 bitBuf;
    u32 bitCount;
    
    bool underflow;
    
    streaming_chunk *last;
    streaming_chunk *first;
};


struct png_huffman_entry
{
    u16 symbol;
    u16 bitsUsed;
};

struct png_huffman
{
    u32 maxCodeLengthBits;
    u32 entryCount;
    
    png_huffman_entry *entries;
};

struct image_out_info
{
    u32 width;
    u32 height;
    
    void *memory;
};

#endif //PNG
