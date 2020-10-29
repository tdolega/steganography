#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "shared.h"
#include "decoding.h"
#include "encoding.h"

void help()
{
    printf("Usage: steganography [MODE] [OPTIONS]... \n");
    printf("       steganography -e -i INPUT_BMP -o OUTPUT_BMP -s SECRET_FILE\n");
    printf("       steganography -d -i INPUT_BMP -o OUTPUT_SECRET \n\n");
    printf("Hide any file into BMP file. \n\n");
    printf("MODES: \n");
    printf("  -d \t\t decode \n");
    printf("  -e \t\t encode \n\n");
    printf("OPTIONS: \n");
    printf("  -o \t\t output file \n");
    printf("  -i \t\t input file \n\n");
    printf("encoding only: \n");
    printf("  -s \t\t secret file to encode\n\n");
    printf("custom number of bits to use for encoding: \n");
    printf("  -r \t\t bits of red \n");
    printf("  -g \t\t bits of green  \n");
    printf("  -b \t\t bits of blue \n");
    printf("  -a \t\t bits of alpha (32 bit only) \n");
    printf("default values when none provided are (0, 2, 2, 2) -> (A, R, G, B) \n");
    printf("\n\nTymoteusz Dolega 2020 \n");
    exit(0);
}

int main(int argc, char *argv[])
{
    char *output, *bmpInput, *secretInput;
    enum Mode
    {
        DECODE,
        ENCODE
    } selectedMode;
    struct Options selected = {};
    struct Meta *info = malloc(sizeof(struct Meta));
    // default values
    info->lsbUsed.r = 2;
    info->lsbUsed.g = 2;
    info->lsbUsed.b = 2;
    info->lsbUsed.a = 0; // because it's easy to spot

    while ((++argv)[0])
    {
        if (argv[0][0] == '-')
        {
            switch (argv[0][1])
            {
            default:
                printf("Unknown option -%c \n\n", argv[0][1]);
            case 'h':
                help();

            case 'e':
                selectedMode = ENCODE;
                break;
            case 'd':
                selectedMode = DECODE;
                break;

            case 'o':
                output = (++argv)[0];
                selected.o = 1;
                break;
            case 'i':
                bmpInput = (++argv)[0];
                selected.i = 1;
                break;
            case 's':
                secretInput = (++argv)[0];
                selected.s = 1;
                break;

            case 'r':
                info->lsbUsed.r = (int)strtol((++argv)[0], NULL, 10);
                break;
            case 'g':
                info->lsbUsed.g = (int)strtol((++argv)[0], NULL, 10);
                break;
            case 'b':
                info->lsbUsed.b = (int)strtol((++argv)[0], NULL, 10);
                break;
            case 'a':
                info->lsbUsed.a = (int)strtol((++argv)[0], NULL, 10);
                break;
            }
        }
    }
    switch (selectedMode)
    {
    default:
        printf("ERROR: please select MODE \n");
        printf("try -h for help \n");
        free(info);
        return 1;
    case DECODE:
        if (selected.o && selected.i)
        {
            printf("MODE: decoding \n");
            if (decode(output, bmpInput, info) == 0)
            {
                printf("success :) \n");
            }
            else
            {
                printf("ERROR... \n");
            }
        }
        else
        {
            printf("ERROR: please provide all needed parameters \n");
        }
        break;
    case ENCODE:
        if (selected.o && selected.i && selected.s)
        {
            printf("MODE: encoding \n");
            if (encode(output, bmpInput, secretInput, info) == 0)
            {
                printf("success :) \n");
            }
            else
            {
                printf("ERROR... \n");
            }
        }
        else
        {
            printf("ERROR: please provide all needed parameters \n");
        }
        break;
    }
    free(info);
    return 0;
}