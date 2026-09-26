#include "processos.h"


int ehNumero(const char* str){
    if(!str) return 0;

    while(*str){
        if(!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}

ProcessoInfo* listarProcessos(void){
    DIR *diretorio = opendir("/proc");
    
    if(!diretorio){
        perror("Erro ao abrir /proc");
        return NULL;
    }
    struct dirent* entrada;
    size_t capacidade = 10;
    size_t quantidade = 0;

    ProcessoInfo* processos = malloc(
        capacidade * sizeof(ProcessoInfo)
    );
    
    int i = 0;
    while ((entrada = readdir(diretorio)) != NULL){
        if(!ehNumero(entrada->d_name))
            continue;

        
        DIR *name = opendir()
        
        
        if(capacidade == quantidade){
            size_t nova_capacidade = capacidade * 2;

            ProcessoInfo *temp = realloc(
                processos,
                nova_capacidade * * sizeof(ProcessoInfo)
            )
        }
        if (!temp) {
            perror("Erro ao realocar memória");
            free(processos);
            closedir(diretorio);
            return NULL;
        }

        
    }
    

    closedir(diretorio);
}