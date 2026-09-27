#include "lawshell.h"
#include "processos.h"
#include "completador.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#include <readline/readline.h>
#include <readline/history.h>

#define MAX_ARGUMENTOS 64


/*
 * Separa a linha digitada em argumentos.
 *
 * Suporta:
 * - Espaços entre argumentos
 * - Aspas simples
 * - Aspas duplas
 * - Caracteres de escape (\)
 * - Argumentos vazios ("")
 *
 * Retorno:
 * >= 0: quantidade de argumentos
 * -1: aspas não fechadas ou escape incompleto
 * -2: quantidade máxima excedida
 *
 * O buffer deve ter pelo menos strlen(linha) + 1 bytes.
 * Os argumentos apontam para posições dentro dele.
 */
static int interpretarComando(
    const char *linha,
    char *buffer,
    char **argumentos
) {
    const char *leitura = linha;
    char *saida = buffer;

    int quantidade = 0;
    char aspas = '\0';

    while (*leitura) {

        // Ignora espaços antes de cada argumento.
        while (isspace((unsigned char)*leitura))
            leitura++;

        if (*leitura == '\0')
            break;

        if (quantidade >= MAX_ARGUMENTOS)
            return -2;

        // Guarda o início do argumento.
        argumentos[quantidade] = saida;
        quantidade++;

        aspas = '\0';

        while (*leitura) {

            // Escape fora de aspas simples.
            if (*leitura == '\\' && aspas != '\'') {

                leitura++;

                if (*leitura == '\0')
                    return -1;

                *saida++ = *leitura++;
                continue;
            }

            // Identifica abertura e fechamento de aspas.
            if (*leitura == '\'' || *leitura == '"') {

                if (aspas == '\0') {
                    aspas = *leitura;
                    leitura++;
                    continue;
                }

                if (aspas == *leitura) {
                    aspas = '\0';
                    leitura++;
                    continue;
                }
            }

            // Espaço fora de aspas encerra o argumento.
            if (aspas == '\0' &&
                isspace((unsigned char)*leitura)) {
                break;
            }

            // Copia o caractere para o argumento.
            *saida++ = *leitura++;
        }

        // Detecta aspas que não foram fechadas.
        if (aspas != '\0')
            return -1;

        // Finaliza a string do argumento.
        *saida++ = '\0';
    }

    // execvp exige um NULL no final do vetor.
    argumentos[quantidade] = NULL;

    return quantidade;
}


/*
 * Interpretador de comandos do LawOS.
 */
void shell(void **args) {

    (void)args;

    configurarAutocomplete();

    while (1) {

        char *comando = readline("lawOS@root: ");

        // Ctrl + D encerra o LawShell.
        if (!comando)
            break;

        // Reserva memória para os argumentos.
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

        // Trata erros de interpretação.
        if (quantidade == -1) {

            fprintf(
                stderr,
                "LawShell: aspas não fechadas "
                "ou escape incompleto.\n"
            );

            free(buffer);
            free(comando);
            continue;
        }

        if (quantidade == -2) {

            fprintf(
                stderr,
                "LawShell: número máximo "
                "de argumentos excedido.\n"
            );

            free(buffer);
            free(comando);
            continue;
        }

        // Ignora comandos vazios.
        if (quantidade == 0) {

            free(buffer);
            free(comando);
            continue;
        }

        // Adiciona a linha original ao histórico.
        add_history(comando);


        /*
         * Comandos internos do LawShell.
         */

        // Encerrar o shell.
        if (strcmp(argumentos[0], "exit-shell") == 0) {

            free(buffer);
            free(comando);
            break;
        }


        /*
         * Encerramento de processos.
         *
         * Uso: kill <PID>
         */
        if (strcmp(argumentos[0], "kill") == 0) {

            if (quantidade != 2) {

                printf("Uso: kill <PID>\n");

                free(buffer);
                free(comando);
                continue;
            }

            char *fim;
            errno = 0;

            long numero = strtol(
                argumentos[1],
                &fim,
                10
            );

            // Validação do PID.
            if (errno == ERANGE ||
                fim == argumentos[1] ||
                *fim != '\0' ||
                numero <= 0 ||
                numero > INT_MAX) {

                printf("PID inválido.\n");

                free(buffer);
                free(comando);
                continue;
            }

            pid_t pid = (pid_t)numero;

            if (encerrarProcesso(pid) == 0) {

                printf(
                    "Sinal enviado ao processo %ld\n",
                    (long)pid
                );
            }

            free(buffer);
            free(comando);
            continue;
        }


        /*
         * Executa programas externos.
         */
        pid_t pid = iniciarProcesso(
            argumentos[0],
            argumentos
        );

        if (pid == -1) {

            free(buffer);
            free(comando);
            continue;
        }

        // Execução em primeiro plano.
        aguardarProcesso(pid);

        free(buffer);
        free(comando);
    }
}