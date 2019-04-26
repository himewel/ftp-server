#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <arpa/inet.h>
//#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define TAM_BUFFER 500
#define PORTNUM 2300

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

  // Loop principal
  do {
    printf("%% ");
    scanf("%s", msg_write);
    write (s, msg_write, TAM_BUFFER);
    read (s, msg_read, TAM_BUFFER+1);
    printf("%s\n",msg_read);
  } while (strcmp(msg_write,"bye") != 0);

  // Fecha conexão
  close(s);

  return 0;
}
