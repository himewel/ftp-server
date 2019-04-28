#include "server_func.h"

int main (void) {
  int s, client_s, addr_len;
  int flag_connection;
  char *msg_write;
  char msg_read[TAM_BUFFER];

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
    write (client_s, "220 Service ready for new user.", TAM_BUFFER);

    flag_connection = 1;

    // Struct que mantém estado da conexão
    ConnectionStatus *c = initializeStatus(c);

    // Caso erro na conexão ou mensagem solicitando encerramento
    while (client_s != -1 || flag_connection == 1) {
      // Decodifica mensagem e trata
      read(client_s, msg_read, TAM_BUFFER+1);
      int message = decode_message(msg_read);
      // Trata o comando recebido
      switch (message) {
        case -1:
          write (client_s, "bye", TAM_BUFFER);
          flag_connection = 0;
          break;
        case 0:
          c = func_user(c,msg_read);
          write(client_s, c->return_message, TAM_BUFFER);
          break;
        case 1:
          c = func_pass(c,msg_read);
          write(client_s, c->return_message, TAM_BUFFER);
          break;
        case 2:
          c = func_acct(c,msg_read);
          write(client_s, c->return_message, TAM_BUFFER);
          break;
        case 3:
          c = func_cwd(c,msg_read);
          write(client_s, c->return_message, TAM_BUFFER);
          break;
        case 4:
          c = func_cdup(c,msg_read);
          write(client_s, c->return_message, TAM_BUFFER);
          break;
        case 5:
          c = func_smnt(c,msg_read);
          write(client_s, c->return_message, TAM_BUFFER);
          break;
        case 6:
          c = func_rein(c,msg_read);
          write(client_s, c->return_message, TAM_BUFFER);
        default:
          write (client_s, msg_read, TAM_BUFFER);
          break;
      }
    }
    free(c);
    client_s = -1;
  }

  return 0;
}
