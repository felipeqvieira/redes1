#include "define.h"

int main(int argc, char *argv[]) {
  int socket;
  protocolo *msg = NULL, *msgRcd = NULL;
  interface ifc;
  char cmd[255], param[255];

  if((socket = openRSocket(ifc.name)) < 0)
  {
    // socket_errors(socket, ifc.name);
    exit(ABORTAR_EXEC);
  }

  return 0;
}

//Client.c -> Falta fazer a transferência de arquivos com janela deslizante
// Verificar CRC