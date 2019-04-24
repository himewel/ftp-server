#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define TAM_BUFFER 500
#define PORTNUM 2300

int main (void) {
  int s, client_s;
  struct sockaddr_in self, client;

  int addrlen = sizeof(client);
  char msg_write[TAM_BUFFER], msg_read[TAM_BUFFER];

  printf("Abrindo socket\n");
  s = socket(AF_INET, SOCK_STREAM, 0);

  bzero(&self, sizeof(self));
  self.sin_family = AF_INET;
  self.sin_port = htons(PORTNUM);
  self.sin_addr.s_addr = INADDR_ANY;

  printf("Dando bind\n");
  bind(s, (struct sockaddr*)&self, sizeof(self));

  printf("Listen!\n");
  listen(s, 5);

  while (1) {
    client_s = accept(s, (struct sockaddr*)&client, &addrlen);

    do {
      read(client_s, msg_read, TAM_BUFFER+1);
      if (strcmp(msg_read,"USER") == 0) {
        write (client_s, "  200 Logado\n", TAM_BUFFER);
      } else if (strcmp(msg_read,"PASS") == 0) {
        write (client_s, "  200 Senha\n", TAM_BUFFER);
      } else {
        write (client_s, "bye\n", TAM_BUFFER);
        return 0;
      }
    } while (strcmp(msg_read,"bye") != 0);
    close(client_s);
  }

  return 0;
}
