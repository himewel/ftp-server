#include "common.h"

int main (void) {
  // Buffers de leitura e escrita
  char msg_write[TAM_BUFFER];
  char msg_read[TAM_BUFFER];

  // Reserva socket e configura conexão
  int s;
  s = socket(AF_INET, SOCK_STREAM, 0);

  // Configuração do endereço de destino: SERVIDOR
  struct sockaddr_in dest;
  bzero(&dest, sizeof(dest));
  // Endereço e porta destino
  dest.sin_family = AF_INET;
  dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  dest.sin_port = htons(PORTNUM);

  // Solicita conexão com o servidor configurado
  connect(s, (struct sockaddr *)&dest, sizeof(dest));

  read (s, msg_read, TAM_BUFFER+1);
  printf("%s\n",msg_read);

  // Loop principal
  while (strcmp(msg_read, "221 Service closing control connection.") != 0) {
    printf("%% ");
    gets(msg_write);
    write (s, msg_write, TAM_BUFFER);
    read (s, msg_read, TAM_BUFFER+1);
    printf("%s\n",msg_read);
  }

  // Fecha conexão
  close(s);

  return 0;
}
