#include "lawshell.h"
#include "processos.h"
#include "completador.h"
#include "parser.h"
#include "comandos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>


void shell(void **args) {

    (void)args;

    configurarAutocomplete();

    while (1) {

        char *comando = readline("lawOS@root: ");

        if (!comando)
            break;

        char *buffer = malloc(strlen(comando) + 1);

        if (!buffer) {
            perror("Erro ao alocar memória");
            free(comando);
            break;
        }

        char *argumentos[MAX_ARGUMENTOS + 1];

        int quantidade = interpretarComando(
            comando,
            buffer,
            argumentos
        );

        if (quantidade < 0) {

            fprintf(
                stderr,
                "Erro ao interpretar comando: %d\n",
                quantidade
            );

            free(buffer);
            free(comando);

            continue;
        }

        if (quantidade == 0) {

            free(buffer);
            free(comando);

            continue;
        }

        add_history(comando);


        /*
         * Procura o comando no registro.
         */
        ResultadoComando resultado =
            executarComandoInterno(
                quantidade,
                argumentos
            );


        /*
         * Encerra o interpretador.
         */
        if (resultado == COMANDO_SAIR) {

            free(buffer);
            free(comando);

            break;
        }


        /*
         * Executa programas externos.
         */
        if (resultado == COMANDO_EXTERNO) {

            pid_t pid = iniciarProcesso(
                argumentos[0],
                argumentos
            );

            if (pid > 0) {
                aguardarProcesso(pid);
            }
        }

        free(buffer);
        free(comando);
    }
}