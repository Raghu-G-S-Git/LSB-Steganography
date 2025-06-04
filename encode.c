#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "decode.h"

/* ------------------- Function Definitions ------------------- */

//Function calls for Encoding
Status do_encoding(EncodeInfo *encInfo)
{
    printf("INFO: ## Encoding Proceduer Started ##\n");
    sleep(1);
    //Store first 54 bytes header data as it is 
    copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    sleep(1);
    //Encode Magic String declared in common.h
    encode_magic_string(MAGIC_STRING, encInfo);
    sleep(1);
    //Encode secret file extension size and extension
    encode_secret_file_extn(strstr(encInfo->secret_fname,"."), encInfo);
    sleep(1);
    //Encode secret file size
    encode_secret_file_size(encInfo->size_secret_file, encInfo);
    sleep(1);
    //Encode secret file data in lsb 
    encode_secret_file_data(encInfo);
    sleep(1);
    //Encode remaining data as it is
    copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image);
    sleep(1);

    printf("INFO: ## Encoding Done Successfully ##\n");
    //Close all files
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_stego_image);
    fclose(encInfo->fptr_secret);
}

/*
 * Function  : get_image_size_for_bmp Calculates image size in bytes for a BMP file.
 * Parameters: fptr_image - pointer to the image file
 * Returns   : Image size in bytes = width * height * 3 (since it's a 24-bit BMP)
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;

    // Seek to the 18th byte where width is stored
    fseek(fptr_image, 18, SEEK_SET);

    // Read width (4 bytes)
    fread(&width, sizeof(int), 1, fptr_image);

    // Read height (4 bytes)
    fread(&height, sizeof(int), 1, fptr_image);

    // Return total number of bytes used to store the image data
    return width * height * 3;
}

/*
 * Function: get_file_size
 * Returns total size of the file.
 */
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);  // Go to end of file
    return ftell(fptr);        // Get byte position = file size
}

/*
 * Function: open_files
 * Opens the source image, secret file, and stego image file.
 * Performs error checking for each file.
 */
Status open_files(EncodeInfo *encInfo)
{
    printf("INFO: Opnenig required files\n");
    sleep(1);
    // Open source BMP image
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
    	return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo->src_image_fname);
    sleep(1);
    // Open secret file to hide
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
    	return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo->secret_fname);
    sleep(1);
    // Create output file (stego image)
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
    	return e_failure;
    }
    printf("INFO: Opened %s\n", encInfo->stego_image_fname);
    sleep(1);

    return e_success;
}

/*
 * Function: copy_bmp_header
 * Copies the first 54 bytes of the BMP header from source image to stego image.
 */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char ch;
    fseek(fptr_src_image, 0, SEEK_SET);  // Move to beginning
    while(ftell(fptr_src_image) < 54)    // BMP header size is 54 bytes
    {
        fread(&ch, 1, 1, fptr_src_image);
        fwrite(&ch, 1, 1, fptr_dest_image);
    }
    printf("INFO: Header file copied\n"); 
    sleep(1);  
}

/*
 * Function: encode_magic_string
 * Encodes a predefined magic string to help during decoding.
 */
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string, strlen(magic_string), encInfo);
    printf("INFO: Magic string encoded\n");
}

/*
 * Function: encode_secret_file_extn
 * Encodes the extension of the secret file (.txt, .c, etc.).
 */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    long size = (long)strlen(file_extn);

    // Encode extension length first
    encode_secret_file_size(size, encInfo);

    // Encode the extension
    encode_data_to_image(file_extn, size, encInfo);
    printf("INFO: File Extension encoded\n");
}

/*
 * Function: encode_secret_file_size
 * Encodes the size of the secret file (in bytes) into the image.
 */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char *size_ch = (char*)&file_size;  // Convert to byte sequence

    // Encode 8 bytes of file size
    encode_data_to_image(size_ch, sizeof(long), encInfo);
    printf("INFO: Secret File Size encoded\n");
}

/*
 * Function: encode_secret_file_data
 * Reads each character from the secret file and encodes it into the image.
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    rewind(encInfo->fptr_secret);  // Reset to beginning of file

    ch = fgetc(encInfo->fptr_secret);
    while(ch != EOF)
    {
        encode_data_to_image(&ch, 1, encInfo);
        ch = fgetc(encInfo->fptr_secret);
    }
    
    printf("INFO: Secret File encoded\n");
}

/*
 * Function: copy_remaining_img_data
 * After encoding, copies the rest of the image from source to stego image.
 */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while(1)
    {
        if(!fread(&ch, 1, 1, fptr_src))  // If end of file
            break;
        fwrite(&ch, 1, 1, fptr_dest);
    }
    printf("INFO: Remaining data Copied\n"); 
}

/*
 * Function: encode_data_to_image
 * Encodes each bit of input data (1 byte = 8 bits) into LSB of image bytes.
 * 
 * Logic:
 * For each byte in 'data':
 *   For each bit from MSB to LSB:
 *     - Read a byte from source image
 *     - Modify its LSB to match current bit of data
 *     - Write modified byte to stego image
 */
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
    int i = 0;
    while(i < size)
    {
        for(int j = 7; j >= 0; j--)  // For each bit in the byte
        {
            char ch_o, ch_n;

            // Read original byte from source image
            fread(&ch_o, 1, 1, encInfo->fptr_src_image);

            // Modify LSB to match data bit
            if(data[i] >> j & 1)
            {
                ch_n = ch_o | 1;       // Set LSB to 1
            }
            else
            {
                ch_n = ch_o & ~1;      // Set LSB to 0
            }

            // Write modified byte to stego image
            fwrite(&ch_n, 1, 1, encInfo->fptr_stego_image);
        }
        i++;
    }    
}

//function to Check capacity of file
Status check_capacity(EncodeInfo *encInfo)
{
    printf("INFO: Checking for beautiful.bmp capacity to handle secret.txt\n");
    sleep(1);
    if(get_image_size_for_bmp(encInfo->fptr_src_image) > ((encInfo->size_secret_file + 14) *8))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}