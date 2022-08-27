
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <curl/curl.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "./header/base64.h"
#include "./header/cJSON.h"

#ifdef DISABLE_EXPECT
/*
  Using POST with HTTP 1.1 implies the use of a "Expect: 100-continue"
  header.  You can disable this header with CURLOPT_HTTPHEADER as usual.
  NOTE: if you want chunked transfer too, you need to combine these two
  since you can only set one list of headers with CURLOPT_HTTPHEADER. */

/* A less good option would be to enforce HTTP 1.0, but that might also
   have other implications. */
{
    struct curl_slist *chunk = NULL;

    chunk = curl_slist_append(chunk, "Expect:");
    res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    /* use curl_slist_free_all() after the *perform() call to free this
       list again */
}
#endif

int main()
{

    // lee el archivo de configuracion

    char puerto[5];
    char filePath[200];
    char direccionIP[16];
    FILE *fd;
    // *filePath = "plat.jpg";

    // espera la entrada del usuario, path de la imagen
    printf("Ingrese el path del archivo de texto: \n");
    scanf("%200s", filePath);

    // espera la entrada del usuario, valor del puerto
    printf("Ingrese un valor para el puerto: \n");
    scanf("%5s", puerto);

    // espera la entrada del usuario, path de la imagen
    printf("Ingrese la direccion IP: \n");
    scanf("%16s", direccionIP);

    printf("puerto: %s, path %s y direccion IP: %s \n", puerto, filePath, direccionIP);

    /* int width, height, channels;

    unsigned char *img = stbi_load(filePath, &width, &height, &channels, 0);
    // unsigned char *img = stbi_load(filePath, &width, &height, &channels, 0);
    if (img == NULL)
    {
        printf("error al cargar el archivo \n");
        exit(1);
    }*/

    fd = fopen(filePath, "rb"); /* open file to upload */
    if(!fd)
    {
        printf("error al cargar el archivo \n");
        return 1;
    }
    printf("archivo cargado \n");

    char direccion[35];
    strcpy( direccion, "http://" );
    strcat( direccion, direccionIP );
    strcat( direccion, ":"  );
    strcat( direccion, puerto);
    strcat( direccion, "/archivo");

    printf("Direccion URL: %s \n", direccion);

    // pixeles

    //size_t img_size = width * height * channels;

    //char *base64 = malloc(sizeof(char) * Base64encode_len(img_size));
    //Base64encode(base64, img, img_size);

    cJSON *json = cJSON_CreateObject();

    cJSON_AddStringToObject(json, "filename", filePath);
    //cJSON_AddNumberToObject(json, "pixel", puerto);
    //cJSON_AddNumberToObject(json, "width", width);
    //cJSON_AddNumberToObject(json, "height", height);
    //cJSON_AddNumberToObject(json, "channels", channels);
    // cJSON_AddStringToObject(json, "content-base64", "10101010101010101010dwhOKsssssssssssssssssssssssssssssssssssssssssssHVwg9nQ3ur8HjVCrA0puJnPCV27rS6BPo4CUVDcIdAWs4kfxajCZH8MIvwhOKsssssssssssssssssssssssssssssssssssssssssssHVwg9nQ3ur8HjVCrA0puJnPCV27rS6BPo4CUVDcIdAWs4kfxajCZH8MIvwhOKsssssssssssssssssssssssssssssssssssssssssssHVwg9nQ3ur8HjVCrA0puJnPCV27rS6BPo4CUVDcIdAWs4kfxajCZH8MIvwhOKsssssssssssssssssssssssssssssssssssssssssssHVwg9nQ3ur8HjVCrA0puJnPCV27rS6BPo4CUVDcIdAWs4kfxajCZH8MIvwhOKsssssssssssssssssssssssssssssssssssssssssssHVwg9nQ3ur8HjVCrA0puJnPCV27rS6BPo4CUVDcIdAWs4kfxajCZH8MIvwhOKsssssssssssssssssssssssssssssssssssssssssssHVwg9nQ3ur8HjVCrA0puJnPCV27rS6BPo4CUVDcIdAWs4kfxajCZH8MIvwhOKsssssssssssssssssssssssssssssssssssssssssssHVwg9nQ3ur8HjVCrA0puJnPCV27rS6BPo4CUVDcIdAWs4kfxajCZH8MIvwhOKsssssssssssssssssssssssssssssssssssssssssssHVwg9nQ3ur8HjVCrA0puJnPCV27rS6BPo4CUVDcIdAWs4kfxajCZH8MIvwhOKsssssssssssssssssssssssssssssssssssssssssssHVwg9nQ3u");
    //cJSON_AddStringToObject(json, "content-base64", base64);

    char *jsonString = cJSON_Print(json);
    int jsonSize = strlen(jsonString);
    cJSON_AddNumberToObject(json, "size", jsonSize);
    // printf("%s", jsonString);

    CURL *curl;
    CURLcode res;

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);
    
    /* get a curl handle */
    curl = curl_easy_init();
    if (curl)
    {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */
        // curl_easy_setopt(curl, CURLOPT_URL, "https://36e74342-b0bf-46de-9d0e-03158360dbd2.mock.pstmn.io/");
        curl_easy_setopt(curl, CURLOPT_URL, direccion);
        // curl_easy_setopt(curl, CURLOPT_URL, "http://20.39.51.23:5001/image");
        /* Now specify the POST data */
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonString);
        curl_easy_setopt(curl, CURLOPT_MAXFILESIZE, 1000000);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1000000);
        // Disable Expect: 100-continue
        struct curl_slist *chunk = NULL;
        chunk = curl_slist_append(chunk, "Expect:");
        chunk = curl_slist_append(chunk, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    // int Base64encode(char * coded_dst, const char *plain_src,int len_plain_src);
    // for (unsigned char *p = img; p != img + img_size; p += channels)
    // {
    //     printf(" valor de pixel: (%d, %d, %d) \n", *p, *(p + 1), *(p + 2));
    // }

    //stbi_image_free(img);
}

// -I/mingw64/include
// gcc Client.c ./source/base64.c ./source/cJSON.c -lm -o Client -lcurl && ./Client
// sudo apt-get install libcurl4-openssl-dev
