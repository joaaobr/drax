#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#include "d_mod_http.h"
#include "../dstring.h"

drax_value start_http_server(d_vm* vm, char *options[], int* fail) {
  dhttp_server* configs = (dhttp_server*) malloc(sizeof(dhttp_server));
  configs->address = (struct sockaddr_in*) malloc(sizeof(struct sockaddr_in));

  configs->new_socket = -1;
  int opt = 1;

  char* port = options[0];
  char* endptr;
  int iport = strtod(port, &endptr);
  if (endptr == port) iport = 5000;

  if ((configs->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("Fail to create");
    *fail = 1;
  }

  if (setsockopt(configs->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("Fail to configure");
    close(configs->server_fd);
    *fail = 1;
  }

  /**
   * Define address and port
   */
  configs->address->sin_family = AF_INET;
  configs->address->sin_addr.s_addr = INADDR_ANY;
  configs->address->sin_port = htons(iport);

  if (inet_pton(AF_INET, "127.0.0.1", &configs->address->sin_addr) <= 0) {
    close(configs->server_fd);
    *fail = 1;
    return DS_VAL(new_derror(vm, (char *) "server error, Unsupported address!"));
  }

  if (bind(configs->server_fd, (struct sockaddr *) configs->address, sizeof(*configs->address)) < 0) {
    close(configs->server_fd);
    *fail = 1;
    return DS_VAL(new_derror(vm, (char *) "server error, fail to bind!"));
  }

  if (listen(configs->server_fd, 3) < 0) {
    close(configs->server_fd);
    *fail = 1;
    return DS_VAL(new_derror(vm, (char *) "server error, fail to listen!"));
  }

  *fail = 0;
  return (drax_value) configs;
}

int accept_http_server(d_vm* vm, drax_value aconf, drax_value* res) {
  if(!aconf) { return 0; }

  dhttp_server* configs = (dhttp_server*) aconf;
  int addrlen = sizeof(configs->address);

  if ((configs->new_socket = accept(configs->server_fd, (struct sockaddr *) configs->address,
   (socklen_t*) &addrlen)) < 0) {
    close(configs->server_fd);
    *res = DS_VAL(new_derror(vm, (char *) "server error, fail to accept connection!"));
    return 0;
  }

  return 1;
}

char* receive_http_server(d_vm* vm, drax_value aconf) {
  UNUSED(vm);
  if(!aconf) { return 0; }
  int bffsz = 16384;
  char* buffer = (char*) malloc(bffsz * sizeof(char));

  dhttp_server* configs = (dhttp_server*) aconf;
  read(configs->new_socket, buffer, bffsz);

  return buffer;
}

ssize_t send_http_server(d_vm* vm, drax_value aconf, char* s) {
  UNUSED(vm);
  if(!aconf) { return 0; }

  dhttp_server* configs = (dhttp_server*) aconf;
  char* data = str_format_output(s);
  ssize_t r = write(configs->new_socket, data, strlen(data));
  return -1 != r ;
}

int disconnect_client_http_server(d_vm* vm, drax_value v) {
  UNUSED(vm);
  if(!v) { return 0; }
  dhttp_server* configs = (dhttp_server*) v;
  close((int) configs->new_socket);
  return 1;
}

int stop_http_server(d_vm* vm, drax_value v) {
  UNUSED(vm);
  if(!v) { return 0; }
  dhttp_server* configs = (dhttp_server*) v;
  close((int) configs->server_fd);
  return 1;
}

