#include "server_func.h"

ConnectionStatus *initializeStatus() {
  ConnectionStatus *c = (ConnectionStatus*) malloc(sizeof(ConnectionStatus));
  // Configura diretório atual
  getcwd(c->actual_path, sizeof(c->actual_path));
  strcat(c->actual_path, "/");

  c->connection_ok = 1;
  c->data_session = -1;
  c->data_session_port = -1;
  c->control_session = 0;
  c->type = 'A';
  return c;
};

/* ENVIO DE DADOS */

void send_data(ConnectionStatus *c, char *mensagem) {
  int client_s;
  int w;
  char *mes;

  // Conecta com cliente
  struct sockaddr_in dest;
  bzero(&dest, sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_port = htons(c->data_session_port);
  dest.sin_addr.s_addr = INADDR_ANY;
  client_s = connect(c->data_session, (struct sockaddr*)&dest, sizeof(dest));
  if (client_s == -1) {
    int erro = errno;
    printf("%i\n", erro);
    mes = "425 Can't open data connection.\n";
    write(c->control_session, mes, strlen(mes));
    return;
  }

  // Informa início da transferência
  mes = "125 Data connection already open; transfer starting.\n";
  printf("%s", mes);
  write(c->control_session, mes, strlen(mes));

  // Envia dado
  w = write(c->data_session, mensagem, strlen(mensagem));
  printf("%s", mensagem);
  if (w == -1) {
    int erro = errno;
    printf("%i\n", erro);
    mes = "425 Can't open data connection.\n";
    write(c->control_session, mes, strlen(mes));
    return;
  }
  // Fecha conexão
  mes = "226 Closing data connection.\n";
  write(c->control_session, mes, strlen(mes));
  shutdown(c->data_session, SHUT_RDWR);
  close(client_s);
  c->data_session = -1;
  return;
}

/* DECODIFICAÇÃO DO COMANDO */

int decode_message (char *command) {
  char aux[STRING_SIZE];
  strcpy(aux, command);
  char **args = split_words(aux, " ");
  strlwr(args[0]);
  args[0][4] = 0;
  int code;
  if (strcmp(args[0],"user") == 0) {
    code = 0;
  } else if (strcmp(args[0],"pass") == 0) {
    code = 1;
  } else if (strcmp(args[0],"acct") == 0) {
    code = 2;
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
  } else if (strcmp(args[0],"syst") == 0) {
    code = 13;
  } else if (strcmp(args[0],"port") == 0) {
    code = 14;
  } else if (strcmp(args[0],"type") == 0) {
    code = 15;
  } else if (strcmp(args[0],"pasv") == 0) {
    code = 16;
  } else {
    code = -1000;
  }

  if (code == -1000) {
    args[0][3] = 0;
    if (strcmp(args[0],"cwd") == 0) {
      code = 3;
    } else if (strcmp(args[0],"pwd") == 0) {
      code = 9;
    } else if (strcmp(args[0],"mkd") == 0) {
      code = 10;
    } else if (strcmp(args[0],"rmd") == 0) {
      code = 11;
    }
  }

  for (int i = 0; i < MAX_ARGUMENTS; i++) {
    free(args[i]);
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

char **split_words(char *m, char *limit) {
  // Separa palavras em vetor
  char *ptr = strtok(m, limit);

  char **res = (char**) malloc(10*sizeof(char*));
  for (int i = 0; i < 10; i++) {
    res[i] = (char *) malloc(STRING_SIZE*sizeof(char));
    res[i][0] = 0;
  }

  for (int i = 0; ptr != NULL && i < 10; i++) {
    if (ptr[strlen(ptr) - 1] == '\n') {
      ptr[strlen(ptr) - 1] = 0;
    }
    if (ptr[strlen(ptr) - 1] == '\r') {
      ptr[strlen(ptr) - 1] = 0;
    }
    strcpy(res[i], ptr);
    ptr = strtok(NULL, limit);
  }
  return res;
}

int hex_to_dec(char *hex, int n) {
  int val;
  int decimal = 0;
  int pow = 1;

  for (int i = n-1; i >= 0; i--) {
    if (hex[i] >= '0' && hex[i] <= '9') {
        val = hex[i] - 48;
    }
    if (hex[i] >= 'A' && hex[i] <= 'F') {
        val = hex[i] - 55;
    }
    decimal += val * pow;
    pow *= 16;
  }
  return decimal;
}

char *dec_to_hex(int dec, int n) {
  char HEXVALUE[] = {'0', '1', '2', '3', '4', '5', '6','7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  char *hex = (char*) malloc(n*sizeof(char));

  for (int i = n-1; i >= 0; i--) {
    hex[i] = HEXVALUE[dec % 16];
    if (dec > 0) {
      dec /= 16;
    }
  }

  return hex;
}

/* CONTROLE DE ACESSO */

char *func_user(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  return_message = "331 User name okay, need password.\n";
  return return_message;
}

char *func_pass(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  return_message = "230 User logged in, proceed.\n";
  return return_message;
}

char *func_acct(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  return_message = "230 User logged in, proceed.\n";
  return return_message;
}

char *func_cwd(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  // Recebe mensagem decodificada em espaços, alocada itens do vetor
  char **args = split_words(message, " ");

  // Verifica a existência do diretório, caso positivo armazena na struct de
  // estado
  // Utiliza uma variável auxiliar para consulta da existencia do diretório
  char consult[STRING_SIZE] = "";
  if (args[1][0] == '/') {
    // Vai para raiz do servidor
    args[1][strlen(args[1]) - 1] = '/';
    strcpy(consult, args[1]);
  } else {
    // Concatena destino com caminho atual
    strcpy(consult, c->actual_path);
    args[1][strlen(args[1]) - 1] = '/';
    strcat(consult, args[1]);
  }

  // Consulta existência do diretório
  DIR *dir = opendir(consult);
  if (dir != NULL) {
    strcpy(c->actual_path, consult);
    return_message = "200 Working directory changed.\n";
    closedir(dir);
  }
  else {
    return_message = "550 Requested action not taken.\n";
  }

  // libera variáveis
  for (int i = 0; i < 10; i++) {
    free(args[i]);
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
    return_message = "200 Command okay.\n";
    closedir(dir);
  }
  else {
    return_message = "550 Requested action not taken.\n";
  }

  return return_message;
}

char *func_smnt(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  return_message = "502 Command not implemented.\n";
  return return_message;
}

char *func_rein(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  free(c);
  c = initializeStatus();
  return_message = "220 Service ready for new user.\n";
  return return_message;
}

char *func_quit(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  c->connection_ok = 0;
  return_message = "221 Service closing control connection.\n";
  return return_message;
}

// PARÂMETROS DE TRANSFERÊNCIA

char *func_port(ConnectionStatus *c, char *message) {
  // Decodificação das strings
  char **words = split_words(message, " ");
  char **args = split_words(words[1], ",");
  char *ini = dec_to_hex(strtol(args[4], NULL, 10), 2);
  char *fim = dec_to_hex(strtol(args[5], NULL, 10), 2);
  char *concat = (char*) malloc(4*sizeof(char));
  concat[0] = ini[0];
  concat[1] = ini[1];
  concat[2] = fim[0];
  concat[3] = fim[1];
  int porta = hex_to_dec(concat, 4);

  // Configuração do socket
  int client_s, s;
  struct sockaddr_in dest;
  bzero(&dest, sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_port = htons(c->data_session_port);
  dest.sin_addr.s_addr = INADDR_ANY;

  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
  s = socket(AF_INET, SOCK_STREAM, 0);

  if (s == -1) {
    int erro = errno;
    printf("Errno: %i\n", erro);
    return "421 Service not available, closing control connection.\n";
  }

  // Atualiza status da conexão
  c->data_session_port = porta;
  c->data_session = s;

  return "200 Command okay.\n";
}

char *func_type(ConnectionStatus *c, char *message) {
  // Decodifica mensagem
  char **args = split_words(message, " ");
  printf("%s\n", args[1]);
  // Somente ASCII implementada na versão mínima
  if (strcmp(args[1], "A") == 0) {
    c->type = 'A';
    return "200 Command okay.\n";
  } else {
    return "504 Command not implemented for that parameter.\n";
  }
}

char *func_pasv(ConnectionStatus *c, char *message) {
  // define porta para conexão aleatória
  srand(time(NULL));
  int porta = (rand() % 65035) + 5000;
  printf("%i\n",porta);

  // Configuração do socket
  int client_s, s;
  struct sockaddr_in dest, client;
  int addr_len = sizeof(client);
  bzero(&dest, sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_port = htons(porta);
  dest.sin_addr.s_addr = INADDR_ANY;

  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
  s = socket(AF_INET, SOCK_STREAM, 0);
  bind(s, (struct sockaddr*)&dest, sizeof(dest));
  listen(s, 5);


  if (s == -1) {
    int erro = errno;
    printf("Errno: %i\n", erro);
    return "421 Service not available, closing control connection.\n";
  }

  // Atualiza status da conexão
  c->data_session_port = porta;
  c->data_session = s;

  // Formata mensagem para decodificação do cliente
  char *hex = dec_to_hex(porta, 4);
  printf("%s\n",hex);
  char ini[2];
  char fim[2];
  ini[0] = hex[0];
  ini[1] = hex[1];
  fim[0] = hex[2];
  fim[1] = hex[3];
  int a = hex_to_dec(ini, 2);
  int b = hex_to_dec(fim, 2);
  printf("%i %i\n",a,b);

  char *return_message = (char *) malloc(STRING_SIZE*sizeof(char));
  sprintf(return_message, "227 Entering Passive Mode (127,0,0,1,%i,%i).\n",a,b);
  printf("%s\n",return_message);
  write(c->control_session, return_message, strlen(return_message));

  client_s = accept(s, (struct sockaddr*)&client, &addr_len);
  char *msg = (char*) malloc(STRING_SIZE*sizeof(char));
  write (client_s, msg, strlen(msg)+1);
  printf("%s\n",msg);

  return return_message;
}

// COMANDOS FTP

// Lista arquivos da pasta especificada(comando LIST)
char *func_list(ConnectionStatus *c,char *message) {
  chdir(c->actual_path);
  char shell_command[STRING_SIZE] = "dir ";
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  char **args = split_words(message, " ");
  char *path = args[1];
  FILE *arquivos;
  char ch;

  // cria o sintax do comando dir
  // caso o cliente tenha setado uma pasta especifica
  strcat(shell_command, path);

  // faz com que os erros sejam escritos na menssagem caso ocorra
  strcat(shell_command," 2>&1");
  printf("%s \n", shell_command);

  //chama o comando no sistema e salva em um arquivo
  arquivos = popen(shell_command, "r");

  //converte o arquivo para string
  for (int i = 0; ((ch = fgetc(arquivos)) != EOF); i++){
    return_message[i] = ch;
  }
  // verificando se nao deu erro de diretorio nao encontrado
  // pegando as 4 primeiras letras da menssagem pois se deu erro no dir vai exibir dir:
  if (strncmp(return_message, "dir:", 4) == 0){
    pclose(arquivos);
    return_message = "550 Path not found\n";
    return return_message;
  }

  // a menssagem de retorno possui um \n no final entao para podermos comparalas
  strcat(path,"\n");
  // tambem devemos checar a possibilidade de o cliente estar busando informacoes sobre um arquivo especifico
  if (strcmp(return_message, path) == 0){
    // caso ele esteja vamos utilizar o comando stat para retornar as informacoes
    strcpy(shell_command, "stat ");
    strcat(shell_command, c->actual_path);
    strcat(shell_command, path);

    arquivos = popen(shell_command,"r");
    //converte o arquivo para string
    for (int i = 0; ((ch = fgetc(arquivos)) != EOF); i++){
      return_message[i] = ch;
    }
    // se as primeiras 5 letras do nosso retorno for stat: quer dizer que nao e um arquivo
    // deveremos retornar uma menssagem de erro
    if (strncmp(return_message, "stat:", 5) == 0){
      return_message = "550 File not found\n";
      return return_message;
    }
  }

  send_data(c, return_message);

  pclose(arquivos);
  return "";
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
  strcat(aux, "\" path name.\n");
  return_message = aux;

  return return_message;
}

char *func_mkd(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  // Recebe mensagem decodificada em espaços, alocada itens do vetor
  char **args = split_words(message, " ");

  char shell_command[STRING_SIZE];
  if (args[1][0] == '/') {
    strcpy(shell_command, args[1]);
  } else {
    // Concatena destino com caminho atual
    strcpy(shell_command, c->actual_path);
    strcat(shell_command, args[1]);
  }

  printf("%s\n", shell_command);

  int err = mkdir(shell_command, 0775);
  // Verifica se houve erro
  if (err == 0) {
    char aux[STRING_SIZE];
    strcpy(aux, "257 \"");
    strcat(aux, args[1]);
    strcat(aux, "\" directory created.\n");
    return_message = aux;
  } else {
    return_message = "550 Requested action not taken.\n";
  }

  for (int i = 0; i < 10; i++) {
    free(args[i]);
  }

  return return_message;
}

char *func_rmd(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  // Recebe mensagem decodificada em espaços, alocada itens do vetor
  char **args = split_words(message, " ");
  // Número de palavras dentro da mensagem
  char path[STRING_SIZE];
  if (args[1][0] == '/') {
    strcpy(path, args[1]);
  } else {
    // Concatena destino com caminho atual
    strcpy(path, c->actual_path);
    strcat(path, args[1]);
  }
  printf("%s\n",path);
  // Verifica se houve erro
  if (rmdir(path) == 0) {
    return_message = "250 Requested file action okay, completed.\n";
  } else {
    return_message = "550 Requested action not taken.\n";
  }
  return return_message;
}

char *func_noop(ConnectionStatus *c, char *message) {
    char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
    return_message = "OK \n";
    return return_message;
}

char *func_syst(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  return_message = "215 UNIX system type.\n";
  return return_message;
}
