#include "define.h"
#include "socket.h"
#include "network.h"

int conv_comando(char *comando, char *parametro);

char *current_dir;

int main(int argc, char *argv[]) {
  int socket, i, sequencia;
  protocolo *msg = NULL, *msgRcd = NULL;
  interface ifc;
  char cmd[255], param[255];
  protocolo *msg_recebida = NULL;
  current_dir = (char *)malloc(255 * sizeof(char));
//   get_current_directory(current_dir, 255);

  strncpy(ifc.name, argv[1], sizeof(ifc.name));

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

  do {
    printf("Flix $: ");
    scanf("%[^\n]",cmd);
    fgetc(stdin);
    switch (conv_comando(cmd, param))
    {
      case LISTAR:
        printf("Comando de listar no client\n");
        // list_files_remote(socket);
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

}

int conv_comando(char *comando, char *parametro){
    char copia_comando[256];  // Supondo que comando não exceda 256 caracteres
    strcpy(copia_comando, comando); // Fazendo uma cópia para preservar o original

    char *quebra;
    strcpy(parametro, "");  // Inicializa parametro com uma string vazia
    quebra = strtok(copia_comando, " ");

    if (quebra != NULL && !strcmp(quebra, "listar")) {
        return LISTAR;
    } else if (quebra != NULL && !strcmp(quebra, "baixar")) {
        quebra = strtok(NULL, ";");
        if (quebra != NULL) {
            strcpy(parametro, quebra);
        }
        return BAIXAR;
    } else if (quebra != NULL && !strcmp(quebra, "sair")) {
        return SAIR;
    }

    return -1;  // Comando não reconhecido

}