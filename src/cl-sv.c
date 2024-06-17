#include "define.h"

/*
  Entrada:
    cl-sv.c <tipo> <interface>
    tipo: 's' para servidor ou 'c
*/
char *current_dir;

int main(int argc, char *argv[]) {
  int socket, i, sequencia;
  protocolo *msg = NULL, *msgRcd = NULL;
  interface ifc;
  char cmd[255], param[255];
  protocolo *msg_recebida = NULL;
  current_dir = (char *)malloc(255 * sizeof(char));
  get_current_directory(current_dir, 255);

  strncpy(ifc.name, argv[2], sizeof(ifc.name)); 

  if ((socket = openRSocket(ifc.name)) < 0) {

    switch (socket) {
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
    // socket_errors(socket, ifc.name);
    exit(ABORTAR_EXEC);
  }

  switch (argv[1][1]) {
    case 's':
      while (1) {
        msg_recebida = recebe_msg(socket, 1);
        sequencia = prox_seq(msg_recebida);
        switch (ler_msg(msg_recebida)) {
        case LISTAR:
          list_files(current_dir);
          break;
        case BAIXAR:
          printf("Comando para baixar video\n");
          break;
        case ACK:
          printf("ACK recebido\n");
          break;
        case NACK:
          printf("NACK recebido\n");
          break;
        default:
          printf("Erro\n");
        }
        free_msg(msg_recebida);
      }
    break;
    case 'c':
      do {
        scanf("%[^\n]",cmd);
        fgetc(stdin);
        switch (conv_comando(cmd, param))
          {
          case LISTAR:
            printf("Comando de listar no client\n");
            // list_files(current_dir);
            list_files_remote(socket);
            break;
          case BAIXAR:
            printf("Comando de baixar no client\n");
            printf("Parametro: %s\n", param);
            break;
          case ACK:
            printf("ACK recebido\n");
            break;
          case NACK:
            printf("NACK recebido\n");
            break;
          case SAIR:
            printf("Encerrando programa\n");
            break;
          default:
            printf("ERRO - Comando %s não encontrado\n", cmd);
          }
  
      } while (strcmp(cmd, "sair") != 0);
    break;
    default:
      printf("ERRO - Tipo de cliente não encontrado\n");
      return -1;
  }
}

// Client.c -> Falta fazer a transferência de arquivos com janela deslizante
//  Verificar CRC