#include "server_func.h"

ConnectionStatus *initializeStatus() {
  ConnectionStatus *c = (ConnectionStatus*) malloc(sizeof(ConnectionStatus));
  // Configura diretório atual
  getcwd(c->actual_path, sizeof(c->actual_path));
  strcat(c->actual_path, "/");
  strcpy(c->user, "");

  c->logged_on = 0;
  c->connection_ok = 1;
  return c;
};

/* DECODIFICAÇÃO DO COMANDO */

int decode_message (char *command) {
  char aux[STRING_SIZE];
  strcpy(aux, command);
  char **args = split_words(aux);

  strlwr(args[0]);

  int code;
  if (strcmp(args[0],"user") == 0) {
    code = 0;
  } else if (strcmp(args[0],"pass") == 0) {
    code = 1;
  } else if (strcmp(args[0],"acct") == 0) {
    code = 2;
  } else if (strcmp(args[0],"cwd") == 0) {
    code = 3;
  } else if (strcmp(args[0],"cdup") == 0) {
    code = 4;
  } else if (strcmp(args[0],"smnt") == 0) {
    code = 5;
  } else if (strcmp(args[0],"rein") == 0) {
    code = 6;
  } else if (strcmp(args[0],"quit") == 0) {
    code = 7;
  } else if (strcmp(args[0],"list") == 0) {
    code = 8;
  } else if (strcmp(args[0],"pwd") == 0) {
    code = 9;
  } else if (strcmp(args[0],"mkd") == 0) {
    code = 10;
  } else if (strcmp(args[0],"rmd") == 0) {
    code = 11;
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

char *func_user(ConnectionStatus *c, char *message) {
  // Recebe mensagem decodificada em espaços, alocada itens do vetor
  char **args = split_words(message);
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));

  // Número de palavras dentro da mensagem
  int i = number_words(args);

  // Controle de sintaxe através do número de argumentos
  if (i == 2) {
    if (c->logged_on != 1) {
      strcpy(c->user, args[1]);
      c->logged_on = 1;
      return_message = "230 User logged in, proceed.";
    } else {
      return_message = "503 You are already logged in.";
    }
  } else {
    return_message ="501 Syntax error in parameters or arguments.";
  }

  // Retorna novo estado da conexão
  return return_message;
}

char *func_pass(ConnectionStatus *c, char *message) {
  // Recebe mensagem decodificada em espaços, alocada itens do vetor
  char **args = split_words(message);
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));

  // Número de palavras dentro da mensagem
  int i = number_words(args);

  // Como o comando não será implementado, apenas será verificada quantidade de
  // argumentos
  if (i == 2) {
    return_message = "202 Command not implemented, superfluous at this site.";
  } else {
    return_message = "501 Syntax error in parameters or arguments.";
  }

  // Retorna novo estado da conexão
  return return_message;
}

char *func_acct(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  return_message = "230 User logged in, proceed.";
  return return_message;
}

char *func_cwd(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
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
      return_message = "200 Working directory changed.";
      closedir(dir);
    }
    else {
      return_message = "550 Requested action not taken.";
    }
  } else {
    return_message = "501 Syntax error in parameters or arguments.";
  }

  return return_message;
}

char *func_cdup(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  // Utiliza uma variável auxiliar para consulta da existencia do diretório
  char consult[STRING_SIZE];
  strcpy(consult, c->actual_path);
  // Concatena ../ nbo final do caminho
  strcat(consult, "../");

  // Consulta existência do diretório
  DIR *dir = opendir(consult);
  if (dir != NULL) {
    strcpy(c->actual_path, consult);
    return_message = "200 Command okay.";
    closedir(dir);
  }
  else {
    return_message = "550 Requested action not taken.";
  }

  return return_message;
}

char *func_smnt(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  return_message = "502 Command not implemented.";
  return return_message;
}

char *func_rein(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  free(c);
  c = initializeStatus();
  return_message = "220 Service ready for new user.";
  return return_message;
}

char *func_quit(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  c->connection_ok = 0;
  return_message = "221 Service closing control connection.";
  return return_message;
}

// Lista arquivos da pasta especificada(comando LIST)

char *func_list(ConnectionStatus *c,char *message) {
  chdir(c->actual_path);
  char aux[STRING_SIZE] = "dir ";
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  char *path = (char*) malloc(STRING_SIZE*sizeof(char));
  char *aux2 = (char*) malloc(STRING_SIZE*sizeof(char));
  FILE *arquivos;
  char ch;
  int i,j = 0;

  //retirando os 4 primeiros digitos (pois eles contem list )
  for (int i = 4;message[i]!= '\0';i++) {
    // nao queremos o primeiro espaço
    if(i != 4){
    path[j] = message[i];
    j++;
    }
  }

  // cria o sintax do comando dir
  // caso o cliente tenha setado uma pasta especica
  strcat(aux,path);

  // faz com que os erros sejam escritos na menssagem caso ocorra
  strcat(aux," 2>&1");

  //chama o comando no sistema e salva em um arquivo
  arquivos =  popen(aux,"r");

  //converte o arquivo para string
  for(i =0;((ch = fgetc(arquivos)) != EOF);i++){
     return_message[i] = ch;
   }

   // verificando se nao deu erro de diretorio nao encontrado
   // pegando as 4 primeiras letras da menssagem pois se deu erro no dir vai exibir dir:

   for (int i = 0;i<4;i++) {
     aux2[i] = return_message[i];
   }

   if(strcmp(aux2,"dir:") == 0){
     pclose(arquivos);
     return_message = "550 Path not found";
     return return_message;
   }

   // a menssagem de retorno possui um \n no final entao para podermos comparalas
   strcat(path,"\n");
   // tambem devemos checar a possibilidade de o cliente estar busando informacoes sobre um arquivo especifico
   if(strcmp(return_message,path) == 0){
     // caso ele esteja vamos utilizar o comando stat para retornar as informacoes
     free(aux2);
     aux2 = (char*) malloc(STRING_SIZE*sizeof(char));
     strcat(aux2,"stat ");
     strcat(aux2,c->actual_path);
     strcat(aux2,path);

     arquivos =  popen(aux2,"r");
     //converte o arquivo para string
     for(i =0;((ch = fgetc(arquivos)) != EOF);i++){
        return_message[i] = ch;
      }
     // se as primeiras 5 letras do nosso retorno for stat: quer dizer que nao e um arquivo
     // deveremos retornar uma menssagem de erro
     for (int i = 0;i<5;i++) {
       aux2[i] = return_message[i];
     }
     if(strcmp(aux2,"stat:") == 0){
       return_message = "550 File not found";
       return return_message;
     }
     else{
       pclose(arquivos);
       return return_message;
     }
   }

  pclose(arquivos);
  return return_message;
}

char *func_pwd(ConnectionStatus *c,char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  // Muda diretório raiz para o determinado na struct
  chdir(c->actual_path);
  // Pega nome real do diretório
  getcwd(c->actual_path, sizeof(c->actual_path));
  strcat(c->actual_path, "/");
  // Formata string de resposta
  char aux[STRING_SIZE];
  strcpy(aux, "257 \"");
  strcat(aux, c->actual_path);
  strcat(aux, "\" path name.");
  return_message = aux;

  return return_message;
}

char *func_mkd(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  // Recebe mensagem decodificada em espaços, alocada itens do vetor
  char **args = split_words(message);
  // Número de palavras dentro da mensagem
  int i = number_words(args);

  if (i == 2) {
    char aux[STRING_SIZE];
    if (args[1][0] == '/') {
      strcpy(aux, args[1]);
    } else {
      // Concatena destino com caminho atual
      strcpy(aux, c->actual_path);
      strcat(aux, args[1]);
    }

    int err = mkdir(aux, 0775);
    // Verifica se houve erro
    if (err == 0) {
      char aux[STRING_SIZE];
      strcpy(aux, "257 \"");
      strcat(aux, args[1]);
      strcat(aux, "\" directory created.");
      return_message = aux;
    } else {
      return_message = "550 Requested action not taken.";
    }
  } else {
    return_message = "501 Syntax error in parameters or arguments.";
  }

  return return_message;

}

char *func_rmd(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  // Recebe mensagem decodificada em espaços, alocada itens do vetor
  char **args = split_words(message);
  // Número de palavras dentro da mensagem
  int i = number_words(args);

  if (i == 2) {
    char aux[STRING_SIZE];
    if (args[1][0] == '/') {
      strcpy(aux, args[1]);
    } else {
      // Concatena destino com caminho atual
      strcpy(aux, c->actual_path);
      strcat(aux, args[1]);
    }

    int err = rmdir(aux);
    // Verifica se houve erro
    if (err == 0) {
      return_message = "250 Requested file action okay, completed.";
    } else {
      return_message = "550 Requested action not taken.";
    }
  } else {
    return_message = "501 Syntax error in parameters or arguments.";
  }

  return return_message;
}
