#include "common.h"

/* DECODIFICAÇÃO DO COMANDO */
int decode_message (char *command);
void strlwr (char *s);
int number_words(char *m);

/* CONTROLE DE ACESSO */
char *func_user(char *message);
char *func_pass(char *message);
