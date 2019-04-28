#include "server_func.h"

/* DECODIFICAÇÃO DO COMANDO */

int decode_message (char *command) {
  char message[4];
  strlwr(command);
  strncpy(message, command, 4);
  int code;
  if (strcmp(message,"user") == 0) {
    code = 0;
  } else if (strcmp(message,"pass") == 0) {
    code = 1;
  } else if (strcmp(message,"acct") == 0) {
    code = 2;
  } else if (strcmp(message,"cwd") == 0) {
    code = 3;
  } else if (strcmp(message,"cdup") == 0) {
    code = 4;
  } else if (strcmp(message,"smnt") == 0) {
    code = 5;
  } else if (strcmp(message,"rein") == 0) {
    code = 6;
  } else if (strcmp(message,"quit") == 0) {
    code = 7;
  } else {
    code = -1000;
  }
  return code;
}

void strlwr (char s[]) {
  // Tranforma caracteres da string em caixa baixa
  int c = 0;
  while (s[c] != '\0') {
    if (s[c] >= 'A' && s[c] <= 'Z') {
       s[c] = s[c] + 32;
    }
    c++;
  }
}

int number_words(char *m) {
  // Retorna número de palavras na string
  char *ptr = strtok(m, " ");
  int i;
  for (i = 0; ptr != NULL; i++) {
    ptr = strtok(NULL, " ");
  }
  return i;
}

char **split_words(char *m) {
  // Separa palavras em vetor
  char *ptr = strtok(m, " ");

  char **res = (char**) malloc(MAX_ARGUMENTS*sizeof(char*));
  for (int i = 0; i < MAX_ARGUMENTS; i++) {
    res[i] = (char *) malloc(STRING_SIZE*sizeof(char));
  }

  for (int i = 0; ptr != NULL && i < MAX_ARGUMENTS; i++) {
    strcpy(res[i], ptr);
    ptr = strtok(NULL, " ");
  }
  return res;
}

/* CONTROLE DE ACESSO */

ConnectionStatus *func_user(ConnectionStatus *c, char *message) {
  // Número de palavras dentro da mensagem
  int i = number_words(message);

  // Recebe mensagem decodificada em espaços, alocada itens do vetor
  char **args = (char**) malloc(MAX_ARGUMENTS*sizeof(char*));
  for (int j = 0; j < MAX_ARGUMENTS; j++) {
    args[j] = (char *) malloc(STRING_SIZE*sizeof(char));
  }
  args = split_words(message);

  // Controle de sintaxe através do número de argumentos
  if (i == 2) {
    if (c->logged_on != 1) {
      strcpy(c->user, args[1]);
      c->logged_on = 1;
      strcpy(c->return_message, "230 User logged in, proceed.");
    } else {
      strcpy(c->return_message, "503 You are already logged in.");
    }
  } else {
    strcpy(c->return_message, "501 Syntax error in parameters or arguments.");
  }

  // Retorna novo estado da conexão
  return c;
}

ConnectionStatus *func_pass(ConnectionStatus *c, char *message) {
  // Número de palavras dentro da mensagem
  int i = number_words(message);

  // Como o comando não será implementado, apenas será verificada quantidade de
  // argumentos
  if (i == 2) {
    strcpy(c->return_message, "202 Command not implemented, superfluous at this site.");
  } else {
    strcpy(c->return_message, "501 Syntax error in parameters or arguments.");
  }

  // Retorna novo estado da conexão
  return c;
}

ConnectionStatus *func_acct(ConnectionStatus *c, char *message) {
  strcpy(c->return_message,"230 User logged in, proceed.");
  return c;
}
