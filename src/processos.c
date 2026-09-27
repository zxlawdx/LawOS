#include "processos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/wait.h>
#include <errno.h>

int ehNumero(const char* str) {
    if (!str) return 0;

    while (*str) {
        if (!isdigit((unsigned char)*str)) return 0;
        str++;
    }

    return 1;
}

ProcessoInfo* listarProcessos(size_t *total) {

    if (!total) {
        fprintf(stderr, "Ponteiro total inválido.\n");
        return NULL;
    }

    *total = 0;

    DIR *diretorio = opendir(PROCESS_DIR);

    if (!diretorio) {
        perror("Erro ao abrir /proc");
        return NULL;
    }

    struct dirent* entrada;
    size_t capacidade = 10;
    size_t quantidade = 0;

    ProcessoInfo* processos = malloc(
        capacidade * sizeof(ProcessoInfo)
    );

    if (!processos) {
        perror("Erro ao alocar memória");
        closedir(diretorio);
        return NULL;
    }

    while ((entrada = readdir(diretorio)) != NULL) {

        if (!ehNumero(entrada->d_name))
            continue;

        // 1. Monta o caminho completo (/proc/<pid>/comm)
        char path[2048];

        snprintf(
            path,
            sizeof(path),
            "%s/%s/comm",
            PROCESS_DIR,
            entrada->d_name
        );

        // 2. Abre o arquivo
        FILE *file = fopen(path, "r");

        if (!file)
            continue;

        // 3. Lê o nome do processo
        char name_process[PROCESS_NAME_MAX];

        if (fgets(name_process, sizeof(name_process), file) != NULL) {

            name_process[
                strcspn(name_process, "\n")
            ] = '\0';

        } else {
            fclose(file);
            continue;
        }

        // 4. Fecha o arquivo
        fclose(file);

        // 5. Realoca o vetor quando necessário
        if (quantidade == capacidade) {

            size_t nova_capacidade = capacidade * 2;

            ProcessoInfo *temp = realloc(
                processos,
                nova_capacidade * sizeof(ProcessoInfo)
            );

            if (!temp) {
                perror("Erro ao realocar memória");

                free(processos);
                closedir(diretorio);

                return NULL;
            }

            processos = temp;
            capacidade = nova_capacidade;
        }

        // 6. Preenche a estrutura ProcessoInfo

        processos[quantidade] = (ProcessoInfo){0};

        processos[quantidade].pid =
            (pid_t) atoi(entrada->d_name);

        processos[quantidade].estado =
            PROCESSO_DESCONHECIDO;

        strncpy(
            processos[quantidade].nome,
            name_process,
            sizeof(processos[quantidade].nome) - 1
        );

        processos[quantidade].nome[
            sizeof(processos[quantidade].nome) - 1
        ] = '\0';

        quantidade++;
    }

    closedir(diretorio);

    *total = quantidade;

    return processos;
}


EstadoProcesso converterEstado(char estado) {
    switch (estado) {
        case 'R':
            return PROCESSO_EXECUTANDO;

        case 'S':
        case 'I':
            return PROCESSO_DORMINDO;

        case 'D':
            return PROCESSO_ESPERANDO;

        case 'T':
        case 't':
            return PROCESSO_PARADO;

        case 'Z':
            return PROCESSO_ZUMBI;
        case 'X':
        case 'x':
            return PROCESSO_MORTO;
        default:
            return PROCESSO_DESCONHECIDO;
    }
}

pid_t iniciarProcesso(
    const char *programa,
    char *const argumentos[]
) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("Erro ao criar processo");
        return -1;
    }

    if (pid == 0) {
        execvp(programa, argumentos);

        perror("Erro ao executar programa");
        _exit(127);
    }

    return pid;
}

int encerrarProcesso(pid_t pid) {
    if (pid <= 0)
        return -1;

    int is_kill = kill(pid, SIGTERM);

    if (is_kill == -1) {
        perror("Erro ao enviar sinal");
        return -1;
    }

    return 0;
}


int aguardarProcesso(pid_t pid){
    int status;
    pid_t resultado;

    do {
        resultado =  waitpid(pid, &status, 0);
    }while(resultado == -1 && errno == EINTR);

    if(resultado == -1){
         perror("Erro ao aguardar processo");
        return -1;
    }

    // if(WIFEXITED(status)){
    //     printf(
    //         "Processo %ld terminou com código %d\n",
    //         (long)pid,
    //         WEXITSTATUS(status)
    //     );
    // } else if (WIFSIGNALED(status)){
    //     printf(
    //         "Processo %ld terminou pelo sinal %d\n",
    //         (long)pid,
    //         WTERMSIG(status)
    //     );
    // }

    return 0;
}