#include <dirent.h>
#include <errno.h>
#include <fcnt1.h>
#include <inttypes.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioct1.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/* Atentar com sequência de VPN */

/* Interface */
typedef struct interface {
  char name[20]; // Nome da interface
  char type;     // Servidor ou Cliente
} interface;

/* Protocolo */
typedef struct protocolo {

  uint8_t marcador;    // 8 bits
  uint8_t tamanho;     // 6 bits
  uint8_t seq;         // 5 bits
  uint8_t tipo;        // 5 bits
  uint8_t *dados;      // 63 bytes
  uint8_t detec_erros; // 8 bits - crc-8 (tam, seq, tipo, dados)

} protocolo;

// TAMANHO
#define TAM_BUFFER                                                             \
  67 // 63bytes de dados + 4 bytes fixos (Marcador, Tamanho, Seq, Tipo))

// ERROS
#define ABORTAR_EXEC -1
#define ACESSO_NEGADO 1
#define ARQUIVO_NAO_ENCONTRADO 2
#define DISCO_CHEIO 3
#define PARAMETRO_INVALIDO 4
#define ERRO_CRC 5

// TIPOS

/* Códigos de mensagem */
#define ACK 0x00             // "00000000"
#define NACK 0x01            // "00000001"
#define ERRO 0x1F            // "00011111"
#define DESCRITOR 0x11       // "00010001"
#define DADOS 0x12           // "00010010"
#define MOSTRA_NA_TELA 0X10  // "00010000"
#define MARCADOR 0x7E        // "01111110"
#define FIM_TRANSMISSAO 0x1E // "00011110"

/* Comandos */
#define LISTAR 0x0A // "00001010"
#define BAIXAR 0x0B // "00001011"

/* Cliente */
void download(char *videoName); // Baixar arquivos do Servidor para o Cliente

/* Servidor */
void list(char *dir); // Lista arquivos

/* Raw Socket */
int openRSocket(char *interface); // Abre o socket do tipo Raw

/* Funções Gerais */
void free_msg(protocolo *msg); // Libera a memória alocada para a mensagem
uint8_t prox_seq(protocolo *msg); // Retorna o próximo número de sequência
protocolo *aloca_msg(); // Aloca a memória para a mensagem
protocolo *cria_msg(uint8_t seq, uint8_t tipo, uint8_t *dados, ...); // Cria a mensagem
void envia_msg(int socket, protocolo *msg); // Envia a mensagem para o socket
protocolo *recebe_msg(int socket, int n_msgs); // Recebe a mensagem
uint8_t ler_msg(protocolo *msg); // Lê o tipo da mensagem
void imprime_msg(protocolo *msg); // Imprime a mensagem
void socket_errors(short errno, interface ifc); // Mostas os possíveis erros de socket
