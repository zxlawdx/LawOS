#include "processos.h"
#include <stdio.h>
#include <stdlib.h>


int main(){
    ProcessoInfo *processos;
    size_t qtd = 1;
    processos = listarProcessos(&qtd);
    
    for(size_t i = 0; i < qtd; i++)
        printf("PID: %d - Process: %s\n", processos[i].pid, processos[i].nome);
}