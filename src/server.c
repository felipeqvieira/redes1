#include "define.h"
#include "socket.h"
#include "network.h"
char *current_dir;

int main(int argc, char *argv[]) {
  int socket, i, sequencia;
  protocolo *msg = NULL, *msgRcd = NULL;
  interface ifc;
  char cmd[255], param[255];
  protocolo *msg_recebida = NULL;
  current_dir = (char *)malloc(255 * sizeof(char));
  // get_current_directory(current_dir, 255);

  strncpy(ifc.name, argv[1], sizeof(ifc.name));

  if((socket = openRSocket(ifc.name)) < 0)
  {

    switch (socket)
    {   
      case -1:
        fprintf(stderr, "Erro - abrir socket (Não é ROOT)!\n");
        break;
      case -2:
        fprintf(stderr, "Erro - interface não encontrada\n");
        break;
      case -3:
        fprintf(stderr, "Erro - ligar socket a interface\n");
      case -4:
        fprintf(stderr, "Erro - ativar modo promiscuo na interface\n");
      }
      exit(ABORTAR_EXEC);
  }

  while (1)
  {
    printf("Aguardando requisição...\n");
    msg_recebida = recebe_msg(socket, 1);
    sequencia = prox_seq(msg_recebida);
    switch (ler_msg(msg_recebida)) {
    case LISTAR:
      // list_files(current_dir);
      break;
    case BAIXAR:
      log_message("Comando para baixar video\n");
      break;
    case ACK:
      log_message("ACK recebido!\n");
      break;
    case NACK:
      log_message("NACK recebido!\n");
      break;
    default:
      log_message("Pacote Invalido recebido\n");
    }
    free_packet(msg_recebida);
  }
}    
