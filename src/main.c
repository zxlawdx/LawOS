#include "processos.h"
#include <stdio.h>
#include <stdlib.h>


int main(){
    ProcessoInfo *processos;

    processos = listarProcessos();
    
    for(int i = 0; processos != NULL; i++)
        printf("PID: %d - Process: %s\n", processos[i].pid, processos[i].nome);
}