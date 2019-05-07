#include "server_func.h"

int main (void) {
  int s, client_s, addr_len;
  int flag_connection;
  char msg[STRING_SIZE];

  // Configuração do socket para receber solicitações de qualquer endereço
  struct sockaddr_in self, client;
  addr_len = sizeof(client);
  bzero(&self, sizeof(self));
  self.sin_family = AF_INET;
  self.sin_port = htons(PORTNUM);
  self.sin_addr.s_addr = INADDR_ANY;

  // Reserva socket
  s = socket(AF_INET, SOCK_STREAM, 0);
  int reuse = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
  if (bind(s, (struct sockaddr*)&self, sizeof(self)) == -1) {
    printf("  Falha na criação do socket\n");
    return 0;
  }

  // Ativa escuta de conexões, máximo de 20
  if (listen(s, 5) == -1) {
    printf("  Falha na escuta das conexões\n");
    return 0;
  }

  while (1) {
    // Recebe conexão
    client_s = accept(s, (struct sockaddr*)&client, &addr_len);
    strcpy(msg, "220 Service ready for new user.\n");
    write (client_s, msg, strlen(msg)+1);

    // Struct que mantém estado da conexão
    ConnectionStatus *c = initializeStatus();
    c->control_session = client_s;

    // Caso erro na conexão ou mensagem solicitando encerramento
    while (client_s != -1 && c->connection_ok == 1) {
      printf("%s",msg);
      printf("--------------------------------------------------------------------------------\n");
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
        case 3:
          strcpy(msg,func_cwd(c,msg));
          break;
        case 4:
          strcpy(msg,func_cdup(c,msg));
          break;
        case 7:
          strcpy(msg,func_quit(c,msg));
          break;
        case 8:
          strcpy(msg,func_list(c,msg));
          break;
        case 9:
          strcpy(msg,func_pwd(c,msg));
          break;
        case 10:
          strcpy(msg,func_mkd(c,msg));
          break;
        case 11:
          strcpy(msg,func_rmd(c,msg));
          break;
        case 12:
          strcpy(msg,func_noop(c,msg));
          break;
        case 13:
          strcpy(msg,func_syst(c,msg));
          break;
        case 14:
          strcpy(msg,func_port(c,msg));
          break;
        case 15:
          strcpy(msg,func_type(c,msg));
          break;
        case 16:
          strcpy(msg,func_pasv(c,msg));
          break;
        case 17:
          strcpy(msg,func_retr(c,msg));
          break;
        case 18:
          strcpy(msg,func_stor(c,msg));
          break;
        default:
          strcpy(msg,"202 Command not implemented, superfluous at this site.\n");
          break;
      }
      write(client_s, msg, strlen(msg) + 1);
    }
    free(c);
  }

  return 0;
}
