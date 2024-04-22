#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcnt1.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioct1.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <inttypes.h>
#include <sys/select.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>

/* Atentar com sequência de VPN */

/* Protocolo */
typedef struct protocolo{
  
  uint8_t marcador; // 8 bits
  uint8_t tamanho; // 6 bits
  uint8_t seq; // 5 bits
  uint8_t tipo; // 5 bits
  uint8_t *dados; // 63 bytes
  uint8_t detec_erros; // 8 bits - crc-8 (tam, seq, tipo, dados)

}protocolo;

// TAMANHO
#define TAM_BUFFER 67 // 63bytes de dados + 4 bytes fixos (Marcador, Tamanho, Seq, Tipo))

// ERROS

#define ACESSO_NEGADO 1
#define ARQUIVO_NAO_ENCONTRADO 2
#define DISCO_CHEIO 3
#define PARAMETRO_INVALIDO 4
#define ERRO_CRC 5

// TIPOS

/* Códigos de mensagem */
#define ACK 0x00 // "00000000"
#define NACK 0x01 // "00000001"
#define ERRO 0x1F // "00011111"
#define DESCRITOR 0x11 // "00010001"
#define DADOS 0x12 // "00010010"
#define MOSTRA_NA_TELA 0X10 // "00010000"
#define MARCADOR 0x7E // "01111110"
#define FIM_TRANSMISSAO 0x1E // "00011110"

/* Comandos */
#define LISTAR 0x0A // "00001010"
#define BAIXAR 0x0B // "00001011"

/* Cliente */

//listar
//baixar

/* Servidor */

/* Raw Socket */

/* Funções Gerais */
void free_msg(protocolo *msg);