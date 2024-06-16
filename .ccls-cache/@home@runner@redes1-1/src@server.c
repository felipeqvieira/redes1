#include "define.h"

int send_single_file(char *file_name, int socket) {
  int long long packets_count = 0;

  if (file_name == NULL) {
    perror("src_path is NULL!");
    return -1;
  }

  FILE *file = fopen(file_name, "rb");

  if (file == NULL) {
    perror("Could not open file!");
    return -1;
  }

#ifdef DEBUG
  log_message("Sending file...");
  log_message("Opening file...");
  log_message("File opened!");
#endif

  protocolo *p = cria_msg(NULL, MAX_DATA_SIZE, 0, DADOS, file_name);

  log_message(file_name);

  if (p == NULL) {
    perror("Could not create packet!");
    return -1;
  }

  /* Send packet for start backup single file */
  if (send_packet_and_wait_for_response(p, p, TIMEOUT, socket) != 0) {
    log_message("Error while trying to reach server!\n");
    return -1;
  }

#ifdef DEBUG
  log_message("Sending PT_BACKUP_ONE_FILE packet...");
  log_message("Response received!");
#endif

  /* Create buffers */
  uint8_t data_buffer[63];
  protocolo p_buffer;

  int file_read_bytes = MAX_DATA_SIZE;
  long long file_size = get_file_size(file_name);
  long long packets_quantity = ceil(file_size / (float)(MAX_DATA_SIZE));

  int packet_sequence = 1;
  for (int i = 0; i < packets_quantity; i++) {
    printf("\r%s: %lld/%lld", file_name, packets_count, packets_quantity);
    fflush(stdout);
    /* Sometimes the last packet is not full,
     * so we need to know how many bytes we need to read.
     */

    if (i == packets_quantity - 1) {
      file_read_bytes = file_size % MAX_DATA_SIZE;
      p->dados[file_read_bytes] = '\0';
    }

    /* Read bytes from file. */
    fread(data_buffer, file_read_bytes, 1, file);

    /* Put read data into packet. */
    cria_msg(p, file_read_bytes, packet_sequence, DADOS, data_buffer);

    if (send_packet_and_wait_for_response(p, &p_buffer, TIMEOUT, socket) != 0) {
      printf("Error while sending file: %s \n", file_name);
      fclose(file);
      free_msg(p);
      return -1;
    }

    /* Reset packet sequence number. */
    if (packet_sequence == MAX_SEQUENCE)
      packet_sequence = 0;
    packet_sequence++;
    packets_count++;
  }

  printf("\n");
  fclose(file);

  /* Send end file packet. */
  cria_msg(p, 0, 0, FIM_TRANSMISSAO, NULL);
  if (send_packet_and_wait_for_response(p, &p_buffer, TIMEOUT, socket) != 0) {
#ifdef DEBUG
    log_message("Couldn't send end file packet!\n");
#endif
    free_msg(p);
    return -1;
  }

  free_msg(p);
  log_message("File sent successfully!");
  return 0;
}

// int main() {
//   printf("Criando socket...\n");
//   int socket = openRSocket("enp1s0f1");
//   printf("Socket criado!\n");
//   printf("Servidor Rodando!\n");

//   // char current_directory[100];
//   // get_current_directory(current_directory, sizeof(current_directory));

//   // printf(current_directory);

//   struct protocolo buffer;
//   struct protocolo *packet = cria_msg(NULL, 0, 0, ACK, NULL);
//   char *file_name = NULL;
//   char *full_path_to_file = NULL;

//   while (1) {
//     printf("Waiting for request...");
//     recebe_msg(&buffer, 9999, socket);

//     switch (buffer.tipo) {
//     case ACK:
//       printf("ACK recebido!");
//       break;
//     case NACK:
//       printf("NACK recebido!");
//       break;
//     case BAIXAR:
//       printf("BAIXAR recebido!");

//       // Receive file name
//       file_name = arrayToString(buffer.dados, buffer.tamanho);
//       printf("File to restore:");
//       printf("%s",file_name);

//       if (access(file_name, F_OK) != 0) {
//         printf("File does not exist!");

//         cria_msg(packet, MAX_DATA_SIZE, 0, ERRO, "File does not exist!");
//         envia_msg(packet, socket); // send ERROR

//         free(file_name);
//         break;
//       } else {
//         printf("Requested file exists!");
//       }

//       // Send OK
//       // cria_msg(packet, 0, 0, PT_OK, NULL);
//       envia_msg(packet, socket);

//       printf("Sending:");
//       printf("%s", file_name);

//       if (send_single_file(file_name, socket) != 0) {
//         printf("Error sending file!");

//         // Error sending file
//         cria_msg(packet, 0, 0, ERRO, "Error sending file!");
//         envia_msg(packet, socket); // send ERROR

//         free(file_name);
//         free(full_path_to_file);
//         break;
//       }

//       free(file_name);
//       free(full_path_to_file);
//       break;

//     case FIM_TRANSMISSAO:
//       // cria_msg(packet, 0, 0, OK , NULL);
//       envia_msg(packet, socket);
//       break;
//     default:
//       printf("Invalid packet received!\n");
//       break;
//     }

//     // Reset the buffer
//     buffer.marcador = 0;
//   }

//   // Close the socket after sending and receiving frames
//   close(socket);
//   return 0;
// }