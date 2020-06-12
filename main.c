#include "include/main.h"
#include "include/embed.h"
#include "include/extract.h"

int main(int argc, char ** argv) {
    arg_parse(argc, argv);
    validate_params();

    if (stegobmp_config.operation == embed) {
        start_embedding();
    } else if (stegobmp_config.operation == extract) {
        start_extraction();
    }
}

void arg_parse(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Try 'stegobmp -h' for more information.\n");
        exit(EXIT_FAILURE);
    }

    // Default values
    stegobmp_config.operation = operation_undefined;
    stegobmp_config.file_to_hide = "";
    stegobmp_config.bearer = "";
    stegobmp_config.out_bitmapfile = "";
    stegobmp_config.steg = steg_undefined;
    stegobmp_config.encrypt = false;
    stegobmp_config.enc_algorithm = "aes128";
    stegobmp_config.enc_mode = "cbc";

    int option_index = 0;
    int option;
    static struct option long_options[] = {
            {"embed", no_argument, NULL, 'e'},
            {"extract", no_argument, NULL, 'E'},
            {"help", no_argument, NULL, 'h'},
            {"in", required_argument, NULL, 'i'},
            {"p", required_argument, NULL, 'p'},
            {"out", required_argument, NULL, 'o'},
            {"steg", required_argument, NULL, 's'},
            {"a", required_argument, NULL, 'a'},
            {"m", required_argument, NULL, 'm'},
            {"pass", required_argument, NULL, 'P'}

    };

    while ((option = getopt_long_only(argc, argv, ":eEhi:p:o:s:a:m:P:", long_options, &option_index)) != -1) {
        switch (option) {
            case 'e':
                stegobmp_config.operation  = embed;
                break;
            case 'E':
                stegobmp_config.operation = extract;
                break;
            case 'h':
                help();
                break;
            case 'i':
                stegobmp_config.file_to_hide = malloc(strlen(optarg) + 1);
                memcpy(stegobmp_config.file_to_hide, optarg, strlen(optarg) + 1);
                break;
            case 'p':
                stegobmp_config.bearer = malloc(strlen(optarg) + 1);
                memcpy(stegobmp_config.bearer, optarg, strlen(optarg) + 1);
                break;
            case 'o':
                stegobmp_config.out_bitmapfile = malloc(strlen(optarg) + 1);
                memcpy(stegobmp_config.out_bitmapfile, optarg, strlen(optarg) + 1);
                break;
            case 's':
                if (strcmp(optarg, "LSB1") == 0) {
                    stegobmp_config.steg = LSB1;
                } else if (strcmp(optarg, "LSB4") == 0) {
                    stegobmp_config.steg = LSB4;
                } else if (strcmp(optarg, "LSBI") == 0) {
                    stegobmp_config.steg = LSBI;
                } else {
                    fprintf(stderr, "Invalid option for steg.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'a':
                stegobmp_config.enc_algorithm = malloc(strlen(optarg) + 1);
                memcpy(stegobmp_config.enc_algorithm, optarg, strlen(optarg) + 1);
                break;
            case 'm':
                stegobmp_config.enc_mode = malloc(strlen(optarg) + 1);
                memcpy(stegobmp_config.enc_mode, optarg, strlen(optarg) + 1);
                break;
            case 'P':
                stegobmp_config.encrypt = true;
                stegobmp_config.pass = malloc(strlen(optarg) + 1);
                memcpy(stegobmp_config.pass, optarg, strlen(optarg) + 1);
                break;
            case ':':
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                exit(EXIT_FAILURE);
            case '?':
                fprintf(stderr, "stegobmp: invalid option -- '%c'\n", optopt);
            default:
                fprintf(stderr, "Try 'stegobmp -h' for more information.\n");
                exit(EXIT_FAILURE);
        }
    }

    bool non_option_arg_flag = false;
    for (int index = optind; index < argc-1; index++) {
        fprintf (stderr, "Non-option argument %s\n", argv[index]);
        non_option_arg_flag = true;
    }

    if(non_option_arg_flag)
        exit(EXIT_FAILURE);
}

void help() {
    printf("Uso:\n"
           "\tstegobmp [ -embed | -extract ] -in file -p bitmapfile -out bitmapfile -steg <LSB1 | LSB4 | LSBI> [ -a <aes128 | ase192 | aes256 | des> -m <ecb | cfb | ofb | cbc> -pass password ]\n\n"
           "Parámetros:\n"
           "\t-embed          Indica que se va a ocultar información.\n"
           "\t-extract        Indica que se va a extraer información.\n"
           "\t-help           Imprime la ayuda y termina.\n"
           "\t-in file        Archivo que se va a ocultar.\n"
           "\t-p bitmapfile   Archivo bmp que será el portador.\n"
           "\t-o bitmapfile   Archivo bmp de salida, es decir, el archivo bitmapfile con la información de file incrustada.\n\n"
           "Parámetros opcionales:\n"
           "\t-steg <LSB1 | LSB4 | LSBI>            Algoritmo de esteganografiado: LSB de 1bit, LSB de 4 bits, LSB Enhanced\n"
           "\t-a  <aes128 | aes192 | aes256 | des>  Algoritmo de encripción\n"
           "\t-m <ecb | cfb | ofb | cbc>            Modo de encripción\n"
           "\t-pass password                        Contraseña de encripción\n");

    exit(0);
}

void validate_params(void) {
    bool error = false;
    char * error_message = "";

    if (stegobmp_config.operation == operation_undefined) {
        error = true;
        error_message = "Debe indicar la operación a realizar con --embed o --extract.\n";
    } else if (stegobmp_config.operation == embed) {
        if (strcmp(stegobmp_config.file_to_hide, "") == 0) {
            error = true;
            error_message = "Debe indicar la ruta del archivo que se va a ocultar.\n";
        }
    }
    if (strcmp(stegobmp_config.bearer, "") == 0) {
        error = true;
        error_message = "Debe indicar la ruta del archivo bmp portador.\n";
    }
    if (strcmp(stegobmp_config.out_bitmapfile, "") == 0) {
        error = true;
        error_message = "Debe indicar la ruta del archivo de salida.\n";
    }
    if (stegobmp_config.steg == steg_undefined) {
        error = true;
        error_message = "Debe indicar el algoritmo de esteganografiado a utilizar.\n";
    }
    if (stegobmp_config.encrypt) {
        if (strcmp(stegobmp_config.enc_algorithm, "aes128") != 0 && strcmp(stegobmp_config.enc_algorithm, "aes192") != 0 &&
            strcmp(stegobmp_config.enc_algorithm, "aes256") != 0 && strcmp(stegobmp_config.enc_algorithm, "des") != 0) {
            error = true;
            error_message = "Debe indicar un algoritmo de encripción válido";
        }
        if (strcmp(stegobmp_config.enc_mode, "ecb") != 0 && strcmp(stegobmp_config.enc_mode, "cfb") != 0 &&
            strcmp(stegobmp_config.enc_mode, "ofb") != 0 && strcmp(stegobmp_config.enc_mode, "cbc") != 0) {
            error = true;
            error_message = "Debe indicar un modo de encripción válido";
        }
    }

    if (error) {
        fprintf(stderr, "%s", error_message);
        exit(EXIT_FAILURE);
    }
}
