#include "common.h"

struct connection_status {
  char user[STRING_SIZE];
  char actual_path[STRING_SIZE];
  int logged_on;
  int connection_ok;
};

typedef struct connection_status ConnectionStatus;

ConnectionStatus *initializeStatus();

/* DECODIFICAÇÃO DO COMANDO */
int decode_message (char *command);
void strlwr (char *s);
int number_words(char **m);
char **split_words(char *m);

/* CONTROLE DE ACESSO */
char *func_user(ConnectionStatus *c, char *message);
char *func_pass(ConnectionStatus *c, char *message);
char *func_acct(ConnectionStatus *c, char *message);
char *func_cwd(ConnectionStatus *c, char *message);
char *func_cdup(ConnectionStatus *c, char *message);
char *func_smnt(ConnectionStatus *c, char *message);
char *func_rein(ConnectionStatus *c, char *message);
char *func_quit(ConnectionStatus *c, char *message);
char *func_list(ConnectionStatus *c, char *message);
char *func_pwd(ConnectionStatus *c,char *message);
char *func_mkd(ConnectionStatus *c,char *message);
char *func_rmd(ConnectionStatus *c, char *message);
char *func_noop(ConnectionStatus *c, char *message); 
