#include "server_func.h"

int client_s;
int MAX_CLIENTS = 20;
int clients_conec;

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

  // Cria socket e fica em loop até sua criação ser bem sucedida
  int s = createSocketToServe(address, port);
  while (s == -1) {
    printf("%s%c[1mErro: Aguardando %s%c[1m5s%s%c[1m para nova tentativa de criação do socket...%s\n",RED,27,YEL,27,RED,27,NRM);
    sleep(5);
    s = createSocketToServe(address, port);
  }
  printf("%s%c[1mInfo: %sSocket criado com %c[1msucesso%c[0m.\n",YEL,27,NRM,27,27);

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
      } else {
        strcpy(msg, "220 Service ready for new user.\n");
        write(client_s, msg, strlen(msg));

        printf("%s%c[1mInfo: %sConexão estabelecida com: %s%c[1m%s:%d%s.\n",YEL,27,NRM,BLU,27,inet_ntoa(client.sin_addr),(int) ntohs(client.sin_port),NRM);
        printf("%s%c[1mInfo: %sNúmero de conexões atualmente: %s%c[1m%i%s.\n",YEL,27,NRM,BLU,27,clients_conec+1,NRM);
        printf("%s%c[1mSend: %s%s",GRN,27,NRM,msg);
        printf("%s--------------------------------------------------------------------------------%s\n",RED,NRM);
        printf("%s--------------------------------------------------------------------------------%s\n",GRN,NRM);
        // criando a thread
        clients_conec++;
        pthread_create(&id[clients_conec],NULL,multUser,(void*)c);
      }
    } else {
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
  shutdown(c->control_session, SHUT_RDWR);
  close(c->control_session);
  free(c);
  // Atualiza número de clientes conectados
  clients_conec--;
  printf("%s--------------------------------------------------------------------------------%s\n",RED,NRM);
  printf("%s%c[1mInfo: %sConexão encerrada, atualizando número de conexões: %s%c[1m%i%s.\n",YEL,27,NRM,BLU,27,clients_conec,NRM);
  printf("%s--------------------------------------------------------------------------------%s\n",RED,NRM);
  printf("%s--------------------------------------------------------------------------------%s\n",GRN,NRM);
  //client_s = -1;
}
