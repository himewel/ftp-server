#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#define PORTNUM 2300
#define MAX_ARGUMENTS 5
#define STRING_SIZE 200

struct connection_status {
  char actual_path[STRING_SIZE];
  int connection_ok;
  int data_session;
  int data_session_port;
  int control_session;
};

typedef struct connection_status ConnectionStatus;

ConnectionStatus *initializeStatus();

/* DECODIFICAÇÃO DO COMANDO */
int decode_message (char *command);
void strlwr (char *s);
int number_words(char **m);
char **split_words(char *m, char *limit);
int hex_to_dec(char *hex);
char *dec_to_hex(int dec);

/* TRANSFERÊNCIA DE DADOS */
void send_data(ConnectionStatus *c, char *mensagem);

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
char *func_syst(ConnectionStatus *c, char *message);
char *func_port(ConnectionStatus *c, char *message);
