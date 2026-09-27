#include "comandos.h"
#include "processos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>


/*
 * Define a assinatura das funções
 * responsáveis pelos comandos internos.
 */
typedef ResultadoComando (*FuncaoComando)(
    int argc,
    char *const argv[]
);


/*
 * Representa um comando registrado.
 */
typedef struct {
    const char *nome;
    const char *subcomando;
    FuncaoComando executar;
} ComandoInterno;


/*
 * Comando: listar processos
 */
static ResultadoComando cmdListarProcessos(
    int argc,
    char *const argv[]
) {
    (void)argv;

    if (argc != 0) {
        printf("Uso: listar processos\n");
        return COMANDO_INTERNO;
    }

    size_t quantidade = 0;

    ProcessoInfo *processos = listarProcessos(
        &quantidade
    );

    if (!processos) {
        fprintf(stderr, "Erro ao listar processos.\n");
        return COMANDO_INTERNO;
    }

    printf("%-10s %s\n", "PID", "PROCESSO");

    for (size_t i = 0; i < quantidade; i++) {
        printf(
            "%-10ld %s\n",
            (long)processos[i].pid,
            processos[i].nome
        );
    }

    free(processos);

    return COMANDO_INTERNO;
}


/*
 * Comando: kill <PID>
 */
static ResultadoComando cmdKill(
    int argc,
    char *const argv[]
) {
    if (argc != 1) {
        printf("Uso: encerrar <PID>\n");
        return COMANDO_INTERNO;
    }

    char *fim;

    errno = 0;

    long numero = strtol(
        argv[0],
        &fim,
        10
    );

    if (errno == ERANGE ||
        fim == argv[0] ||
        *fim != '\0' ||
        numero <= 0 ||
        numero > INT_MAX) {

        fprintf(stderr, "PID inválido.\n");

        return COMANDO_INTERNO;
    }

    pid_t pid = (pid_t)numero;

    if (encerrarProcesso(pid) == 0) {
        printf(
            "Sinal enviado ao processo %ld\n",
            (long)pid
        );
    }

    return COMANDO_INTERNO;
}


/*
 * Comando: exit-shell
 */
static ResultadoComando cmdSair(
    int argc,
    char *const argv[]
) {
    (void)argv;

    if (argc != 0) {
        printf("Uso: exit-shell\n");
        return COMANDO_INTERNO;
    }

    return COMANDO_SAIR;
}


/*
 * Registro dos comandos internos.
 *
 * Múltiplos nomes podem apontar
 * para a mesma função.
 */
static const ComandoInterno comandos[] = {

    {
        "listar",
        "processos",
        cmdListarProcessos
    },

    {
        "lp",
        NULL,
        cmdListarProcessos
    },

    {
        "encerrar",
        NULL,
        cmdKill
    },

    {
        "matar",
        NULL,
        cmdKill
    },

    {
        "exit-shell",
        NULL,
        cmdSair
    },

    {
        "sair",
        NULL,
        cmdSair
    }
};


/*
 * Identifica e executa comandos registrados.
 */
ResultadoComando executarComandoInterno(
    int argc,
    char *const argv[]
) {
    if (argc <= 0 || !argv || !argv[0])
        return COMANDO_INTERNO;

    size_t total =
        sizeof(comandos) / sizeof(comandos[0]);

    for (size_t i = 0; i < total; i++) {

        const ComandoInterno *comando = &comandos[i];

        if (strcmp(argv[0], comando->nome) != 0)
            continue;

        int consumidos = 1;

        if (comando->subcomando != NULL) {

            if (argc < 2)
                continue;

            if (strcmp(
                argv[1],
                comando->subcomando
            ) != 0) {
                continue;
            }

            consumidos = 2;
        }

        return comando->executar(
            argc - consumidos,
            argv + consumidos
        );
    }

    return COMANDO_EXTERNO;
}