#include "define.h"

void free_msg(protocolo *msg) {

  if (msg) {
    free(msg->dados);
    free(msg);
  }
}

uint8_t prox_seq(protocolo *msg) {

  uint8_t proxima_seq;

  /* Verifica se mensagem existe */
  if (!msg)
    return (0);

  /* Verifica se é o ultimo número */
  if (msg->seq < 31)
    proxima_seq = msg->seq + 1;
  else
    proxima_seq = 0;

  return (proxima_seq);
}

// cal_crc8

// aloca_msg
protocolo *aloca_msg() {

  protocolo *msg;

  if (!(msg = (protocolo *)malloc(sizeof(protocolo))))
    exit(-1);

  return (msg);
}

// aloca_vetor
uint8_t *aloca_vetor(int tam) {

  uint8_t *dados;

  dados = (uint8_t *)malloc((sizeof(uint8_t) * tam + 1));

  if (!dados) {
    fprintf(stderr, "Erro na alocação de memória.\n");
    exit(-1);
  }

  return (dados);
}

protocolo *cria_msg(uint8_t seq, uint8_t tipo, uint8_t *dados, ...) {

  protocolo *msg;

  va_list argumentos;
  va_start(argumentos, dados);

  msg = aloca_msg();
  msg->marcador = MARCADOR;

  switch (tipo) {
  case DADOS:
    msg->tamanho = va_arg(argumentos, int);
    break;

  case DESCRITOR:
    msg->tamanho = va_arg(argumentos, int);
    break;

  case NACK:
    msg->tamanho = sizeof(uint8_t);
    break;

  case ERRO:
    msg->tamanho = sizeof(uint8_t);
    break;

  default:
    if (dados)
      msg->tamanho = strlen(dados);
    else {
      msg->tamanho = strlen("SEMDADOS");
      dados = (uint8_t *)"SEMDADOS";
    }
  }

  msg->seq = seq;
  msg->tipo = tipo;
  msg->dados = aloca_vetor(msg->tamanho);

  for (int i = 0; i < msg->tamanho; i++)
    msg->dados[i] = dados[i];

  /* Desloca bits para crc-8 */
  msg->tamanho = (msg->tamanho << 2);
  msg->seq = (msg->seq << 3);
  msg->tipo = (msg->tipo << 3);

  msg->detec_erros = cal_crc8(msg);

  return (msg);
}

void envia_msg(int socket, protocolo *msg) {

  uint8_t *buffer;

  if (msg) {
    buffer = aloca_vetor(TAM_BUFFER);

    // inicio
    buffer[0] = msg->marcador;

    // tamanho
    buffer[1] = (msg->tamanho);

    buffer[1] |= (msg->seq >> 6);

    buffer[2] = (msg->seq << 2);

    buffer[2] |= (msg->tipo >> 3);

    for (int i = 1; i < msg->tamanho; i++)
      buffer[3 + i] = msg->dados[i];

    buffer[3 + msg->tamanho] = msg->detec_erros;

    write(socket, buffer, TAM_BUFFER);
    free(buffer);
  } else {
    fprintf(stderr, "ERRO: Mensagem Vazia\n");
  }
}

protocolo *recebe_msg(int socket, int n_msgs) {

  protocolo *msg;
  uint8_t *buffer;

  buffer = aloca_vetor(TAM_BUFFER);

  msg = aloca_msg();

  read(socket, buffer, TAM_BUFFER);

  if (buffer[0] == MARCADOR) {

    msg->marcador = buffer[0];

    msg->tamanho = (buffer[1] >> 2);

    msg->dados = aloca_vetor(msg->tamanho);

    msg->seq = (buffer[1] << 6);
    msg->seq = (msg->seq >> 3);
    msg->seq |= (buffer[2] >> 5);

    msg->tipo = (buffer[2] << 3);
    msg->tipo = (msg->tipo >> 3);

    for (int i = 0; i < msg->tamanho; i++)
      msg->dados[i] = buffer[i + 3];

    msg->detec_erros = buffer[3 + msg->tamanho];

    free(buffer);
    return (msg);

  } else {

    return (NULL);
  }
}

uint8_t ler_msg(protocolo *msg) {

  uint8_t teste_crc = 0;

  if (!msg)
    return (-1);

  teste_crc = cal_crc8(msg);

  if (teste_crc == msg->detec_erros)
    return (msg->tipo);
  else
    return (ERRO_CRC);
}

/* Imprime mensagem */
void imprime_msg(protocolo *msg) {
  if (msg) {
    printf("****************************\n");
    printf("MARCADOR : %d \n", msg->marcador);
    printf("TAM : %d \n", msg->tamanho);
    printf("SEQ : %d \n", msg->seq);
    printf("TIPO : %d \n", msg->tipo);
    printf("DADOS : %s \n", (char *)msg->dados);
    printf("CRC : %d \n", msg->detec_erros);
    printf("****************************\n");
  }
}

void socket_errors(short errno, interface ifc) {
  switch (errno) {
  case -1:
    fprintf(stderr, "ERRO - Não foi possível abrir socket, permissão negada "
                    "(Necessário ser Root)\n");
    break;
  case -2:
    fprintf(stderr, "ERRO - Interface %s não encontrada\n", ifc.name);
    break;
  case -3:
    fprintf(stderr, "ERRO - Não foi possível conectar o socket a interface %s",
            ifc.name);
    break;
  case -4:
    fprintf(stderr,
            "ERRO - Não foi possível ativar o modo promíscuo na interface %s",
            ifc.name);
    break;
  }
  fprintf(stderr, "Abortando execução\n");
}

// Créditos: https://github.com/pavan-pan/CRC8
uint8_t CRC8_calc( uint8_t data, uint8_t size)
{
    uint8_t crc = 0;
    const uint8_t generator = 0x07;//Polynomial

    for (int j = 0; j < size; j++)
    {
        crc ^= data;

        //For each bit of the byte check for the MSB bit, if it is 1 then left shift the CRC and XOR with the polynomial
        //otherwise just left shift the variable
        for (int i = 0; i < 8; i++)
        {
            if ((crc & 0x80) != 0)
            {
                crc = (uint8_t)((crc << 1 ^ generator));
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}