/*  
Steganography Decoding involves extracting the hidden secret information that was previously encoded inside an image.  
We pass command line arguments to indicate decode operation using -d.  
In decoding, we provide the stego image (the encoded image) from which the secret message is retrieved.  
The extracted information is then written into an output text file, which can either be provided by the user or is created by default.  

Sample Input - ./a.out -d encoded.bmp [optional_output.txt]  
Here, encoded.bmp represents the image that contains the hidden data,  
and the optional argument specifies the name of the output text file where the decoded secret message will be saved.  
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define RESET "\x1B[0m"
/* Read and validate decode arguments */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Check if stego image has .bmp extension
    int len = strlen(argv[2]);
    if (len <= 4 || strcmp(argv[2] + len - 4, ".bmp") != 0)
    {
        printf(RED "ERROR: Stego image file must end with .bmp\n" RESET);
        return e_failure;
    }

    decInfo->stego_image_fname = argv[2];

    // Handle optional output filename
    if (argv[3] != NULL)
    {
        char temp_name[50];
        strcpy(temp_name, argv[3]);
        char *token = strtok(temp_name, ".");
        strcpy(decInfo->secret_fname, token);  // base name only
    }
    else
    {
        strcpy(decInfo->secret_fname, "decoded"); // default output
    }

    return e_success;
}

/* Open stego image file for decoding */
Status open_files_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        printf(RED "ERROR: Unable to open stego image file %s\n" RESET, decInfo->stego_image_fname);
        return e_failure;
    }
    printf(GREEN "Opened stego image file successfully.\n" RESET);
    return e_success;
}

/* Decode 1 byte (8 bits) from 8 LSBs of image data */
char decode_byte_from_lsb(char *image_buffer)
{
    char data = 0;
    for (int i = 0; i < 8; i++)
    {
        data = (data << 1) | (image_buffer[i] & 1);
    }
    return data;
}

/* Decode 4 bytes (32 bits) integer from 32 LSBs */
int decode_size_from_lsb(char *image_buffer)
{
    int size = 0;
    for (int i = 0; i < 32; i++)
    {
        size = (size << 1) | (image_buffer[i] & 1);
    }
    return size;
}

/* Step 1: Verify Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char image_buffer[8];
    char decoded_magic[50];
    int i;

    printf("Decoding magic string starting at offset 54...\n");

    for (i = 0; i < strlen(magic_string); i++)
    {
        fread(image_buffer, 1, 8, decInfo->fptr_stego_image);
        decoded_magic[i] = decode_byte_from_lsb(image_buffer);
    }
    decoded_magic[i] = '\0';

    if (strcmp(decoded_magic, magic_string) != 0)
    {
        printf(RED "ERROR: Magic string mismatch! Hidden data not found.\n" RESET);
        return e_failure;
    }

    printf(GREEN "Magic string verified successfully: \"%s\"\n" RESET, decoded_magic);
    return e_success;
}

/* Step 2: Decode secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, decInfo->fptr_stego_image);

    decInfo->extn_size = decode_size_from_lsb(buffer);

    printf("Decoded secret file extension size: %d\n", decInfo->extn_size);
    printf("Offset after decoding extension size: %ld\n", ftell(decInfo->fptr_stego_image));

    return e_success;
}

/* Step 3: Decode secret file extension (.txt, .c, etc.) */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char image_buffer[8];
    int size = decInfo->extn_size;
    char decoded_extn[size + 1];

    printf("Decoding secret file extension of size %d...\n", size);

    for (int i = 0; i < size; i++)
    {
        fread(image_buffer, 1, 8, decInfo->fptr_stego_image);
        decoded_extn[i] = decode_byte_from_lsb(image_buffer);
    }

    decoded_extn[size] = '\0';
    strcpy(decInfo->extn_secret_file, decoded_extn);

    // Combine base filename and extension
    sprintf(decInfo->secret_fname, "%s%s", decInfo->secret_fname, decoded_extn);

    // Open output file for decoded data
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    if (decInfo->fptr_secret == NULL)
    {
        printf(RED "ERROR: Unable to create output secret file.\n" RESET);
        return e_failure;
    }

    printf(GREEN "Secret file extension decoded: %s\n" RESET, decoded_extn);
    printf(GREEN "Created output file: %s\n" RESET, decInfo->secret_fname);
    printf("Offset after decoding extension: %ld\n", ftell(decInfo->fptr_stego_image));

    return e_success;
}

/* Step 4: Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, decInfo->fptr_stego_image);
    decInfo->size_secret_file = decode_size_from_lsb(buffer);

    printf("Decoded secret file size: %ld bytes\n", decInfo->size_secret_file);
    printf("Offset after decoding file size: %ld\n", ftell(decInfo->fptr_stego_image));

    return e_success;
}

/* Step 5: Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    printf("Starting secret data decoding...\n");

    long size = decInfo->size_secret_file;
    char *decoded_data = (char *)malloc(size + 1);
    if (!decoded_data)
    {
        printf(RED "ERROR: Memory allocation failed for decoded data.\n" RESET);
        return e_failure;
    }

    char image_buffer[8];
    for (long i = 0; i < size; i++)
    {
        fread(image_buffer, 1, 8, decInfo->fptr_stego_image);
        decoded_data[i] = decode_byte_from_lsb(image_buffer);
    }

    decoded_data[size] = '\0';
    fwrite(decoded_data, 1, size, decInfo->fptr_secret);

    printf(GREEN "Decoded secret file data successfully.\n" RESET);
    printf("Final offset after decoding: %ld\n", ftell(decInfo->fptr_stego_image));

    fclose(decInfo->fptr_secret);
    free(decoded_data);

    return e_success;
}

/* Main decoding driver */
Status do_decoding(DecodeInfo *decInfo)
{
    if (open_files_decode(decInfo) != e_success)
    {
        fprintf(stderr, RED "Failed to open stego image file. Aborting decoding.\n" RESET);
        return e_failure;
    }

    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    printf("Skipped BMP header. Current offset: %ld\n", ftell(decInfo->fptr_stego_image));

    if (decode_magic_string(MAGIC_STRING, decInfo) != e_success)
        return e_failure;

    if (decode_secret_file_extn_size(decInfo) != e_success)
        return e_failure;

    if (decode_secret_file_extn(decInfo) != e_success)
        return e_failure;

    if (decode_secret_file_size(decInfo) != e_success)
        return e_failure;

    if (decode_secret_file_data(decInfo) != e_success)
        return e_failure;

    printf(GREEN "Decoding completed successfully. Output file: %s\n" RESET, decInfo->secret_fname);
    return e_success;
}
