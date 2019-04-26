#include "server_func.h"

int main (void) {
  int s, client_s, addr_len;
  char msg_write[TAM_BUFFER], msg_read[TAM_BUFFER];

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

    // Caso erro na conexão ou mensagem solicitando encerramento
    while (client_s != -1 || strcmp(msg_read,"bye") != 0) {
      // Decodifica mensagem e trata
      read(client_s, msg_read, TAM_BUFFER+1);
      int message = decode_message(msg_read);

      // Trata o comando recebido
      switch (message) {
        case -1:
          write (client_s, "bye", TAM_BUFFER);
          close(client_s);
          break;
        case 0:
          write (client_s, "  200 Logado", TAM_BUFFER);
          //func_user(msg_read);
          break;
        case 1:
          write (client_s, "  200 Senha", TAM_BUFFER);
          //func_pass(msg_read);
          break;
        default:
          write (client_s, msg_read, TAM_BUFFER);
          break;
      }
    }

    close(client_s);

  }

  return 0;
}
