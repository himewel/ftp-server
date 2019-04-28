#include "server_func.h"

ConnectionStatus *initializeStatus() {
  ConnectionStatus *c = (ConnectionStatus*) malloc(sizeof(ConnectionStatus));
  // Configura diretório atual
  strcat(c->actual_path, "./");

  return c;
};

/* DECODIFICAÇÃO DO COMANDO */

int decode_message (char *command) {
  char message[4];
  strncpy(message, command, 4);
  strlwr(message);
  int code;
  if (strcmp(message,"user") == 0) {
    code = 0;
  } else if (strcmp(message,"pass") == 0) {
    code = 1;
  } else if (strcmp(message,"acct") == 0) {
    code = 2;
  } else if (strcmp(message,"cwd") == 0 || strcmp(message,"cwd ") == 0) {
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

int number_words(char **m) {
  // Retorna número de elementos na mensagem já splitada
  int i = 0;
  for (int j = 0; j < MAX_ARGUMENTS; j++) {
    if (m[j][0] != 0) {
      i++;
    }
  }
  return i;
}

char **split_words(char *m) {
  // Separa palavras em vetor
  char *ptr = strtok(m, " ");

  char **res = (char**) malloc(MAX_ARGUMENTS*sizeof(char*));
  for (int i = 0; i < MAX_ARGUMENTS; i++) {
    res[i] = (char *) malloc(STRING_SIZE*sizeof(char));
    res[i][0] = 0;
  }

  for (int i = 0; ptr != NULL && i < MAX_ARGUMENTS; i++) {
    strcpy(res[i], ptr);
    ptr = strtok(NULL, " ");
  }
  return res;
}

/* CONTROLE DE ACESSO */

ConnectionStatus *func_user(ConnectionStatus *c, char *message) {
  // Recebe mensagem decodificada em espaços, alocada itens do vetor
  char **args = split_words(message);

  // Número de palavras dentro da mensagem
  int i = number_words(args);

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
  // Recebe mensagem decodificada em espaços, alocada itens do vetor
  char **args = split_words(message);

  // Número de palavras dentro da mensagem
  int i = number_words(args);

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

ConnectionStatus *func_cwd(ConnectionStatus *c, char *message) {
  // Recebe mensagem decodificada em espaços, alocada itens do vetor
  char **args = split_words(message);
  // Número de palavras dentro da mensagem
  int i = number_words(args);

  if (i == 2) {
    // Verifica a existência do diretório, caso positivo armazena na struct de
    // estado
    // Utiliza uma variável auxiliar para consulta da existencia do diretório
    char consult[STRING_SIZE];
    if (args[1][0] == '/') {
      // Caso destino seja /, não coloca outra / no final
      if (strcmp(args[1], "/") != 0) {
        strcpy(consult, args[1]);
      } else {
        strcpy(consult, "");
      }
    } else {
      // Concatena destino com caminho atual
      strcpy(consult, c->actual_path);
      strcat(consult, args[1]);
    }
    strcat(consult, "/");

    // Consulta existência do diretório
    DIR *dir = opendir(consult);
    if (dir != NULL) {
      strcpy(c->actual_path, consult);
      strcpy(c->return_message, "250 Requested file action okay, completed.");
      closedir(dir);
    }
    else {
      strcpy(c->return_message, "550 Requested action not taken.");
    }
  } else {
    strcpy(c->return_message, "501 Syntax error in parameters or arguments.");
  }

  return c;
}

ConnectionStatus *func_cdup(ConnectionStatus *c, char *message) {
  // Utiliza uma variável auxiliar para consulta da existencia do diretório
  char consult[STRING_SIZE];
  strcpy(consult, c->actual_path);
  // Concatena ../ nbo final do caminho
  strcat(consult, "../");

  // Consulta existência do diretório
  DIR *dir = opendir(consult);
  if (dir != NULL) {
    strcpy(c->actual_path, consult);
    strcpy(c->return_message, "200 Command okay.");
    closedir(dir);
  }
  else {
    strcpy(c->return_message, "550 Requested action not taken.");
  }

  return c;
}

ConnectionStatus *func_smnt(ConnectionStatus *c, char *message) {
  strcpy(c->return_message,"502 Command not implemented.");
  return c;
}
