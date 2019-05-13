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
#include <time.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <signal.h>

#define PORTNUM 2300
#define MAX_ARGUMENTS 5
#define STRING_SIZE 200
#define BUF_SIZE 8192

#define YEL "\x1B[33m"
#define RED  "\x1B[31m"
#define NRM  "\x1B[0m"
#define BLU  "\x1B[34m"
#define GRN  "\x1B[32m"

struct connection_status {
  char actual_path[STRING_SIZE];
  int connection_ok;
  int data_session;
  int data_session_port;
  int control_session;
  char type;
  char *server_address;
  int modo_passivo;
};

typedef struct connection_status ConnectionStatus;

/* INICIALIZAÇÃO DA CONEXÃO */
ConnectionStatus *initializeStatus();
char *getIPaddress(char *interface);
int createSocketToServe(char *address, int port);
int createConnectionToAccept(int socket);
int createConnectionToConnect(int socket, char* address, int port);

/* DECODIFICAÇÃO DO COMANDO */
int decode_message (char *command);
void strlwr (char *s);
int number_words(char **m);
char **split_words(char *m, char *limit);
int hex_to_dec(char *hex, int n);
char *dec_to_hex(int dec, int n);

/* CONTROLE DE ACESSO */
char *func_user(ConnectionStatus *c, char *message);
char *func_pass(ConnectionStatus *c, char *message);
char *func_cwd(ConnectionStatus *c, char *message);
char *func_cdup(ConnectionStatus *c, char *message);
char *func_quit(ConnectionStatus *c, char *message);

/* PARÂMETROS DE TRANSFERÊNCIA */
char *func_port(ConnectionStatus *c, char *message);
char *func_type(ConnectionStatus *c, char *message);
char *func_pasv(ConnectionStatus *c, char *message);

/* COMANDOS FTP */
char *func_list(ConnectionStatus *c, char *message);
char *func_pwd(ConnectionStatus *c,char *message);
char *func_mkd(ConnectionStatus *c,char *message);
char *func_rmd(ConnectionStatus *c, char *message);
char *func_noop(ConnectionStatus *c, char *message);
char *func_syst(ConnectionStatus *c, char *message);
char *func_retr(ConnectionStatus *c, char *message);
char *func_stor(ConnectionStatus *c, char *message);
