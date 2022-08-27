#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>


int main ()
{   
    int pixelValue;
    char imagePath[200];

    //espera la entrada del usuario, valor del pixel
    printf("Ingrese un valor de pixel: \n");
    scanf("%d", &pixelValue);


    //espera la entrada del usuario, path de la imagen
    printf("Ingrese el path de la imagen: \n");
    scanf("%200s", imagePath);



    printf("pixel: %d, path %s \n", pixelValue, imagePath );



    return(0);







}