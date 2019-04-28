#include "common.h"
#include <sys/stat.h>
#include <dirent.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_ARGUMENTS 5
#define STRING_SIZE 500

struct connection_status {
  char user[STRING_SIZE];
  int logged_on;
  char return_message[STRING_SIZE];
  char actual_path[STRING_SIZE];
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
