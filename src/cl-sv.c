#include "define.h"

/*
  Entrada:
    cl-sv.c <tipo> <interface>
    tipo: 's' para servidor ou 'c
*/

int main(int argc, char *argv[]) {
  int socket, i, sequencia;
  protocolo *msg = NULL, *msgRcd = NULL;
  interface ifc;
  char cmd[255], param[255];
  protocolo *msg_recebida = NULL;

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
          printf("Comando de listar videos\n");
          break;
        case BAIXAR:
          printf("Comando para baixar video\n");
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
            break;
          case BAIXAR:
            printf("Comando de baixar no client\n");
            printf("Parametro: %s\n", param);
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