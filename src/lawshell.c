#include "lawshell.h"
#include "processos.h"
#include "completador.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include <readline/readline.h>
#include <readline/history.h>

#define MAX_ARGUMENTOS 32

void shell(void) {
    

    configurarAutocomplete();

    while (1) {
        char *comando = readline("lawOS@root: ");

        // Ctrl + D encerra o shell.
        if (!comando)
            break;

        // Ignora linhas vazias.
        if (comando[strspn(comando, " \t")] == '\0') {
            free(comando);
            continue;
        }

        add_history(comando);

        // Separa o comando e seus argumentos.
        char *argumentos[MAX_ARGUMENTOS];

        size_t quantidade = 0;

        char *token = strtok(comando, " \t");

        while (token && quantidade < MAX_ARGUMENTOS - 1) {
            argumentos[quantidade++] = token;
            token = strtok(NULL, " \t");
        }

        argumentos[quantidade] = NULL;

        if (token != NULL) {
            printf("Número máximo de argumentos excedido.\n");
            free(comando);
            continue;
        }

        // Encerra o LawShell.
        if (strcmp(argumentos[0], "exit-shell") == 0) {
            free(comando);
            break;
        }

        // Comando interno: kill <PID>
        if (strcmp(argumentos[0], "kill") == 0) {

            if (quantidade != 2) {
                printf("Uso: kill <PID>\n");
                free(comando);
                continue;
            }

            char *fim;
            errno = 0;

            long numero = strtol(argumentos[1], &fim, 10);

            if (errno == ERANGE ||
                fim == argumentos[1] ||
                *fim != '\0' ||
                numero <= 0 ||
                numero > INT_MAX) {

                printf("PID inválido.\n");
                free(comando);
                continue;
            }

            pid_t pid = (pid_t)numero;

            if (encerrarProcesso(pid) == 0) {
                printf(
                    "Sinal de encerramento enviado ao PID %ld\n",
                    (long)pid
                );
            }

            free(comando);
            continue;
        }

        // Executa os demais comandos.
        pid_t pid = iniciarProcesso(
            argumentos[0],
            argumentos
        );

        if (pid == -1) {
            free(comando);
            continue;
        }

        // Aguarda o processo executado em primeiro plano.
        aguardarProcesso(pid);

        free(comando);
    }
}