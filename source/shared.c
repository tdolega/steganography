#include <stdio.h>

#include "shared.h"

int get_bytes(FILE *f, int offset, int size)
{
    int data;
    fseek(f, offset, SEEK_SET);
    fread(&data, size, 1, f);
    return data;
}

void pixel_count(struct Meta *info)
{
    info->width = get_bytes(info->in, HEADER_WIDTH, HEADER_WIDTH_SIZE);
    info->height = get_bytes(info->in, HEADER_HEIGHT, HEADER_HEIGHT_SIZE);
    int count = info->width * info->height;
    printf("%i x %i (%i) pixels \n", info->width, info->height, count);
}

int wrong_depth(struct Meta *info)
{
    if (info->lsbUsed.a > info->colorsUsed.a)
        return 1;
    if (info->lsbUsed.r > info->colorsUsed.r)
        return 1;
    if (info->lsbUsed.g > info->colorsUsed.g)
        return 1;
    if (info->lsbUsed.b > info->colorsUsed.b)
        return 1;
    return 0;
}

void check_padding(struct Meta *info, int colorDepth)
{ // ceil without importing math for this one operation
    int a = colorDepth * info->width;
    int b = 32;
    info->rowSize = (a / b) + ((a % b) != 0);
    info->padding = info->rowSize - (a / b);
}

int validate_in(struct Meta *info)
{
    if (get_bytes(info->in, HEADER_FILE_TYPE, HEADER_FILE_TYPE_SIZE) != CORRECT_FILE_TYPE)
    {
        printf("ERROR: wrong input file, not a correct bitmap \n");
        return 1;
    }
    int headerType = get_bytes(info->in, HEADER_FILE_TYPE, HEADER_FILE_TYPE_SIZE);
    if (headerType != CORRECT_FILE_TYPE)
    {
        printf("ERROR: unsupported header \n");
        if (headerType == BITMAPCOREHEADER)
        {
            printf("Windows 2.0 header in the wild \n");
        }
        return 1;
    }

    // pixels
    int colorDepth = get_bytes(info->in, HEADER_BITS_PER_PIXEL, HEADER_BITS_PER_PIXEL_SIZE);
    printf("%i bit image ", colorDepth);
    switch (colorDepth)
    {
    default:
        printf("ERROR: not supported depth \n");
        return 1;
    case 16:
        info->depth = c16b5551RGB;
        printf("(c16b5551RGB) \n");
        info->colorsUsed.a = 0;
        info->colorsUsed.r = 5;
        info->colorsUsed.g = 5;
        info->colorsUsed.b = 5;
        info->pixelSize = 2;
        break;
    case 24:
        info->depth = c24bTrueColor;
        printf("(c24bTrueColor) \n");
        info->colorsUsed.a = 0;
        info->colorsUsed.r = 8;
        info->colorsUsed.g = 8;
        info->colorsUsed.b = 8;
        info->pixelSize = 3;
        break;
    case 32:
        info->depth = c32bTrueColorRGBA;
        printf("(c32bTrueColorRGBA) \n");
        info->colorsUsed.a = 8;
        info->colorsUsed.r = 8;
        info->colorsUsed.g = 8;
        info->colorsUsed.b = 8;
        info->pixelSize = 4;
        break;
    }

    pixel_count(info);
    check_padding(info, colorDepth);
    return 0;
}

int max_secret(struct Meta *info)
{
    int inputPixels = info->height * info->width;
    int maxSecretSize = inputPixels * info->lsbUsed.r;
    maxSecretSize += inputPixels * info->lsbUsed.g;
    maxSecretSize += inputPixels * info->lsbUsed.b;
    if (info->depth == c32bTrueColorRGBA)
    {
        maxSecretSize += inputPixels * info->lsbUsed.a;
    }
    int maxSecretSizeB = maxSecretSize / 8;
    maxSecretSizeB -= 4; // 4 bytes to encode secret size
    printf("using (%i, %i, %i, %i) LSB from (A, R, G, B) \n", info->lsbUsed.a, info->lsbUsed.r, info->lsbUsed.g, info->lsbUsed.b);
    printf("therfore max possible secret size is %i B \n", maxSecretSizeB);
    return maxSecretSizeB;
}

unsigned separate_return(unsigned dataChunk, unsigned offset, unsigned size)
{
    dataChunk = dataChunk << offset;
    dataChunk = dataChunk >> (32 - size);
    return dataChunk;
}

unsigned separate(struct Meta *info, unsigned dataChunk, char color)
{
    switch (info->depth)
    {
    case c16b5551RGB:
        switch (color)
        {
        case 'r':
            return separate_return(dataChunk, 17, 5);
        case 'g':
            return separate_return(dataChunk, 22, 5);
        case 'b':
            return separate_return(dataChunk, 27, 5);
        }
    case c24bTrueColor:
        switch (color)
        {
        case 'r':
            return separate_return(dataChunk, 8, 8);
        case 'g':
            return separate_return(dataChunk, 16, 8);
        case 'b':
            return separate_return(dataChunk, 24, 8);
        }
    case c32bTrueColorRGBA:
        switch (color)
        {
        case 'r':
            return separate_return(dataChunk, 0, 8);
        case 'g':
            return separate_return(dataChunk, 8, 8);
        case 'b':
            return separate_return(dataChunk, 16, 8);
        case 'a':
            return separate_return(dataChunk, 24, 8);
        }
    }
}