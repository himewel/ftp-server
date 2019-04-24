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
  char msg_write[TAM_BUFFER];
  char msg_read[TAM_BUFFER];
  int s;
  struct sockaddr_in dest;

  s = socket(AF_INET, SOCK_STREAM, 0);
  bzero(&dest, sizeof(dest));                /* zero the struct */
  dest.sin_family = AF_INET;
  dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /* set destination IP number - localhost, 127.0.0.1*/
  dest.sin_port = htons(PORTNUM);                /* set destination port number */

  connect(s, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));

  do {
    printf("%% ");
    scanf("%s", msg_write);
    write (s, msg_write, TAM_BUFFER);
    read (s, msg_read, TAM_BUFFER+1);
    printf(msg_read);
  } while (strcmp(msg_write,"bye") != 0);

  close(s);

  return 0;
}
