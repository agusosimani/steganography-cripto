#include "include/main.h"

int main(int argc, char ** argv) {
    arg_parse(argc, argv);
}

void arg_parse(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Try 'stegobmp -h' for more information.\n");
        exit(EXIT_FAILURE);
    }

    // Default values
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
            {"bearer", required_argument, NULL, 'p'},
            {"out", required_argument, NULL, 'o'},
            {"steg", required_argument, NULL, 's'},
            {"algorithm", required_argument, NULL, 'a'},
            {"mode", required_argument, NULL, 'm'},
            {"pass", required_argument, NULL, 'P'}

    };

    while ((option = getopt_long(argc, argv, ":eEhi:p:o:s:a:m:P:", long_options, &option_index)) != -1) {
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
           "\tstegobmp [ --embed | --extract ] --in file -p bitmapfile --out bitmapfile --steg <LSB1 | LSB4 | LSBI> [ -a <aes128 | ase192 | aes256 | des> -m <ecb | cfb | ofb | cbc> --pass password ]\n\n"
           "Parámetros:\n"
           "\t-e | --embed              Indica que se va a ocultar información.\n"
           "\t-E | --extract            Indica que se va a extraer información.\n"
           "\t-h | --help               Imprime la ayuda y termina.\n"
           "\t-i | --in file            Archivo que se va a ocultar.\n"
           "\t-p | --bearer bitmapfile  Archivo bmp que será el portador.\n"
           "\t-o | --out bitmapfile     Archivo bmp de salida, es decir, el archivo bitmapfile con la información de file incrustada.\n\n"
           "Parámetros opcionales:\n"
           "\t-s | --steg <LSB1 | LSB4 | LSBI>                   Algoritmo de esteganografiado: LSB de 1bit, LSB de 4 bits, LSB Enhanced\n"
           "\t-a | --algorithm <aes128 | aes192 | aes256 | des>  Algoritmo de encripción\n"
           "\t-m | --mode <ecb | cfb | ofb | cbc>                Modo de encripción\n"
           "\t-P | --pass password                               Contraseña de encripción\n");

    exit(0);
}
