#include "common.h"

#define MAX_ARGUMENTS 5
#define STRING_SIZE 500

struct connection_status {
  char user[STRING_SIZE];
  int logged_on;
  char return_message[STRING_SIZE];
};

typedef struct connection_status ConnectionStatus;

/* DECODIFICAÇÃO DO COMANDO */
int decode_message (char *command);
void strlwr (char *s);
int number_words(char *m);
char **split_words(char *m);

/* CONTROLE DE ACESSO */
ConnectionStatus *func_user(ConnectionStatus *c, char *message);
ConnectionStatus *func_pass(ConnectionStatus *c, char *message);
