#include "server_func.h"

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
    printf("Interface selecionada não está disponível\n");
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
  self.sin_port = htons(PORTNUM);
  self.sin_addr.s_addr = inet_addr(address);

  int s = socket(AF_INET, SOCK_STREAM, 0);
  int b = bind(s, (struct sockaddr*)&self, sizeof(self));
  int l = listen(s, 4);

  return (b < 0 || l < 0 || s <0) ? -1 : s;
}

int main (int argc, char *argv[]) {
  // Confere a existência do argumento
  if(argc <= 1) {
    printf("Falha: Informe a interface de rede para o servidor.\n");
    return 0;
  }

  // Pega endereço do servidor
  char *address = getIPaddress(argv[1]);
  if (strcmp(address, "") == 0) {
    return 0;
  }

  // Cria socket e fica em loop até sua criação ser bem sucedida
  int s = createSocketToServe(address, PORTNUM);
  while (s == -1) {
    printf("Falha: Aguardando 5s para nova tentativa de criação do scket...\n");
    sleep(5);
    s = createSocketToServe(address, PORTNUM);
  }

  int client_s;
  int flag_connection;
  char msg[STRING_SIZE];
  struct sockaddr_in client;
  int addr_len = sizeof(client);

  printf("Rodando em %s:%i\n", address, PORTNUM);

  while (1) {
    // Recebe conexão
    client_s = accept(s, (struct sockaddr*)&client, &addr_len);
    while (client_s < 1) {
      if (client_s == 0) {
        printf("Erro: %i\n", errno);
        strcpy(msg, "421 Service not available, closing control connection.\n");
        printf("%s",msg);
        printf("--------------------------------------------------------------------------------\n");
        write(client_s, msg, strlen(msg));
      }
      client_s = accept(s, (struct sockaddr*)&client, &addr_len);
    }

    printf("client_s %i\n", client_s);
    strcpy(msg, "220 Service ready for new user.\n");
    write(client_s, msg, strlen(msg));

    // Struct que mantém estado da conexão
    ConnectionStatus *c = initializeStatus();
    c->control_session = client_s;
    c->server_address = address;

    // Caso erro na conexão ou mensagem solicitando encerramento
    while (c->connection_ok == 1) {
      printf("%s",msg);
      printf("--------------------------------------------------------------------------------\n");
      bzero(msg, STRING_SIZE);
      // Decodifica mensagem e trata
      int r = read(client_s, msg, sizeof(msg));
      if (r == -1) {
        printf("Conexão perdida com cliente: %i\n", errno);
        strcpy(msg, "421 Service not available, closing control connection.\n");
        write(client_s, msg, strlen(msg) + 1);
        break;
      }
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
