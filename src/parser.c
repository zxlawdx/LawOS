#include "parser.h"
#include <ctype.h>
#include <stddef.h>

/*
 * Interpreta a entrada do usuário.
 *
 * Suporta:
 * - Espaços entre argumentos
 * - Aspas simples e duplas
 * - Caracteres de escape
 * - Argumentos vazios
 *
 * Retorno:
 * >= 0: quantidade de argumentos
 * -1: aspas não fechadas ou escape incompleto
 * -2: quantidade máxima excedida
 */
int interpretarComando(
    const char *linha,
    char *buffer,
    char **argumentos
) {
    const char *leitura = linha;
    char *saida = buffer;

    int quantidade = 0;
    char aspas = '\0';

    while (*leitura) {

        // Ignora espaços.
        while (isspace((unsigned char)*leitura))
            leitura++;

        if (*leitura == '\0')
            break;

        if (quantidade >= MAX_ARGUMENTOS)
            return -2;

        // Registra o início do argumento.
        argumentos[quantidade] = saida;
        quantidade++;

        aspas = '\0';

        while (*leitura) {

            // Trata caracteres de escape.
            if (*leitura == '\\' && aspas != '\'') {

                leitura++;

                if (*leitura == '\0')
                    return -1;

                *saida++ = *leitura++;
                continue;
            }

            // Trata aspas simples e duplas.
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

            // Encerra o argumento ao encontrar espaço.
            if (aspas == '\0' &&
                isspace((unsigned char)*leitura)) {
                break;
            }

            *saida++ = *leitura++;
        }

        if (aspas != '\0')
            return -1;

        *saida++ = '\0';
    }

    // Finaliza o vetor de argumentos.
    argumentos[quantidade] = NULL;

    return quantidade;
}