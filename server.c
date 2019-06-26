#include "server_func.h"

int client_s;
int MAX_CLIENTS = 20;
int clients_conec;
int taxa_servidor = 2000000;
int taxa_atual = 0;

void signal_handler(int sign) {
  close(client_s);
  printf("\n%s%c[1mInfo: %sFechando conexão.\n",YEL,27,NRM);
  exit(1);
}

int main (int argc, char *argv[]) {
  printf("%s--------------------------------------------------------------------------------%s\n",GRN,NRM);
  // Confere a existência do argumento
  char *interface;
  // Pega endereço do servidor
  char address[32];
  if (argc > 1) {
    interface = argv[1];
    printf("%s%c[1mInfo: %sInterface selecionada: %c[1m%s%s%s.\n",YEL,27,NRM,27,BLU,interface,NRM);
    sprintf(address,"%s",getIPaddress(interface));
    if (strcmp(address, "") == 0) {
      printf("%s%c[1mInfo: %sUtilizando interface padrão: %s%c[1mlo%s.\n",YEL,27,NRM,BLU,27,NRM);
      interface = "lo";
      sprintf(address,"%s",getIPaddress(interface));
    } else {
      printf("%s%c[1mInfo: %sUtilizando interface selecionada.\n",YEL,27,NRM);
    }
  } else {
    printf("%s%c[1mInfo: %sInterface não informada, utilizando interface padrão: %s%c[1mlo%s.\n",YEL,27,NRM,BLU,27,NRM);
    interface = "lo";
    sprintf(address,"%s",getIPaddress(interface));
  }

  // Pega porta informada ou padrão
  int port;
  if (argc > 2) {
    port = (int)strtol(argv[2], NULL, 10);
    printf("%s%c[1mInfo: %sPorta selecionada: %s%c[1m%i%s.\n",YEL,27,NRM,BLU,27,port,NRM);
    if (port <= 1024) {
      printf("%s%c[1mErro: Sem permissão para utilização da porta selecionada.%s\n",RED,27,NRM);
      port = PORTNUM;
      printf("%s%c[1mInfo: %sPorta padrão selecionada: %s%c[1m%i%s.\n",YEL,27,NRM,BLU,27,port,NRM);
    }
  } else {
    port = PORTNUM;
    printf("%s%c[1mInfo: %sPorta padrão selecionada: %s%c[1m%i%s.\n",YEL,27,NRM,BLU,27,port,NRM);
  }

  // variáveis de thread
  clients_conec = 0;
  if (argc > 3) {
    MAX_CLIENTS = (int)strtol(argv[3], NULL, 10);
    printf("%s%c[1mInfo: %sNúmero máximo de clientes conectados simultaneamente: %s%c[1m%i%s.\n",YEL,27,NRM,BLU,27,MAX_CLIENTS,NRM);
  } else {
    printf("%s%c[1mInfo: %sNúmero máximo de clientes conectados simultaneamente não informado, utilizando valor padrão: %s%c[1m%i%s.\n",YEL,27,NRM,BLU,27,MAX_CLIENTS,NRM);
  }
  pthread_t id[MAX_CLIENTS];

  // Pega limite de taxa do servidor
  if (argc > 4) {
    taxa_servidor = (int)strtol(argv[4], NULL, 10);
    printf("%s%c[1mInfo: %sLimite total da taxa de transmissão do servidor: %s%c[1m%i%s.\n",YEL,27,NRM,BLU,27,taxa_servidor,NRM);
  } else {
    printf("%s%c[1mInfo: %sUtilizando limite total da taxa de transmissão padrão: %s%c[1m%i%s.\n",YEL,27,NRM,BLU,27,taxa_servidor,NRM);
  }

  // Cria socket e fica em loop até sua criação ser bem sucedida
  int s = createSocketToServe(address, port);
  while (s == -1) {
    printf("%s%c[1mErro: Aguardando %s%c[1m5s%s%c[1m para nova tentativa de criação do socket...%s\n",RED,27,YEL,27,RED,27,NRM);
    sleep(5);
    s = createSocketToServe(address, port);
  }
  printf("%s%c[1mInfo: %sSocket criado com %c[1msucesso%c[0m.\n",YEL,27,NRM,27,27);

  FILE *file;
  // Variáveis com dados
  char endereco[MAX_CLIENTS][15];
  int taxa[MAX_CLIENTS];
  // Leitura do arquivo
  if ((file = fopen("config.ini", "r")) == NULL) {
    printf("%s%c[1mInfo: %sArquivo de configuração não encontrado, utilizando taxa padrão para todos os usuários.\n",YEL,27,NRM);
    printf("%s%c[1mInfo: %sTaxa de transmissão padrão: %i\n",YEL,27,NRM,(int)taxa_servidor/MAX_CLIENTS);
  } else {
    // Variáveis na leitura
    char *line;
    size_t len = 0;
    ssize_t read;
    int i = 0;
    while ((read = getline(&line, &len, file)) != -1) {
      i = i + 1;
      char grandeza;
      sscanf(line, "%s %i %c", endereco[i-1], &taxa[i-1], &grandeza);
      switch (grandeza) {
        case 'G':
        case 'g':
          taxa[i-1] *= 1000000000;
          break;
        case 'M':
        case 'm':
          taxa[i-1] *= 1000000;
          break;
        case 'K':
        case 'k':
          taxa[i-1] *= 1000;
          break;
        case 'B':
        case 'b':
          break;
        default:
          printf("Unidade de grandeza não reconhecida na linha %i, lendo como K.\n", i);
          taxa[i-1] *= 1000;
          break;
      }
      if (i == MAX_CLIENTS) {
        break;
      }
    }
    fclose(file);
  }

  // Cria variáveis usadas nas conexões
  char msg[STRING_SIZE];
  struct sockaddr_in client;
  int addr_len = sizeof(client);

  printf("%s%c[1mInfo: %sRodando servidor em: %s%c[1m%s:%i%s.\n",YEL,27,NRM,BLU,27,address,port,NRM);
  printf("%s--------------------------------------------------------------------------------%s\n",GRN,NRM);
  while (1) {
    // Recebe conexão e garante que valor seja diferente de 0
    client_s = accept(s, (struct sockaddr*)&client, &addr_len);
    signal(SIGINT, signal_handler);
    printf("%s--------------------------------------------------------------------------------%s\n",RED,NRM);

    if (clients_conec < MAX_CLIENTS){
      // Struct que mantém estado da conexão
      ConnectionStatus *c = initializeStatus();
      c->control_session = client_s;
      c->server_address = address;
      char *client_address = (char*) malloc(15*sizeof(char));
      client_address = inet_ntoa(client.sin_addr);
      c->client_address = client_address;
      c->taxa_transmissao = (int)taxa_servidor/MAX_CLIENTS;
      for (int i = 0; i < MAX_CLIENTS; i++) {
        if (strcmp(endereco[i],c->client_address) == 0) {
          c->taxa_transmissao = taxa[i];
          break;
        }
      }
      // Checa falha na conexão
      if (client_s == 0) {
        printf("%s%c[1mInfo: %sConexão recusada com: %s%c[1m%s:%d%s.\n",YEL,27,NRM,BLU,27,inet_ntoa(client.sin_addr),(int) ntohs(client.sin_port),NRM);
        printf("%s%c[1mErro: %i%s\n",RED,27,errno,NRM);
        strcpy(msg, "421 Service not available, closing control connection.\n");
        printf("%s%c[1mSend: %s%s",GRN,27,NRM,msg);
        printf("%s--------------------------------------------------------------------------------%s\n",RED,NRM);
        printf("%s--------------------------------------------------------------------------------%s\n",GRN,NRM);
        write(client_s, msg, strlen(msg));
        free(c);
        continue;
      } else if (c->taxa_transmissao + taxa_atual > taxa_servidor) {
        printf("%s%c[1mInfo: %sConexão recusada com: %s%c[1m%s:%d%s.\n",YEL,27,NRM,BLU,27,inet_ntoa(client.sin_addr),(int) ntohs(client.sin_port),NRM);
        printf("%s%c[1mErro: Taxa atribuída ao cliente é maior que taxa limite do servidor.%s\n",RED,27,NRM);
        strcpy(msg, "421 Service not available, closing control connection.\n");
        printf("%s%c[1mSend: %s%s",GRN,27,NRM,msg);
        printf("%s--------------------------------------------------------------------------------%s\n",RED,NRM);
        printf("%s--------------------------------------------------------------------------------%s\n",GRN,NRM);
        write(client_s, msg, strlen(msg));
        free(c);
        continue;
      } else {
        strcpy(msg, "220 Service ready for new user.\n");
        write(client_s, msg, strlen(msg));
        taxa_atual += c->taxa_transmissao;

        printf("%s%c[1mInfo: %sConexão estabelecida com: %s%c[1m%s:%d%s.\n",YEL,27,NRM,BLU,27,inet_ntoa(client.sin_addr),(int) ntohs(client.sin_port),NRM);
        printf("%s%c[1mInfo: %sNúmero de conexões atualmente: %s%c[1m%i%s.\n",YEL,27,NRM,BLU,27,clients_conec+1,NRM);
        printf("%s%c[1mInfo: %sTaxa de transmissão reservada/Taxa máxima definida: %s%c[1m%i/%i%s.\n",YEL,27,NRM,BLU,27,taxa_atual,taxa_servidor,NRM);
        printf("%s%c[1mSend: %s%s",GRN,27,NRM,msg);
        printf("%s--------------------------------------------------------------------------------%s\n",RED,NRM);
        printf("%s--------------------------------------------------------------------------------%s\n",GRN,NRM);
        // criando a thread
        pthread_create(&id[clients_conec],NULL,multUser,(void*)c);
        clients_conec++;
      }
    } else {
      // Caso o número máximo de conexões tenha sido atingido
      printf("%s%c[1mInfo: %sConexão recusada com: %s%c[1m%s:%d%s.\n",YEL,27,NRM,BLU,27,inet_ntoa(client.sin_addr),(int) ntohs(client.sin_port),NRM);
      printf("%s%c[1mInfo: %sNúmero máximo de conexões simultâneas atigido: %s%c[1m%i%s.\n",YEL,27,NRM,BLU,27,MAX_CLIENTS,NRM);
      strcpy(msg, "421 Service not available, closing control connection.\n");
      printf("%s%c[1mSend: %s%s",GRN,27,NRM,msg);
      write(client_s, msg, strlen(msg));
      printf("%s--------------------------------------------------------------------------------%s\n",RED,NRM);
      printf("%s--------------------------------------------------------------------------------%s\n",GRN,NRM);
    }
  }

  return 0;
}

void *multUser(void *_c){

  ConnectionStatus *c = (ConnectionStatus*) _c;
  char msg[STRING_SIZE];

  // Caso erro na conexão ou mensagem solicitando encerramento
  while (c->connection_ok == 1) {
    bzero(msg, STRING_SIZE);
    // Decodifica mensagem e trata
    read(c->control_session, msg, sizeof(msg));
    printf("%s%c[1mRecv: %s%s",BLU,27,NRM,msg);
    int message = decode_message(msg);
    // Trata o comando recebido
    switch (message) {
      case 0:
        strcpy(msg,func_user(c, msg));
        break;
      case 1:
        strcpy(msg,func_pass(c,msg));
        break;
      case 20:
        strcpy(msg,func_cwd(c,msg));
        break;
      case 2:
        strcpy(msg,func_cdup(c,msg));
        break;
      case 3:
        strcpy(msg,func_quit(c,msg));
        break;
      case 4:
        strcpy(msg,func_list(c,msg));
        break;
      case 21:
        strcpy(msg,func_pwd(c,msg));
        break;
      case 22:
        strcpy(msg,func_mkd(c,msg));
        break;
      case 23:
        strcpy(msg,func_rmd(c,msg));
        break;
      case 11:
        strcpy(msg,func_noop(c,msg));
        break;
      case 5:
        strcpy(msg,func_syst(c,msg));
        break;
      case 6:
        strcpy(msg,func_port(c,msg));
        break;
      case 7:
        strcpy(msg,func_type(c,msg));
        break;
      case 8:
        strcpy(msg,func_pasv(c,msg));
        break;
      case 9:
        strcpy(msg,func_retr(c,msg));
        break;
      case 10:
        strcpy(msg,func_stor(c,msg));
        break;
      default:
        strcpy(msg,"202 Command not implemented, superfluous at this site.\n");
        break;
    }
    write(c->control_session, msg, strlen(msg));
    printf("%s%c[1mSend: %s%s",GRN,27,NRM,msg);
    printf("%s--------------------------------------------------------------------------------%s\n",GRN,NRM);
  }
  // Atualiza taxa de transmissão do servidor
  taxa_atual -= c->taxa_transmissao;

  shutdown(c->control_session, SHUT_RDWR);
  close(c->control_session);
  free(c);
  // Atualiza número de clientes conectados
  clients_conec--;
  printf("%s--------------------------------------------------------------------------------%s\n",RED,NRM);
  printf("%s%c[1mInfo: %sConexão encerrada, atualizando número de conexões: %s%c[1m%i%s.\n",YEL,27,NRM,BLU,27,clients_conec,NRM);
  printf("%s%c[1mInfo: %sTaxa de transmissão reservada/Taxa máxima definida: %s%c[1m%i/%i%s.\n",YEL,27,NRM,BLU,27,taxa_atual,taxa_servidor,NRM);
  printf("%s--------------------------------------------------------------------------------%s\n",RED,NRM);
  printf("%s--------------------------------------------------------------------------------%s\n",GRN,NRM);
  //client_s = -1;
}
