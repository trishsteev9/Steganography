#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

// Color codes for terminal output
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define RESET "\x1B[0m"

// Function to check operation type (-e for encode, -d for decode)
OperationType check_operation_type(char *symbol);

int main(int argc, char *argv[])
{
    // Check if enough arguments are provided
    if (argc < 3)
    {
        // Display usage message
        printf("Usage:\n");
        printf(RED"  Encoding: ./stego.out -e <src.bmp> <secret.txt> [output.bmp]\n"RESET);
        printf(RED"  Decoding: ./stego.out -d <stego.bmp> [output_name]\n"RESET);
        return 1;
    }

    // Identify the operation type (encode or decode)
    OperationType op_type = check_operation_type(argv[1]);

    switch (op_type)
    {
        // Encoding process
        case e_encode:
        {
            // Check argument count for encoding
            if (argc >= 4 && argc <= 5)
            {
                EncodeInfo encInfo;

                // Validate encoding arguments
                if (read_and_validate_encode_args(argv, &encInfo) == e_success)
                {
                    // Perform encoding
                    if (do_encoding(&encInfo) == e_success)
                        printf(GREEN "\nEncoding completed successfully: %s\n" RESET, 
                               encInfo.stego_image_fname);
                    else
                        printf(RED "\nERROR: Encoding failed!\n" RESET);
                }
                else
                {
                    printf(RED "ERROR: Validation failed for encoding.\n" RESET);
                }
            }
            else
            {
                // Incorrect usage for encoding
                printf(RED "Usage: ./stego.out -e <src.bmp> <secret.txt> [output.bmp]\n" RESET);
            }
            break;
        }

        // Decoding process
        case e_decode:
        {
            // Check argument count for decoding
            if (argc >= 3 && argc <= 4)
            {
                DecodeInfo decInfo;

                // Validate decoding arguments
                if (read_and_validate_decode_args(argv, &decInfo) == e_success)
                {
                    // Perform decoding
                    if (do_decoding(&decInfo) == e_success)
                        printf(GREEN "\nDecoding completed successfully: %s\n" RESET, 
                               decInfo.secret_fname);
                    else
                        printf(RED "\nERROR: Decoding failed!\n" RESET);
                }
                else
                {
                    printf(RED "ERROR: Validation failed for decoding.\n" RESET);
                }
            }
            else
            {
                // Incorrect usage for decoding
                printf(RED "Usage: ./stego.out -d <stego.bmp> [output_name]\n" RESET);
            }
            break;
        }

        // Unsupported operation type
        default:
            printf(RED "ERROR: Unsupported operation: %s\n" RESET, argv[1]);
            printf("Use -e for encoding or -d for decoding.\n");
            break;
    }

    return 0;
}

// Function to determine the type of operation based on user input
OperationType check_operation_type(char *symbol)
{
    if (strcmp(symbol, "-e") == 0)
        return e_encode;        // Encoding
    else if (strcmp(symbol, "-d") == 0)
        return e_decode;        // Decoding
    else
        return e_unsupported;   // Invalid option
}
