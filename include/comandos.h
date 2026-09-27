#ifndef LAWOS_COMANDOS_H
#define LAWOS_COMANDOS_H

typedef enum {
    COMANDO_EXTERNO,
    COMANDO_INTERNO,
    COMANDO_SAIR
} ResultadoComando;

ResultadoComando executarComandoInterno(
    int argc,
    char *const argv[]
);

#endif