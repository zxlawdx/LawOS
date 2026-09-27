#ifndef LAWOS_PROCESSOS_H
#define LAWOS_PROCESSOS_H

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PROCESS_DIR "/proc"
#define PROCESS_NAME_MAX 256

/* Estados dos processos */
typedef enum {
    PROCESSO_DESCONHECIDO,
    PROCESSO_EXECUTANDO,
    PROCESSO_DORMINDO,
    PROCESSO_ESPERANDO,
    PROCESSO_PARADO,
    PROCESSO_ZUMBI,
    PROCESSO_MORTO
} EstadoProcesso;

/* Informações obtidas sobre um processo */
typedef struct {
    pid_t pid;
    pid_t ppid;
    char* endereco_memoria;
    char nome[PROCESS_NAME_MAX];
    EstadoProcesso estado;
    uint64_t tamanho;

} ProcessoInfo;

/*
 * Retorna uma lista de processos.
 *
 * quantidade recebe o número de processos encontrados.
 * O chamador deve liberar o vetor retornado com free().
 *
 * Retorna NULL caso ocorra uma falha.
 */
ProcessoInfo *listarProcessos(size_t *quantidade);

/*
 * Obtém as informações de um processo específico.
 *
 * Retorna 0 em caso de sucesso e -1 em caso de erro.
 */
int obterProcesso(pid_t pid, ProcessoInfo *processo);

/* Converte o estado retornado pelo Linux. */
EstadoProcesso converterEstado(char estado);


pid_t iniciarProcesso(const char *programa, char *const argumentos[]);

int encerrarProcesso(pid_t pid);

int aguardarProcesso(pid_t pid);
#endif