#include "server_func.h"

int main (void) {
  int s, client_s, addr_len;
  int flag_connection;
  char *return_message;
  char msg_read[STRING_SIZE];

  // Configuração do socket para receber solicitações de qualquer endereço
  struct sockaddr_in self, client;
  addr_len = sizeof(client);
  bzero(&self, sizeof(self));
  self.sin_family = AF_INET;
  self.sin_port = htons(PORTNUM);
  self.sin_addr.s_addr = INADDR_ANY;

  // Reserva socket
  s = socket(AF_INET, SOCK_STREAM, 0);
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
    write (client_s, "220 Service ready for new user.\n", STRING_SIZE);

    // Struct que mantém estado da conexão
    ConnectionStatus *c = initializeStatus();

    // Caso erro na conexão ou mensagem solicitando encerramento
    while (client_s != -1 && c->connection_ok == 1) {
      // Decodifica mensagem e trata
      int buffer_len = read(client_s, msg_read, STRING_SIZE);
      msg_read[buffer_len-1] = 0;
      printf("%s\n", msg_read);
      int message = decode_message(msg_read);
      // Trata o comando recebido
      switch (message) {
        case 0:
          return_message = func_user(c, msg_read);
          break;
        case 1:
          return_message = func_pass(c,msg_read);
          break;
        case 2:
          return_message = func_acct(c,msg_read);
          break;
        case 3:
          return_message = func_cwd(c,msg_read);
          break;
        case 4:
          return_message = func_cdup(c,msg_read);
          break;
        case 5:
          return_message = func_smnt(c,msg_read);
          break;
        case 6:
          return_message = func_rein(c,msg_read);
          break;
        case 7:
          return_message = func_quit(c,msg_read);
          break;
        case 8:
          return_message = func_list(c,msg_read);
          break;
        case 9:
          return_message = func_pwd(c,msg_read);
          break;
        case 10:
          return_message = func_mkd(c,msg_read);
          break;
        case 11:
          return_message = func_rmd(c,msg_read);
          break;
        case 12:
          return_message = func_noop(c,msg_read);
          break;
        default:
          return_message = "500 Syntax error, command unrecognized.\n";
          break;
      }
      write(client_s, return_message, STRING_SIZE);
    }
    free(c);
    client_s = -1;
  }

  return 0;
}
