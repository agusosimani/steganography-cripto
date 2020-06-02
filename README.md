#stegobmp

Directorio raiz: steganography-cripto/

##Materiales

* Archivo de construcción generado por CLion CMakeLists.txt
* Códigos fuente

##Compilación

Parado en el directorio raiz:

$ cd cmake-build-debug

$ make

###Artefacto generado en la carpeta cmake-build-debug
* stegobmp

##Ejecución

###stegobmp
Parado en el directorio cmake-build-debug

    ./stegobmp [ --embed | --extract ] --in file -p bitmapfile --out bitmapfile --steg <LSB1 | LSB4 | LSBI> [ -a <aes128 | ase192 | aes256 | des> -m <ecb | cfb | ofb | cbc> --pass password ]

    Parámetros:
        -e | --embed              Indica que se va a ocultar información.
        -E | --extract            Indica que se va a extraer información.
        -h | --help               Imprime la ayuda y termina.
        -i | --in file            Archivo que se va a ocultar.
        -p | --bearer bitmapfile  Archivo bmp que será el portador.
        -o | --out bitmapfile     Archivo bmp de salida, es decir, el archivo bitmapfile con la información de file incrustada.
    
    Parámetros opcionales:
        -s | --steg <LSB1 | LSB4 | LSBI>                   Algoritmo de esteganografiado: LSB de 1bit, LSB de 4 bits, LSB Enhanced
        -a | --algorithm <aes128 | aes192 | aes256 | des>  Algoritmo de encripción
        -m | --mode <ecb | cfb | ofb | cbc>                Modo de encripción
        -P | --pass password                               Contraseña de encripción
