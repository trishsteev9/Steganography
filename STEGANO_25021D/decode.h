#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include <string.h>
#include "types.h"

#define MAGIC_STRING "#*"   // Must match encoding part

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Output (decoded) Secret File Info */
    char secret_fname[20];
    FILE *fptr_secret;

    /* Extracted Extension Info */
    char extn_secret_file[8];
    int extn_size;               // ✅ newly added field to store decoded extension size

    /* Secret File Size Info */
    long size_secret_file;

} DecodeInfo;

/* Function Prototypes */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status open_files_decode(DecodeInfo *decInfo);
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);
Status decode_secret_file_extn_size(DecodeInfo *decInfo);
Status decode_secret_file_extn(DecodeInfo *decInfo);
Status decode_secret_file_size(DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);
Status do_decoding(DecodeInfo *decInfo);

/* Helper Functions */
char decode_byte_from_lsb(char *image_buffer);
int decode_size_from_lsb(char *image_buffer);

#endif
