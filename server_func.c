#include "server_func.h"

int decode_message (char *message) {
  int code;
  if (strcmp(message,"user") == 0) {
    code = 0;
  } else if (strcmp(message,"pass") == 0) {
    code = 1;
  } else if (strcmp(message,"quit") == 0) {
    code = -1;
  } else {
    code = -1000;
  }
  return code;
}

void func_user(msg_read) {
  
}
