/*Steganography Encoding includes encoding a secrect code or information encoded in an image and resulting into an encode image
We pass command line argumnets to indicate encode and decode by -e and -d, In particularly in encoding we pass our source image 
where the this image is encoded into an another image with both the image looking alike.
Trisha Steeve
Sample Input- ./a.out beautiful.bmp secret.txt [optional] here beautiful.bmp represents the source image and secret.txt is the information
file and the fourth argument can be the output file which will contain the destinations which can be entered by user or is craeted by default*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encode.h"
#include "types.h"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define RESET   "\033[0m"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    int len_src = strlen(argv[2]);
    if (len_src > 4 && strcmp(argv[2] + len_src - 4, ".bmp") == 0)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        fprintf(stderr, RED"ERROR: Source file must end with .bmp\n"RESET);
        return e_failure;
    }

    int len_secret = strlen(argv[3]);
    if (len_secret > 4 && strcmp(argv[3] + len_secret - 4, ".txt") == 0)//checks if extension is txt
    {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file, ".txt");
    }
    else if (len_secret > 2 && strcmp(argv[3] + len_secret - 2, ".c") == 0)
    {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file, ".c");
    }
    else if (len_secret > 3 && strcmp(argv[3] + len_secret - 3, ".sh") == 0)
    {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file, ".sh");
    }
    else if (len_secret > 4 && strcmp(argv[3] + len_secret - 4, ".pdf") == 0)
    {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file, ".pdf");
    }
    else if (len_secret > 4 && strcmp(argv[3] + len_secret - 4, ".cpp")==0)
    {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file, ".cpp");
    }
    else
    {
        fprintf(stderr, RED"ERROR: Secret file must end with .txt or .c or .sh or .pdf or .cpp\n"RESET);
        return e_failure;
    }

    if (argv[4] != NULL)
    {
        int len_out = strlen(argv[4]);
        if (len_out > 4 && strcmp(argv[4] + len_out - 4, ".bmp") == 0)
        {
            encInfo->stego_image_fname = argv[4];
        }
        else
        {
            fprintf(stderr, RED"ERROR: Output file must end with .bmp\n"RESET);
            return e_failure;
        }
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }

    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, RED"ERROR: Unable to open file %s\n"RESET, encInfo->src_image_fname);
        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, RED"ERROR: Unable to open file %s\n"RESET, encInfo->secret_fname);
        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, RED"ERROR: Unable to open file %s\n"RESET, encInfo->stego_image_fname);
        return e_failure;
    }

    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    if ((encInfo->image_capacity > (strlen(MAGIC_STRING) * 8) + 32 + (strlen(encInfo->extn_secret_file) * 8) + 32 + (encInfo->size_secret_file * 8)))
    {
        return e_success;
    }
    return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    unsigned char header[54];
    int src_pos;
    int dest_pos;
    rewind(fptr_src_image);
    fread(header, 1, 54, fptr_src_image);
    fwrite(header, 1, 54, fptr_dest_image);
    src_pos = ftell(fptr_src_image);
    dest_pos = ftell(fptr_dest_image);
    if (src_pos == dest_pos && src_pos == 54)
    {
          printf("Offset validation passed: src = %d, dest = %d\n", src_pos, dest_pos);
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char image_buffer[8];
    int src, dest;
    for (int i = 0; i < 2; i++)
    {
        fread(image_buffer, 1, 8, encInfo->fptr_src_image);
        if (encInfo->fptr_src_image == NULL)
        {
            return e_failure;
        }
        encode_byte_to_lsb(magic_string[i], image_buffer);
        fwrite(image_buffer, 1, 8, encInfo->fptr_stego_image);
    }
    src = ftell(encInfo->fptr_src_image);
    dest = ftell(encInfo->fptr_stego_image);
    if ((src && dest) == 70)
    {
        printf("Offset validation passed: src = %d, dest = %d\n", src, dest);
        return e_success;
    }
    return e_failure;
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char image_buffer[32];
    int srcoff;
    int destoff;
    fread(image_buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(size, image_buffer);
    fwrite(image_buffer, 1, 32, encInfo->fptr_stego_image);
    srcoff = ftell(encInfo->fptr_src_image);
    destoff = ftell(encInfo->fptr_stego_image);
    if (srcoff == destoff)
    {
          printf("Offset validation passed: src = %d, dest = %d\n", srcoff, destoff);
        return e_success;
    }
    else
    {
        printf(RED"Unable to copy the size\n"RESET);
        return e_failure;
    }
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char image_buffer[8];
    int src, dest;
    int s = strlen(file_extn);
    for (int i = 0; i < s; i++)
    {
        fread(image_buffer, 1, 8, encInfo->fptr_src_image);
        if (encInfo->fptr_src_image == NULL)
        {
            return e_failure;
        }
        encode_byte_to_lsb(file_extn[i], image_buffer);
        fwrite(image_buffer, 1, 8, encInfo->fptr_stego_image);
    }
    src = ftell(encInfo->fptr_src_image);
    dest = ftell(encInfo->fptr_stego_image);
    if (src == dest)
    {
        printf("Offset validation passed: src = %d, dest = %d\n", src, dest);
        return e_success;
    }
    return e_failure;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char image_buffer[32];
    if (fread(image_buffer, 1, 32, encInfo->fptr_src_image) != 32)
    {
        printf(RED"ERROR: Unable to read 32 bytes from source image.\n"RESET);
        return e_failure;
    }
    encode_size_to_lsb((int)file_size, image_buffer);
    if (fwrite(image_buffer, 1, 32, encInfo->fptr_stego_image) != 32)
    {
        printf(RED"ERROR: Unable to write encoded size to stego image.\n"RESET);
        return e_failure;
    }

    int src_pos = ftell(encInfo->fptr_src_image);
    int dest_pos = ftell(encInfo->fptr_stego_image);
    if (src_pos == dest_pos)
    {
        printf("Offset validation passed: src = %d, dest = %d\n", src_pos, dest_pos);
        return e_success;
    }
    return e_failure;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char *secret_data = malloc(encInfo->size_secret_file);
    if (!secret_data)
    {
        printf(RED"ERROR: Memory allocation failed.\n"RESET);
        return e_failure;
    }

    rewind(encInfo->fptr_secret);
    if (fread(secret_data, 1, encInfo->size_secret_file, encInfo->fptr_secret) != encInfo->size_secret_file)
    {
        printf(RED"ERROR: Unable to read entire secret file into memory.\n"RESET);
        free(secret_data);
        return e_failure;
    }

    char image_buffer[8];
    for (long i = 0; i < encInfo->size_secret_file; i++)
    {
        if (fread(image_buffer, 1, 8, encInfo->fptr_src_image) != 8)
        {
            printf(RED"ERROR: Unable to read 8 bytes from source image.\n"RESET);
            free(secret_data);
            return e_failure;
        }
        encode_byte_to_lsb(secret_data[i], image_buffer);
        if (fwrite(image_buffer, 1, 8, encInfo->fptr_stego_image) != 8)
        {
            printf(RED"ERROR: Unable to write 8 encoded bytes.\n"RESET);
            free(secret_data);
            return e_failure;
        }
    }

    free(secret_data);
    long src_pos = ftell(encInfo->fptr_src_image);
    long dest_pos = ftell(encInfo->fptr_stego_image);
    if (src_pos == dest_pos)
    {
        printf("Offset validation passed: src = %ld, dest = %ld\n", src_pos, dest_pos);
        return e_success;
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char buffer[1024];
    while (!feof(fptr_src))
    {
        size_t n = fread(buffer, 1, sizeof(buffer), fptr_src);
        if (n > 0)
            fwrite(buffer, 1, n, fptr_dest);
    }

    long src_offset = ftell(fptr_src);
    long dest_offset = ftell(fptr_dest);

    if (src_offset == dest_offset)
    {
         printf("Offset validation passed: src = %ld, dest = %ld\n", src_offset, dest_offset);
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] = image_buffer[i] & (~1);
        int bit = (data >> (7 - i)) & 1;
        image_buffer[i] = image_buffer[i] | bit;
    }
    return e_success;
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    for (int i = 0; i < 32; i++)
    {
        imageBuffer[i] = imageBuffer[i] & (~1);
        int bit = (size >> (31 - i)) & 1;
        imageBuffer[i] |= bit;
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_success)
    {
        printf("All the files are opened to perform operations:\n");
    }

    if (check_capacity(encInfo) == e_success)
    {
        printf("The capacity is validated:\n");
    }

    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("Header is copied Successfully\n");
    }

    if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
    {
        printf("Magic string is encoded\n");
    }

    char *extn = strstr(encInfo->secret_fname, ".");
    if (extn != NULL)
    {
        strcpy(encInfo->extn_secret_file, extn);
    }
    else
    {
        printf("No extension found in secret file.\n");
    }

    int s = strlen(encInfo->extn_secret_file);
    if (encode_secret_file_extn_size(s, encInfo) == e_success)
    {
        printf("Secret file extension size copied\n");
    }
    else
    {
        printf(RED"Failed to encode secret file extension size!!\n"RESET);
        return e_failure;
    }

    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) != e_success)
    {
        printf(RED"ERROR: Failed to encode secret file extension.\n"RESET);
        return e_failure;
    }
    else
    {
        printf("Secret file extension encoded successfully.\n");
    }

    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) != e_success)
    {
        printf(RED"ERROR: Encoding secret file size failed.\n"RESET);
        return e_failure;
    }
    printf("Secret file size encoded successfully.\n");

    if (encode_secret_file_data(encInfo) == e_success)
    {
        printf("Secret file data is encoded\n");
    }

    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) != e_success)
    {
        printf(RED"ERROR: Copying remaining image data failed.\n"RESET);
        return e_failure;
    }

    printf(GREEN"Remaining image data copied successfully.\n"RESET);
    return e_success;
}