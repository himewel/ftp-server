#include "server_func.h"

int main (int argc, char *argv[]) {
  printf("%s--------------------------------------------------------------------------------%s\n",GRN,NRM);
  // Confere a existência do argumento
  char *interface;
  if (argc > 1) {
    interface = argv[1];
    printf("%s%c[1mInfo: %sInterface selecionada: %c[1m%s%s%s.\n",YEL,27,NRM,27,BLU,interface,NRM);
  } else {
    printf("%s%c[1mInfo: %sInterface não informada, utilizando interface padrão: %s%c[1mlo%s.\n",YEL,27,NRM,BLU,27,NRM);
    interface = "lo";
  }

  // Pega endereço do servidor
  char *address = getIPaddress(interface);
  if (strcmp(address, "") == 0) {
    printf("%s%c[1mInfo: %sUtilizando interface padrão: %s%c[1mlo%s.\n",YEL,27,NRM,BLU,27,NRM);
    interface = "lo";
    address = getIPaddress(interface);
  } else {
    printf("%s%c[1mInfo: %sUtilizando interface selecionada.\n",YEL,27,NRM);
  }

  // Pega porta informada ou padrão
  int port;
  if (argc > 2) {
    port = (int)strtol(argv[2], NULL, 10);;
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

  // Cria socket e fica em loop até sua criação ser bem sucedida
  int s = createSocketToServe(address, port);
  while (s == -1) {
    printf("%s%c[1mErro: Aguardando %s%c[1m5s%s%c[1m para nova tentativa de criação do socket...%s\n",RED,27,YEL,27,RED,27,NRM);
    sleep(5);
    s = createSocketToServe(address, port);
  }
  printf("%s%c[1mInfo: %sSocket criado com %c[1msucesso%c[0m.\n",YEL,27,NRM,27,27);

  int client_s;
  char msg[STRING_SIZE];
  struct sockaddr_in client;
  int addr_len = sizeof(client);

  printf("%s%c[1mInfo: %sRodando servidor em: %s%c[1m%s:%i%s.\n",YEL,27,NRM,BLU,27,address,port,NRM);
  while (1) {
    printf("%s--------------------------------------------------------------------------------%s\n",GRN,NRM);
    // Recebe conexão e garante que valor seja diferente de 0
    client_s = accept(s, (struct sockaddr*)&client, &addr_len);

    // Struct que mantém estado da conexão
    ConnectionStatus *c = initializeStatus();
    c->control_session = client_s;
    c->server_address = address;

    if (client_s == 0) {
      printf("%s%c[1mErro: %i%s\n",RED,27,errno,NRM);
      strcpy(msg, "421 Service not available, closing control connection.\n");
      printf("%s",msg);
      printf("%s--------------------------------------------------------------------------------%s\n",GRN,NRM);
      write(client_s, msg, strlen(msg));
      c->connection_ok = -1;
      continue;
    } else {
      strcpy(msg, "220 Service ready for new user.\n");
      write(client_s, msg, strlen(msg));
    }

    printf("%s%c[1mInfo: %sConexão estabelecida com: %s%c[1m%s:%d%s.\n",YEL,27,NRM,BLU,27,inet_ntoa(client.sin_addr),(int) ntohs(client.sin_port),NRM);

    // Caso erro na conexão ou mensagem solicitando encerramento
    while (c->connection_ok == 1) {
      printf("%s",msg);
      printf("%s--------------------------------------------------------------------------------%s\n",GRN,NRM);
      bzero(msg, STRING_SIZE);
      // Decodifica mensagem e trata
      read(client_s, msg, sizeof(msg));
      printf("%s", msg);
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
      write(client_s, msg, strlen(msg));
    }
    free(c);
    shutdown(client_s, SHUT_RDWR);
    close(client_s);
    client_s = -1;
  }

  return 0;
}
