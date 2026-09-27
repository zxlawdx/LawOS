#ifndef LAWOS_PARSER_H
#define LAWOS_PARSER_H

#define MAX_ARGUMENTOS 64

int interpretarComando(
    const char *linha,
    char *buffer,
    char **argumentos
);

#endif