#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>
#include "embed.h"
#include "extract.h"

enum Operation { embed, extract, operation_undefined };
enum Steg { LSB1, LSB4, LSBI, steg_undefined };

typedef struct {
    enum Operation operation;
    char * file_to_hide;
    char * bearer;
    char * out_bitmapfile;
    enum Steg steg;
    bool encrypt;
    char * enc_algorithm;
    char * enc_mode;
    char * pass;
} stegobmp_config_t;

stegobmp_config_t stegobmp_config;

void arg_parse(int argc, char **argv);
void help();
void validate_params(void);

#endif //MAIN_H
