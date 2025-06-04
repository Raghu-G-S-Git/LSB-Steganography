/*
    Name        : Raghu G S
    Date        : 22-05-2025
    Description : This project implements Least Significant Bit (LSB) Steganography, a technique used to hide secret data within digital images.
                By modifying the least significant bits of image pixels, the hidden data remains visually undetectable. The project includes 
                encoding and decoding functionalities, allowing users to securely embed and retrieve messages or files in image formats such 
                as PNG or BMP.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;

    //Validate input arguments count 
    if(!(argc >= 3 && argc <= 4))
    {
        error_msg();
    }
    

    //Check for operation type from CLA
    if(check_operation_type(argv) == e_encode)
    {

        if((argc == 4) && (strstr(argv[3],".") != NULL) && (strstr(argv[2],".bmp") != NULL)) //Validate file type 
        {
            encInfo.src_image_fname = argv[2];
            encInfo.secret_fname = argv[3];
            encInfo.stego_image_fname = "stego_img.bmp";
        }
        else
        {
            error_msg();
        }

        // Open files and valiadte
        if (open_files(&encInfo) == e_failure)
        {
            printf("ERROR: %s function failed\n", "open_files" );
            return 1;
        }
        else
        {
            printf("INFO: Done\n");
            sleep(1);
        }

        encInfo.size_secret_file = get_file_size(encInfo.fptr_secret); // Get secret.txt file size
        printf("INFO: ## Checking for capacity of source file ##\n");
        sleep(1);
        if(check_capacity(&encInfo) == e_success)   //Check capacity of source image to store secret.txt data 
        {
            printf("INFO: Done. It has Capacity\n");
        }
        else
        {
            printf("INFO: Done. It has No Capacity\nExiting...\n");
            exit(0);
        }
        
        do_encoding(&encInfo);  //Call Encoding function

    }
    else if(check_operation_type(argv) == e_decode)
    {

        if((argc == 3) && (strstr(argv[2],".bmp") != NULL)) //Validate the file type
        {
            encInfo.stego_image_fname = argv[2];
        }
        else
        {
            error_msg();
        }

        do_decoding(&encInfo); //Call Decodeing function

    }
    else
    {
        error_msg();
    }
    

    return 0;
}

//Function to check operation type
OperationType check_operation_type(char *argv[])
{
    
    if(strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1] ,"-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }

}

void error_msg(void)
{
    printf("\n***********************************************************\n");
    printf("ERROR : INVALID ARGUMENTS.\n\nUSAGE:\n");
    printf("To ENCODE pass like : ./a.out -e <.bmp_file> <.txt_file> \n");
    printf("To DECODE pass like : ./a.out -d <.bmp_file> \n");
    printf("\n***********************************************************\n\n");

    exit(-1);
}