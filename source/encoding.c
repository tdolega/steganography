#include <stdio.h>
#include <stdlib.h>

#include "shared.h"
#include "encoding.h"

int get_file_size(FILE *f)
{
    fseek(f, 0, SEEK_END); // seek to end of file
    int size = ftell(f);   // get current file pointer
    rewind(f);
    return size;
}

void copy_header(struct Meta *info)
{
    int headerSize = get_bytes(info->in, HEADER_DATA_LOCATION, HEADER_DATA_LOCATION_SIZE);
    rewind(info->in);
    rewind(info->out);
    int *header = malloc(headerSize);
    fread(header, headerSize, 1, info->in);
    fwrite(header, headerSize, 1, info->out);
    free(header);
    // no need to rewind
}

unsigned get_next_bit(struct Meta *info)
{
    if (info->secretSizeEncoded)
    {
        // return 1;
        if (info->secredHeadPosition > 7) // all bits from current chunk were readed
        {
            info->secretChunk = fgetc(info->secretIn);
            info->secredHeadPosition = 0;
        }
    }
    if (info->secretChunk == EOF)
    {
        // info->secretEOF = 1;
        return -1;
    }
    unsigned bit = ((info->secretChunk >> info->secredHeadPosition) & 1);
    info->secredHeadPosition++;
    if (info->secredHeadPosition == 31)
    {
        info->secretSizeEncoded = 1;
    }
    return bit;
}

unsigned encode_sub(struct Meta *info, unsigned subChunk, char color)
{
    int lsbUsed = 0;
    switch (color)
    {
    case 'r':
        lsbUsed = info->lsbUsed.r;
        break;
    case 'g':
        lsbUsed = info->lsbUsed.g;
        break;
    case 'b':
        lsbUsed = info->lsbUsed.b;
        break;
    case 'a':
        lsbUsed = info->lsbUsed.a;
        break;
    }
    for (int pos = 0; pos < lsbUsed; pos++) // next data bit
    {
        int secretBit = get_next_bit(info);
        if (secretBit != -1) // secret is not EOF
        {
            subChunk ^= (-secretBit ^ subChunk) & (1UL << pos);
        }
    }
    return subChunk;
}

unsigned merge_return(unsigned newChunk, unsigned subChunk, unsigned moveLeft)
{
    return newChunk | (subChunk << moveLeft);
}

unsigned merge(struct Meta *info, unsigned newChunk, unsigned subChunk, char color)
{
    switch (info->depth)
    {
    case c16b5551RGB:
        switch (color)
        {
        case 'r':
            return merge_return(newChunk, subChunk, 10);
        case 'g':
            return merge_return(newChunk, subChunk, 5);
        case 'b':
            return merge_return(newChunk, subChunk, 0);
        case 'a':
            return newChunk;
        }
    case c24bTrueColor:
        switch (color)
        {
        case 'r':
            return merge_return(newChunk, subChunk, 16);
        case 'g':
            return merge_return(newChunk, subChunk, 8);
        case 'b':
            return merge_return(newChunk, subChunk, 0);
        case 'a':
            return newChunk;
        }
    case c32bTrueColorRGBA:
        switch (color)
        {
        case 'r':
            return merge_return(newChunk, subChunk, 24);
        case 'g':
            return merge_return(newChunk, subChunk, 16);
        case 'b':
            return merge_return(newChunk, subChunk, 8);
        case 'a':
            return merge_return(newChunk, subChunk, 0);
        }
    }
    return newChunk;
}

int encode_data(struct Meta *info)
{
    unsigned dataChunk, subChunk;
    char colors[] = {'a', 'r', 'g', 'b'};
    int apos = 1;
    int rowPos;
    info->secredHeadPosition = 0;
    // info->secretEOF = 0; // needed to end looping faster

    info->secretSizeEncoded = 0;
    info->secretChunk = info->secretSize;

    while (fread(&dataChunk, 1, info->pixelSize, info->in) == info->pixelSize) // not EOF
    {
        unsigned newChunk = 0;
        for (int c = 0; c < 4; c++)
        {
            subChunk = separate(info, dataChunk, colors[c]);
            subChunk = encode_sub(info, subChunk, colors[c]);
            newChunk = merge(info, newChunk, subChunk, colors[c]);
        }
        fwrite(&newChunk, 1, info->pixelSize, info->out);
        rowPos = apos % info->width;
        if (rowPos == 0)
        {
            fread(&dataChunk, 1, info->padding, info->in);
            fwrite(&dataChunk, 1, info->padding, info->out);
        }
        apos++;
    }
}

void encode_settings(struct Meta *info)
{
    fseek(info->out, HEADER_RESERVED4B, SEEK_SET);
    int encodedLsbUsed;
    // each color is set max to 8 (4 bits)
    encodedLsbUsed = info->lsbUsed.r;
    encodedLsbUsed += info->lsbUsed.g * 16;
    encodedLsbUsed += info->lsbUsed.b * 16 * 16;
    encodedLsbUsed += info->lsbUsed.a * 16 * 16 * 16;
    fwrite(&encodedLsbUsed, 2, 1, info->out);
}

int encode(char *outputBmp, char *inputBmp, char *inputSecret, struct Meta *info)
{
    // open input BMP
    info->in = fopen(inputBmp, "r");
    if (!info->in)
    {
        printf("ERROR: opening -i \n");
        return 1;
    }
    if (validate_in(info))
    {
        fclose(info->in);
        return 1;
    }
    if (wrong_depth(info))
    {
        printf("ERROR: depth to encode larger than avaiable \n");
        return 1;
    }

    int maxSecretSizeB = max_secret(info);

    // secret
    info->secretIn = fopen(inputSecret, "r");
    if (!info->secretIn)
    {
        printf("ERROR: opening -s \n");
        fclose(info->in);
        return 1;
    }
    info->secretSize = get_file_size(info->secretIn);
    int percentUsed = info->secretSize * 100 / maxSecretSizeB;
    printf("secret size is %i B (%i%% of max)\n", info->secretSize, percentUsed);
    if (info->secretSize > maxSecretSizeB)
    {
        printf("ERROR: secret is too big \n");
        fclose(info->in);
        fclose(info->secretIn);
        return 1;
    }

    // output
    info->out = fopen(outputBmp, "w");
    if (!info->out)
    {
        printf("ERROR: opening -o \n");
        fclose(info->in);
        fclose(info->secretIn);
        return 1;
    }
    copy_header(info); // pointers are now pointing just after header
    encode_data(info);
    encode_settings(info);

    fclose(info->in);
    fclose(info->secretIn);
    fclose(info->out);
    return 0;
}
