#include <stdio.h>

#include "shared.h"
#include "decoding.h"

int copy_next_bit(struct Meta *info, unsigned secretBit)
{
    if (info->secretSizeEncoded)
    {
        if (info->secredHeadPosition > 7) // all bits from current chunk were readed
        {
            fputc(info->secretChunk, info->out);
            info->secredHeadPosition = 0;
            int currentSize = ftell(info->out);
            if (currentSize >= info->secretSize)
                return 1;
        }
    }
    info->secretChunk ^= (-!!secretBit ^ info->secretChunk) & (1UL << info->secredHeadPosition);

    info->secredHeadPosition++;
    if (info->secredHeadPosition == 31)
    {
        info->secretSizeEncoded = 1;
        info->secretSize = info->secretChunk;
        info->secretChunk = 0;
        info->secredHeadPosition = 0;
        printf("secret size is %i B \n", info->secretSize);
    }
    return 0;
}

int decode_sub(struct Meta *info, unsigned subChunk, char color)
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
        unsigned secretBit = (subChunk & (1 << pos));
        if (copy_next_bit(info, secretBit))
        {
            return 1;
        }
    }
    return 0;
}

int decode_data(struct Meta *info)
{
    int headerSize = get_bytes(info->in, HEADER_DATA_LOCATION, HEADER_DATA_LOCATION_SIZE);
    fseek(info->in, headerSize, SEEK_SET);

    unsigned dataChunk, subChunk;
    char colors[] = {'a', 'r', 'g', 'b'};
    int apos = 1;
    int rowPos;
    info->secredHeadPosition = 0;
    info->secretSize = 4;
    info->secretChunk = 0;
    info->secretSizeEncoded = 0;

    while (fread(&dataChunk, 1, info->pixelSize, info->in) == info->pixelSize) // not EOF
    {
        unsigned newChunk = 0;
        for (int c = 0; c < 4; c++)
        {
            subChunk = separate(info, dataChunk, colors[c]);
            if (decode_sub(info, subChunk, colors[c]))
            {
                return 0;
            }
        }
        rowPos = apos % info->width;
        if (rowPos == 0)
        {
            fread(&dataChunk, 1, info->padding, info->in);
        }
        apos++;
    }
}

void decode_settings(struct Meta *info)
{
    fseek(info->in, HEADER_RESERVED4B, SEEK_SET);
    // each color is set max to 8 (4 bits)
    unsigned encodedLsbUsed;
    fread(&encodedLsbUsed, 2, 1, info->in);
    info->lsbUsed.r = separate_return(encodedLsbUsed, 28, 4);
    info->lsbUsed.g = separate_return(encodedLsbUsed, 24, 4);
    info->lsbUsed.b = separate_return(encodedLsbUsed, 20, 4);
    info->lsbUsed.a = separate_return(encodedLsbUsed, 16, 4);
}

int decode(char *outputSecret, char *inputBmp, struct Meta *info)
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
    decode_settings(info);
    int maxSecretSizeB = max_secret(info);
    // if (!maxSecretSizeB)
    // {
    //     fclose(info->in);
    //     return 1;
    // }
    // int percentUsed = info->secretSize * 100 / maxSecretSizeB;
    // printf("secret size is %i B (%i%% of max)\n", info->secretSize, percentUsed);
    // if (info->secretSize > maxSecretSizeB)
    // {
    //     printf("ERROR: file is not encoded correctly (secret size) \n");
    //     return 1;
    // }
    if (wrong_depth(info))
    {
        printf("ERROR: input file was not encoded properly (depth settings) \n");
        return 1;
    }
    // return 0;

    info->out = fopen(outputSecret, "w");
    if (!info->out)
    {
        printf("ERROR: opening -s \n");
        fclose(info->in);
        return 1;
    }

    decode_data(info);

    fclose(info->in);
    fclose(info->out);
    return 0;
}
