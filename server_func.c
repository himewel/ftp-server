#include "server_func.h"

/* Inicialização da conexão */

ConnectionStatus *initializeStatus() {
  ConnectionStatus *c = (ConnectionStatus*) malloc(sizeof(ConnectionStatus));
  // Configura diretório atual
  getcwd(c->actual_path, sizeof(c->actual_path));
  strcat(c->actual_path, "/");

  c->connection_ok = 1;
  c->data_session = -1;
  c->data_session_port = -1;
  c->control_session = -1;
  c->type = 'A';
  c->modo_passivo = 0;

  return c;
};

char *getIPaddress(char *interface) {
  // Detectando IP
  int fd;
  int s;
  struct ifreq ifr;
  fd = socket(AF_INET, SOCK_DGRAM, 0);
  // Determina que o endereço é IPv4
  ifr.ifr_addr.sa_family = AF_INET;
  // Pega endereço na interface de rede selecionada
  strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);
  s = ioctl(fd, SIOCGIFADDR, &ifr);
  if (s == -1) {
    printf("%s%c[1mErro: Interface selecionada não está disponível.%s\n",RED,27,NRM);
    return "";
  }
  close(fd);
  return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

int createSocketToServe(char *address, int port) {
  // Configuração do socket para receber solicitações
  struct sockaddr_in self;
  bzero(&self, sizeof(self));
  self.sin_family = AF_INET;
  self.sin_port = htons(port);
  self.sin_addr.s_addr = inet_addr(address);

  int s = socket(AF_INET, SOCK_STREAM, 0);
  int b = bind(s, (struct sockaddr*)&self, sizeof(self));
  int l = listen(s, 4);

  return (b < 0 || l < 0 || s <0) ? -1 : s;
}

int createConnectionToAccept(int socket) {
  struct sockaddr_in client;
  int addr_len = sizeof(client);
  int client_s = accept(socket, (struct sockaddr*)&client, &addr_len);
  return client_s;
}

int createConnectionToConnect(int socket, char* address, int port) {
  // Conecta com cliente
  struct sockaddr_in dest;
  bzero(&dest, sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_port = htons(port);
  dest.sin_addr.s_addr = inet_addr(address);
  int client_s = connect(socket, (struct sockaddr*)&dest, sizeof(dest));
  return client_s;
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
  } else if (strcmp(args[0],"cdup") == 0) {
    code = 2;
  } else if (strcmp(args[0],"quit") == 0) {
    code = 3;
  } else if (strcmp(args[0],"list") == 0) {
    code = 4;
  } else if (strcmp(args[0],"syst") == 0) {
    code = 5;
  } else if (strcmp(args[0],"port") == 0) {
    code = 6;
  } else if (strcmp(args[0],"type") == 0) {
    code = 7;
  } else if (strcmp(args[0],"pasv") == 0) {
    code = 8;
  } else if (strcmp(args[0],"retr") == 0) {
    code = 9;
  } else if (strcmp(args[0],"stor") == 0) {
    code = 10;
  } else if (strcmp(args[0],"noop") == 0) {
    code = 11;
  } else {
    code = -1000;
  }

  if (code == -1000) {
    args[0][3] = 0;
    if (strcmp(args[0],"cwd") == 0) {
      code = 20;
    } else if (strcmp(args[0],"pwd") == 0) {
      code = 21;
    } else if (strcmp(args[0],"mkd") == 0) {
      code = 22;
    } else if (strcmp(args[0],"rmd") == 0) {
      code = 23;
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
    sprintf(args[1], "%s/",args[1]);
    strcpy(consult, args[1]);
  } else {
    // Concatena destino com caminho atual
    strcpy(consult, c->actual_path);
    sprintf(consult, "%s%s/",consult,args[1]);
    //strcat(consult, args[1]);
  }
  printf("%s\n",consult);

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

char *func_quit(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  c->connection_ok = 0;
  return_message = "221 Service closing control connection.\n";
  printf("%s",return_message);
  printf("%s--------------------------------------------------------------------------------%s\n",GRN,NRM);
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
  int s;
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
  c->modo_passivo = 0;
  printf("%i\n",porta);

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
  //srand(time(NULL));
  int porta = (rand() % 50035) + 15500;
  printf("%i\n",porta);

  // Configuração do socket
  int client_s, s;
  struct sockaddr_in dest, client;
  int addr_len = sizeof(client);
  bzero(&dest, sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_port = htons(porta);
  dest.sin_addr.s_addr = inet_addr(c->server_address);

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
  c->modo_passivo = 1;

  // Formata mensagem para decodificação do cliente
  char hex[4];
  sprintf(hex, "%X", porta);
  printf("%s\n",hex);
  char ini[3];
  char fim[3];
  sprintf(ini, "%c%c",hex[0],hex[1]);
  sprintf(fim, "%c%c",hex[2],hex[3]);
  int a = (int)strtol(ini, NULL, 16);
  int b = (int)strtol(fim, NULL, 16);
  printf("%i %i\n",a,b);

  char *return_message = (char *) malloc(STRING_SIZE*sizeof(char));
  char ip[STRING_SIZE];
  strcpy(ip,c->server_address);
  char **ip_aux = split_words(ip, ".");
  printf("%s %s %s %s\n",ip_aux[0],ip_aux[1],ip_aux[2],ip_aux[3]);
  sprintf(return_message, "227 Entering Passive Mode (%s,%s,%s,%s,%i,%i).\n",ip_aux[0],ip_aux[1],ip_aux[2],ip_aux[3],a,b);

  return return_message;
}

// COMANDOS FTP

// Lista arquivos da pasta especificada(comando LIST)
char *func_list(ConnectionStatus *c,char *message) {
  chdir(c->actual_path);
  char shell_command[STRING_SIZE];
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  char **args = split_words(message, " ");
  char *path = args[1];
  FILE *arquivos;
  int w;
  char ch;

  // cria o sintax do comando dir
  // caso o cliente tenha setado uma pasta especifica
  // faz com que os erros sejam escritos na menssagem caso ocorra
  sprintf(shell_command, "dir %s 2>&1",path);
  printf("%s\n", shell_command);

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
    return "550 Path not found\n";
  }

  // a menssagem de retorno possui um \n no final entao para podermos comparalas
  strcat(path,"\n");
  // tambem devemos checar a possibilidade de o cliente estar usando informacoes sobre um arquivo especifico
  if (strcmp(return_message, path) == 0){
    // caso ele esteja vamos utilizar o comando stat para retornar as informacoes
    sprintf(shell_command, "stat ");
    strcat(shell_command,c->actual_path);
    sprintf(shell_command, "%s%s",shell_command,path);
    printf("%s\n",shell_command);

    arquivos = popen(shell_command,"r");
    //converte o arquivo para string
    for (int i = 0; ((ch = fgetc(arquivos)) != EOF); i++) {
      return_message[i] = ch;
    }
    printf("%s\n",return_message);
    // se as primeiras 5 letras do nosso retorno for stat: quer dizer que nao e um arquivo
    // deveremos retornar uma menssagem de erro
    if (strncmp(return_message, "stat:", 5) == 0) {
      return "550 File not found\n";
    }
  }
  pclose(arquivos);

  int client_s;
  if (c->modo_passivo == 0) {
    // Conecta com cliente
    client_s = createConnectionToConnect(c->data_session, c->server_address, c->data_session_port);
  } else {
    client_s = createConnectionToAccept(c->data_session);
  }

  if (client_s == -1) {
    printf("Err: %i\n", errno);
    return "425 Can't open data connection.\n";
  }

  // Informa início da transferência
  char *mes = "150 File status okay; about to open data connection.\n";
  printf("%s", mes);
  write(c->control_session, mes, strlen(mes));

  // Substitue \n por \r\n e envia para o cliente
  char *token = strtok(return_message, "\n");
  while (token != NULL) {
    char aux[STRING_SIZE];
    sprintf(aux, "%s\r\n", token);
    printf("%s",aux);
    if (c->modo_passivo == 0) {
      w = write(c->data_session, aux, strlen(aux));
    } else {
      w = write(client_s, aux, strlen(aux));
    }
    // Caso haja erro
    if (w == -1) {
      printf("Err: %i\n", errno);
      if (c->modo_passivo == 0) {
        close(c->data_session);
      } else {
        close(client_s);
      }
      return "425 Can't open data connection.\n";
    }
    token = strtok(NULL, "\n");
  }

  // Fecha conexão
  if (c->modo_passivo == 0) {
    close(c->data_session);
  } else {
    close(client_s);
  }

  return "226 Closing data connection.\n";
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
    return_message = "200 OK.\n";
    return return_message;
}

char *func_syst(ConnectionStatus *c, char *message) {
  char *return_message = (char*) malloc(STRING_SIZE*sizeof(char));
  return_message = "215 UNIX system type.\n";
  return return_message;
}

char *func_retr(ConnectionStatus *c, char *message) {
  // Recolhe nome do arquivo selecionado
  char **args = split_words(message, " ");
  printf("%s\n", args[1]);

  // Define se é um path ou está no diretório atual
  char filename[STRING_SIZE];
  if (args[1][0] == '/') {
    strcpy(filename, args[1]);
  } else {
    // Concatena destino com caminho atual
    strcpy(filename, c->actual_path);
    strcat(filename, args[1]);
  }

  // Checa se é um arquivo ou um diretório
  struct stat buffer;
  stat(filename, &buffer);
  if (!S_ISDIR(buffer.st_mode)) {
    // Informando abertura da conexão
    char *mes = "150 File status okay; about to open data connection.\n";
    write(c->control_session, mes, strlen(mes));

    // Conecta com cliente
    int client_s;
    if (c->modo_passivo == 0) {
      client_s = createConnectionToConnect(c->data_session, c->server_address, c->data_session_port);
    } else {
      client_s = createConnectionToAccept(c->data_session);
    }
    // Envia arquivo
    char buf[BUF_SIZE];
    strcpy(buf,"\0");
    FILE *file = fopen(filename,"r");
    int flag = 0;

    for (int i = 0; i < buffer.st_size; i++) {
      char caractere = fgetc(file);
      // Corrige CRLF com \r\n
      if (caractere == '\n') {
        sprintf(buf, "%s\r", buf);
        // Confere tamanho do buffer
        if (strlen(buf) < BUF_SIZE) {
          sprintf(buf, "%s\n", buf);
        } else {
          flag = 1;
        }
      } else {
        // Insere caractere comum
        sprintf(buf, "%s%c", buf,caractere);
      }

      // Incrementa contador do buffer
      if (strlen(buf) == BUF_SIZE) {
        // Envia dados quando buffer cheio
        printf("Buffer cheio, descarregando...\n");
        if (c->modo_passivo == 0) {
          write(c->data_session, buf, strlen(buf));
        } else {
          write(client_s, buf, strlen(buf));
        }
        if (flag == 1) {
          strcpy(buf,"\n");
        } else {
          strcpy(buf, "\0");
        }
      }
    }
    if (strlen(buf) > 0) {
      printf("Buffer final não cheio, descarregando...\n");
      if (c->modo_passivo == 0) {
        write(c->data_session, buf, strlen(buf));
      } else {
        write(client_s, buf, strlen(buf));
      }
    }
    // Fecha conexão
    if (c->modo_passivo == 0) {
      close(c->data_session);
    } else {
      close(client_s);
    }
    fclose(file);

    return "250 Requested file action okay, completed.\n";
  } else {
    return "450 Requested file action not taken.\n";
  }
}

char *func_stor(ConnectionStatus *c, char *message) {
  // Recolhe nome do arquivo selecionado
  char **args = split_words(message, " ");
  printf("%s\n", args[1]);

  // Define se é um path ou está no diretório atual
  char filename[STRING_SIZE];
  if (args[1][0] == '/') {
    strcpy(filename, args[1]);
  } else {
    // Concatena destino com caminho atual
    strcpy(filename, c->actual_path);
    strcat(filename, args[1]);
  }
  printf("%s\n",filename);

  // Checa se é um arquivo ou um diretório
  char *mes;
  struct stat buffer;
  stat(filename, &buffer);
  if (!S_ISREG(buffer.st_mode)) {
    // Conecta com cliente
    int client_s;
    if (c->modo_passivo == 0) {
      client_s = createConnectionToConnect(c->data_session, c->server_address, c->data_session_port);
    } else {
      client_s = createConnectionToAccept(c->data_session);
    }
    // Informa início da transferência
    mes = "125 Data connection already open; transfer starting.\n";
    printf("%s", mes);
    write(c->control_session, mes, strlen(mes));

    // Recebe mensagens do cliente
    FILE *f = fopen(filename, "w");
    while (1) {
      if (client_s == 0) {
        char *read_message = (char*) malloc(8*sizeof(char));
        int j;
        if (c->modo_passivo == 0) {
          j = read(c->data_session, read_message, sizeof(read_message));
        } else {
          j = read(client_s, read_message, sizeof(read_message));
        }
        if (j == 0) {
          break;
        }
        printf("%s\n", read_message);
        fprintf(f, "%s",read_message);
        free(read_message);
      } else {
        break;
      }
    }
    fclose(f);

    // Fecha conexão
    mes = "226 Closing data connection.\n";
    printf("%s",mes);
    write(c->data_session, mes, strlen(mes));
    if (c->modo_passivo == 0) {
      close(c->data_session);
    } else {
      close(client_s);
    }

    return "250 Requested file action okay, completed.\n";
  } else {
    return "450 Requested file action not taken.\n";
  }
}
