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

protocolo *cria_msg(protocolo *packet, uint8_t size, uint8_t sequence,
                    uint8_t type, void *data) {
  if (!verify_packet_parameters(size, sequence, type)) {
    perror("Invalid packet!");
    return NULL;
  }

  if (packet == NULL) {
    packet = calloc(1, sizeof(struct protocolo));
    if (packet == NULL) {
      perror("Alloc failed!");
      exit(EXIT_FAILURE);
    }
  }

  packet->marcador = MARCADOR;
  packet->tamanho = size;
  packet->seq = sequence;
  packet->tipo = type;
  packet->crc8 = 0;

  if (data != NULL) {
    memcpy(&packet->dados, data, size);
    // packet->crc8 = CRC8_calc((data, size); Vamos verificar depois
  }

  return packet;
}

void envia_msg(protocolo *msg, int socket) {

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

    buffer[3 + msg->tamanho] = msg->crc8;

    if (write(socket, buffer, TAM_BUFFER) != -1)
      // Verificar depois se vamos manter assim
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

  if (read(socket, buffer, TAM_BUFFER) != -1)
    // Verificar depois se vamos manter assim

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

      msg->crc8 = buffer[3 + msg->tamanho];

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

  // Vamos verificar depois
  // teste_crc = CRC8_calc((msg->dados, msg->tamanho);

  // if (teste_crc == msg->crc8)
  //   return (msg->tipo);
  // else
  //   return (ERRO_CRC);
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
    printf("CRC : %d \n", msg->crc8);
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
uint8_t CRC8_calc(uint8_t data, uint8_t size) {
  uint8_t crc = 0;
  const uint8_t generator = 0x07; // Polynomial

  for (int j = 0; j < size; j++) {
    crc ^= data;

    // For each bit of the byte check for the MSB bit, if it is 1 then left
    // shift the CRC and XOR with the polynomial otherwise just left shift the
    // variable
    for (int i = 0; i < 8; i++) {
      if ((crc & 0x80) != 0) {
        crc = (uint8_t)((crc << 1 ^ generator));
      } else {
        crc <<= 1;
      }
    }
  }
  return crc;
}

void log_message(const char *message) {
  time_t now;
  time(&now);
  struct tm *local_time = localtime(&now);
  char timestamp[20];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local_time);

  printf("%s - %s\n", timestamp, message);
}

int verify_packet_parameters(uint8_t size, uint8_t sequence, uint8_t type) {
  if ((size > MAX_DATA_SIZE) || (sequence > MAX_SEQUENCE) || (type > MAX_TYPE))
    return 0;

  return 1;
}

char *arrayToString(uint8_t *array, size_t length) {
  char *str = (char *)malloc((length + 1) *
                             sizeof(char)); // Allocate memory for the string

  if (str == NULL) {
    fprintf(stderr, "Memory allocation failed.\n");
    return NULL;
  }

  for (size_t i = 0; i < length; i++) {
    str[i] =
        (char)array[i]; // Convert each element to char and store in the string
  }

  str[length] = '\0'; // Add the null-terminating character

  return str;
}

/* Shift bits function */
void shift_bits(struct protocolo *p) {
  p->tamanho = p->tamanho << 2; // shift left 2 bits to use upper 6 bits
  p->seq = p->seq << 2;         // shift left 2 bits to use upper 6 bits
  p->tipo = p->tipo << 4;       // shift left 4 bits to use upper 4 bits
}

void unshift_bits(struct protocolo *p) {
  p->tamanho =
      p->tamanho >> 2;    // shift right 2 bits to get back original 6 bits
  p->seq = p->seq >> 2;   // shift right 2 bits to get back original 6 bits
  p->tipo = p->tipo >> 4; // shift right 4 bits to get back original 4 bits
}

void clear_buffer(struct protocolo *packet) {
  memset(packet, 0, sizeof(struct protocolo));
}

int is_start_marker_correct(struct protocolo *p) {
  if (p->marcador != (uint8_t)(MARCADOR)) {
    return 0;
  }
  return 1;
}

int listen_packet(struct protocolo *buffer, int timeout, int socket) {
  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(socket, &read_fds);

  struct timeval t_out;
  t_out.tv_usec = 0;

  clock_t start = clock();
  clock_t now = clock();

  /* The timeout needs to be checked in the while loop
   * because other packets that it's not the one we want
   * can be received (packets that isn't comming from client).
   */
  while (time_passed(start, now) < timeout) {
    clear_buffer(buffer);
    t_out.tv_sec = timeout - time_passed(start, now);

    int ready = select(socket + 1, &read_fds, NULL, NULL, &t_out);

    if (ready == -1) {
      return -1;
    } else if (ready == 0) {
      return -2; // Timeout
    } else {
      ssize_t bytes_received =
          recvfrom(socket, buffer, sizeof(struct protocolo), 0, NULL, NULL);
      unshift_bits(buffer);

      if (bytes_received == -1) {
        return -1;
      }

      /* Checks if the packet is from client and if it's parity is right  */
      if (is_start_marker_correct(buffer)) 
      {
        //   if (is_parity_right(buffer) == 0) {
        //   struct protocolo *nack = cria_msg(NULL, 0, 0, NACK, NULL);
        //   envia_msg(nack, socket);
        //   free_msg(nack);
        // } else {
        //   // valid packet
        //   return 0;
        // }
      }
    }
    now = clock();
  }
  return -2;
}

int send_packet_and_wait_for_response(struct protocolo *packet,
                                      struct protocolo *response, int timeout,
                                      int socket) {
  int response_confirmation = 0;
  struct protocolo *response_aux = cria_msg(NULL, 0, 0, ACK, NULL);

  while (!response_confirmation) {
    envia_msg(packet, socket);

    int listen_response = listen_packet(response_aux, timeout, socket);

    if (listen_response == -1)
      return -1;
    else if (response_aux->tipo == ACK || response_aux->tipo == PT_OK ||
             response_aux->tipo == ERRO || response_aux->tipo == PT_MD5)
      response_confirmation = 1;
  }

  cria_msg(response, response_aux->tamanho, response_aux->seq,
           response_aux->tipo, response_aux->dados);

  return 0;
}

long long int get_file_size(char *path) {
  struct stat st;
  if (stat(path, &st) == -1) {
    perror("Could not get file size!");
    return -1;
  }
  return st.st_size;
}

double time_passed(clock_t start, clock_t end) {
  return ((double)(end - start) / CLOCKS_PER_SEC);
}