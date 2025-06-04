#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "decode.h"

//Function calls for Decoding
Status do_decoding(EncodeInfo *encInfo)
{
    if(open_files_2(encInfo) == e_success)  //Open and validate files
    {
        printf("INFO: opened Required files\n");
        sleep(1);
    }
    else
    {
        printf("FAILURE: open_files_2 function not completed\n");
        exit(0);
    }

    if(decode_magic_string(MAGIC_STRING, encInfo) == e_success) //Decode Magic string and validate
    {
        printf("INFO: Magic String Matched Successfully\n");
        sleep(1);
    }
    else
    {
        printf("INFO: Magic String Not Matched. Terminating program...\n");
        exit(0);
    }

    //Decode extension
    decode_exten_open_file(encInfo);
    sleep(1);

    //Decode file data
    decode_file(encInfo);
    sleep(1);

    //Close all files
    fclose(encInfo->fptr_stego_image);
    fclose(encInfo->fptr_decoded_file);
}

/*
 * Function: decode_file

 * Decodes the actual contents (data) of the secret file from the stego image.
 * Steps:
 *   1. Decode size of secret file (stored during encoding).
 *   2. Allocate buffer and decode 'size' bytes of data.
 *   3. Write decoded content to output file.
 */
Status decode_file(EncodeInfo *encInfo)
{
    long size;

    // Step 1: Decode size of secret file (8 bytes)
    decode_data_to_text((char *)&size, sizeof(long), encInfo);

    printf("INFO: Decoding the file data..\n");
    sleep(1);
    // Step 2: Create buffer for decoded secret data
    char text[size];

    // Step 3: Decode secret data into buffer
    decode_data_to_text(text, size, encInfo);

    // Step 4: Write the decoded data to output file
    fwrite(text, size, 1, encInfo->fptr_decoded_file);

    printf("\n~~~~ SUCCESSFULLY DECODED SECRET FILE ~~~~\n\n");
}

/*
 * Function: decode_exten_open_file
 * Decodes the file extension of the secret file and creates
 * the output file (decoded_file.<ext>) accordingly.
 */
Status decode_exten_open_file(EncodeInfo *encInfo)
{
    long size;

    // Step 1: Decode size of file extension
    decode_data_to_text((char *)&size, sizeof(long), encInfo);

    // Step 2: Decode actual extension string
    char file_extn[6];  // Assumes extensions are < 6 characters (e.g., .txt, .c)
    decode_data_to_text(file_extn, size, encInfo);
    file_extn[size + 1] = '\0';  // Null terminate extension

    // Step 3: Create final filename using default name + extension
    char *file_name = "decoded_file";
    char file[20];
    strcpy(file, file_name);
    strcat(file, file_extn);
    file[strlen(file_name) + strlen(file_extn) - 1] = '\0';  // Sanitize

    // Step 4: Open the output file to write decoded data
    encInfo->fptr_decoded_file = fopen(file, "w");
    printf("INFO: Decoding file in %s\n", file); 

    // Error handling
    if (encInfo->fptr_decoded_file == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->decoded_fname);
    	exit(0);
    }
}

/*
 * Function: decode_magic_string
 * Verifies the presence of the magic string in the stego image.
 * This ensures the image actually contains hidden data.
 */
Status decode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    // Skip BMP header
    fseek(encInfo->fptr_stego_image, 54, SEEK_SET);

    char ch[3];

    // Decode first 2 bytes from LSBs of image data
    decode_data_to_text(ch, 2, encInfo);
    ch[2] = '\0';  // Null terminate decoded string

    // Compare decoded string with original magic string
    if(strcmp(ch, magic_string) == 0)
    {
        return e_success;  
    }
    else
    {
        return e_failure;
    }
}

/*
 * Function: decode_data_to_text
 * Core decoding logic for extracting text from image data.
 * 
 * Logic:
 *   For each byte to decode:
 *     - Read 8 bytes from image
 *     - Extract LSBs from those bytes
 *     - Combine bits into one character
 */
Status decode_data_to_text(char *data, int size, EncodeInfo *encInfo)
{
    int i = 0;
    while(i < size)
    {
        char ch_o, ch_n = 0;

        // For each bit from MSB to LSB
        for(int j = 7; j >= 0; j--)
        {
            // Read one byte from image
            fread(&ch_o, 1, 1, encInfo->fptr_stego_image);

            // Extract LSB and set corresponding bit
            if(ch_o & 1)
            {
                ch_n |= (1 << j);  // Set bit to 1
            }
            else
            {
                ch_n &= ~(1 << j); // Ensure bit is 0
            }
        }

        // Store constructed character into data
        strcpy(&data[i], &ch_n);
        i++;
    }    
}

/*
 * Function: open_files_2
 * Opens the stego image file for reading during decoding.
 */
Status open_files_2(EncodeInfo *encInfo)
{
    // Open stego image (which has the hidden data)
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "r");

    // Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
    	return e_failure;
    }

    return e_success;
}
