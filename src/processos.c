#include "processos.h"


int ehNumero(const char* str){
    if(!str) return 0;

    while(*str){
        if(!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}

ProcessoInfo* listarProcessos(size_t *total){
    DIR *diretorio = opendir("/proc");
    if(!total){
        perror("Erro ao inicializar");
        return NULL;
    }

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
    
    
    while ((entrada = readdir(diretorio)) != NULL) {
        if (!ehNumero(entrada->d_name))
            continue;

        // 1. Monta o caminho completo com segurança (/proc/<pid>/comm)
        char path[2048];
        snprintf(path, sizeof(path), "/proc/%s/comm", entrada->d_name);

        // 2. Abre o arquivo (ordem: caminho primeiro, modo depois)
        FILE *file = fopen(path, "r");
        if (!file)
            continue; 

        // 3. Lê o conteúdo para uma string (array de char)
        char name_process[256];
        if (fgets(name_process, sizeof(name_process), file) != NULL) {
            // Remove o '\n' que o comm sempre inclui no final
            name_process[strcspn(name_process, "\n")] = '\0';
        } else {
            fclose(file);
            continue;
        }

        // 4. Sempre feche o arquivo para não esgotar descritores
        fclose(file);

        // 5. Realocação e armazenamento no array dinâmico
        if (quantidade == capacidade) {
            size_t nova_capacidade = (capacidade == 0) ? 16 : capacidade * 2;
            ProcessoInfo *temp = realloc(processos, nova_capacidade * sizeof(ProcessoInfo));
            if (!temp) {
                perror("Erro ao realocar memória");
                free(processos);
                closedir(diretorio);
                return NULL;
            }
            processos = temp;
            capacidade = nova_capacidade;
        }

        // Exemplo populando a struct:
        processos[quantidade].pid = atoi(entrada->d_name);
        strncpy(processos[quantidade].nome, name_process, sizeof(processos[quantidade].nome) - 1);
        processos[quantidade].nome[sizeof(processos[quantidade].nome) - 1] = '\0';
        quantidade++;
    }
        closedir(diretorio);
        *total = quantidade;
        return processos;
        
    }
    

   
