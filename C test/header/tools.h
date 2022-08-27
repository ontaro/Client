#ifndef _TOOLS_H__
#define _TOOLS_H__

#include <stdio.h>

#define MAXCHAR 1000

char *t_port_val;
char *t_colores_val;
char *t_histo_val;
char *t_logs_val;
char *t_pyS_val;

int *lens;

char *port;
char *colores_path;
char *histo_path;
char *logs_path;
char *pyS_path;

int getlen(char line[]);
void parse_object(char *string);
void clean_string(char *string);

#endif