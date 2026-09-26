#ifndef PROCESSOS_H
#define PROCESSOS_H
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>


typedef struct {
    int pid;
    char nome[256];
    __u_int size;
    int killed;
    
} ProcessoInfo;

int ehNumero(const char* str);

ProcessoInfo* listarProcessos(void);

#endif