#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

enum Operation { embed, extract };
enum Steg { LSB1, LSB4, LSBI };

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

#endif //MAIN_H
