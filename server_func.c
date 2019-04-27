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
   int c = 0;
   while (s[c] != '\0') {
      if (s[c] >= 'A' && s[c] <= 'Z') {
         s[c] = s[c] + 32;
      }
      c++;
   }
}

int number_words(char *m) {
  char *ptr = strtok(m, " ");
  int i;
  for (i = 0; ptr != NULL; i++) {
    ptr = strtok(NULL, " ");
  }
  return i;
}

/* CONTROLE DE ACESSO */

char *func_user(char *message) {
  int i = number_words(message);
  if (i == 2) {
    return "230 User logged in, proceed.";
  } else {
    return "501 Syntax error in parameters or arguments.";
  }
}

char *func_pass(char *message) {
  int i = number_words(message);
  if (i == 2) {
    return "202 Command not implemented, superfluous at this site.";
  } else {
    return "501 Syntax error in parameters or arguments.";
  }
}
