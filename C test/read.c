#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{

    // variables necesarias
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // abre el archivo

    fp = fopen("config.txt", "r");

    // verifica si se pudo leer el archivo
    if (fp == NULL)
        exit(EXIT_FAILURE);

    // lee la linea 1. la del puerto
    read = getline(&line, &len, fp);

    char port[4];
    strcpy(port, line + 5);

    printf("%s \n", port);

    // lee la linea 2. la direccion de la carpeta a utilizar para guardar
    read = getline(&line, &len, fp);
    char path[100];
    strcpy(path, line + 7);

    printf("%s \n", path);

    // cierra el archivo
    fclose(fp);

    // libera memoria
    if (line)
        free(line);
}

/*
while ((read = getline(&line, &len, fp)) != -1) {
    printf("Retrieved line of length %zu:\n", read);
    printf("%s", line);
}
*/