#ifndef DECODE_H
#define DECODE_H

#include "types.h"

Status do_decoding(EncodeInfo *encInfo);
Status open_files_2(EncodeInfo *encInfo);
Status decode_exten_open_file(EncodeInfo *encInfo);
Status decode_file(EncodeInfo *encInfo);
Status decode_magic_string(char *magic_string, EncodeInfo *encInfo);
Status decode_data_to_text(char *data, int size, EncodeInfo *encInfo);

#endif