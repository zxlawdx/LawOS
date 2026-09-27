#define _POSIX_C_SOURCE 200809L

#include "completador.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include <readline/readline.h>

/* Gera sugestões de comandos a partir do PATH. */
static char *gerarComando(const char *texto, int estado)
{
    static char *caminhos = NULL;
    static char *cursor = NULL;
    static char *diretorio_atual = NULL;
    static DIR *diretorio = NULL;

    if (estado == 0) {

        if (diretorio)
            closedir(diretorio);

        free(diretorio_atual);
        free(caminhos);

        diretorio = NULL;
        diretorio_atual = NULL;

        const char *path = getenv("PATH");

        caminhos = strdup(path ? path : "");
        cursor = caminhos;
    }

    size_t tamanho = strlen(texto);

    while (cursor || diretorio) {

        if (!diretorio) {

            char *inicio = cursor;
            char *fim = strchr(cursor, ':');

            if (fim) {
                *fim = '\0';
                cursor = fim + 1;
            } else {
                cursor = NULL;
            }

            diretorio_atual = strdup(
                *inicio ? inicio : "."
            );

            if (!diretorio_atual)
                continue;

            diretorio = opendir(diretorio_atual);

            if (!diretorio) {
                free(diretorio_atual);
                diretorio_atual = NULL;
                continue;
            }
        }

        struct dirent *entrada;

        while ((entrada = readdir(diretorio)) != NULL) {

            if (strncmp(entrada->d_name, texto, tamanho) != 0)
                continue;

            size_t tamanho_path =
                strlen(diretorio_atual) +
                strlen(entrada->d_name) + 2;

            char *caminho = malloc(tamanho_path);

            if (!caminho)
                continue;

            snprintf(
                caminho,
                tamanho_path,
                "%s/%s",
                diretorio_atual,
                entrada->d_name
            );

            struct stat info;

            int executavel =
                stat(caminho, &info) == 0 &&
                S_ISREG(info.st_mode) &&
                access(caminho, X_OK) == 0;

            free(caminho);

            if (executavel)
                return strdup(entrada->d_name);
        }

        closedir(diretorio);
        diretorio = NULL;

        free(diretorio_atual);
        diretorio_atual = NULL;
    }

    free(caminhos);
    caminhos = NULL;

    return NULL;
}

/* Identifica quando completar comandos. */
static char **completar(
    const char *texto,
    int inicio,
    int fim
) {
    (void)fim;

    /*
     * Se já existe um comando antes da posição atual,
     * deixa a Readline completar arquivos normalmente.
     */
    for (int i = 0; i < inicio; i++) {
        if (!isspace((unsigned char)rl_line_buffer[i]))
            return NULL;
    }

    /*
     * Se o usuário digitou um caminho,
     * utiliza o autocompletar de arquivos.
     */
    if (strchr(texto, '/'))
        return NULL;

    return rl_completion_matches(
        texto,
        gerarComando
    );
}

void configurarAutocomplete(void)
{
    rl_attempted_completion_function = completar;
}