#include "define.h"
#include "network.h"
#include "commands.h"

int receive_file(char *file_name, int socket)
{
    #ifdef DEBUG 
        log_message("Receiving file:");
        log_message(file_name);
    #endif

    // Create a file with "file_name"
    FILE *file = fopen(file_name, "wb");
    if (file == NULL)
    {
        perror("Error opening the file");
        return -1;
    }

    #ifdef DEBUG
        log_message("File opened!");
    #endif

    struct protocolo packet_buffer;
    struct protocolo *response = cria_msg(NULL, 0, 0, ACK, NULL);
    long long int packets_received = 1;
    int end_file_received = 0;

    /* Listen for packets and process them */
    while (!end_file_received)
    {
        printf("\r%s: %lld", file_name, packets_received);
        printf("\rReceiving [%s]. Packets count [%lld]", file_name, packets_received);
        fflush(stdout);
        int listen_status = listen_packet(&packet_buffer, TIMEOUT, socket);

        if (packet_buffer.tipo == FIM_TRANSMISSAO)
            end_file_received = 1;

        if (listen_status != 0) 
        {
            printf("Is the other side still connected?\n");
        }
        else if (packet_buffer.tipo == DADOS)
        {
            /* Write received data to the file */
            fwrite(packet_buffer.dados, packet_buffer.tamanho, 1, file);

            /* Send ACK packet */
            envia_msg(response, socket);

            if(! response){
                perror("Error sending ACK packet");
                fclose(file);
                free_packet(response);
                return -1;
            }
            packets_received++;
        }
    }
    printf("\n");

    fclose(file);

    /* Send OK packet */
    #ifdef DEBUG
        log_message("Sending OK packet");
    #endif

    // create_or_modify_packet(response, 0, 0, OK, NULL);
    envia_msg(response, socket);

    #ifdef DEBUG
        log_message("File received!");
    #endif

    printf("%s received!\n", file_name);

    free_packet(response);
    return 0;
}

void get_current_directory(char *buffer, size_t size){
  if(getcwd(buffer,size) == NULL)
    perror("Error getting current directory");
  strcat(buffer, "/");
}

void change_directory(char *key, const char delimiter[]){
  key = strtok(NULL, delimiter);

  if(chdir(key) != 0)
    printf("Failed to find the directory!\n");
}

void process_command(char file_names[][MAX_FILE_NAME_SIZE], char *key, const char delimiter[], int type_flag, int sockfd){

  key = strtok(NULL, delimiter);

  if (!key)
    printf("No arguments provided!\n");

  if(type_flag == LISTAR)
    printf("oi\n");
  else if(type_flag == BAIXAR)
    printf("oi\n");
  
}

void list_files(const char* directory) {
    DIR* dir;
    struct dirent* entry;

    // Open the directory
    dir = opendir(directory);

    if (dir == NULL) {
        printf("Error opening directory.\n");
        return;
    }

    // Store file names in an array
    char files[100][256];
    int fileCount = 0;

    // Iterate over directory entries
    while ((entry = readdir(dir)) != NULL) {
        sprintf(files[fileCount], "%s", entry->d_name);
        fileCount++;
    }

    // Calculate number of columns
    int numColumns = 3;  // Adjust the number of columns as desired

    // Calculate number of rows
    int numRows = (fileCount + numColumns - 1) / numColumns;

    // Print files in columns
    for (int i = 0; i < numRows; i++) {
        for (int j = i; j < fileCount; j += numRows) {
            printf("%-30s", files[j]);  // Adjust the spacing as desired
        }
        printf("\n");
    }

    // Close the directory
    closedir(dir);
}

void list_files_remote(int socket)
{
    protocolo *msg, *msg_recebida;
    int sequencia, contador;

    contador = 0;
    sequencia = 0;
    msg = NULL;
    msg = cria_msg(msg, 0, 0, LISTAR, NULL);
    envia_msg(msg, socket);
    timeout(socket, msg);
    do
    {
        msg_recebida = recebe_msg(socket, 1);
        sequencia = prox_seq(msg_recebida);
        switch (ler_msg(msg_recebida))
        {
        case ACK:
            printf("ACK recebido\n");
            free_packet(msg_recebida);
            free_packet(msg);
            cria_msg(msg, 0, sequencia, ACK, NULL);
            envia_msg(msg, socket);
            timeout(socket, msg);
            break;
        case NACK:
            printf("NACK recebido\n");
            free_packet(msg_recebida);
            envia_msg(msg, socket);
            timeout(socket, msg);
            break;
        case ERRO:
            printf("Erro ao listar\n");
            free_packet(msg_recebida);
            cria_msg(msg, 0, sequencia, NACK, NULL);
            envia_msg(msg, socket);
            timeout(socket, msg);
            break;
        case ERRO_CRC:
            printf("Erro CRC\n");
            free_packet(msg_recebida);
            cria_msg(msg, 0, sequencia, NACK, NULL);
            envia_msg(msg, socket);
            timeout(socket, msg);
            break;
        case MOSTRA_NA_TELA:
            contador++;
            printf("%d - ", contador);
            printf("%s\n", msg_recebida->dados);
            free_packet(msg_recebida);
            cria_msg(msg, 0, sequencia, ACK, NULL);
            envia_msg(msg, socket);
            timeout(socket, msg);
            break;
        case FIM_TRANSMISSAO:
            break;
        }
    } while (ler_msg(msg_recebida) != FIM_TRANSMISSAO);
}

void print_commands() {
  printf("- ls : Lista os videos do servidor.\n");
  printf("- ls -e <extensao> : Listar os videos da extensao selecionada .mp4 ou .avi.\n");
  printf("- dl <arquivo> : Faz o download do arquivo selecionado.\n");
  printf("- dl -e <extensao> : Faz o download dos videos da extensao selecionada..\n");
  printf("- dl : Faz o download de todos os arquivos\n");
  printf("- help : Mostra os comando disponiveis!\n");
  printf("- exit : Sai da programa.\n");
}

// int main(){

//   int sockfd = openRSocket("enp1s0f1");

//   char input[100];
  
//   char *key;
//   char file_name[MAX_NUM_FILES][MAX_FILE_NAME_SIZE];
//   const char delimiter[] = " \n";

//   current_dir = malloc(sizeof(int) * MAX_DATA_SIZE);
//   get_current_directory(current_dir, MAX_DATA_SIZE);

//   printf("Bem vindo ao Streaming de Videos!\n");
//   printf("\nComandos disponíveis:\n");
//   print_commands();

//   printf("\n");

//   while(1){
    
//     printf("\nclient:%s$ ", current_dir);
//     fgets(input, sizeof(input), stdin);

//     key = strtok(input, delimiter);

//     if(!key)
//       continue;

//     if(strcmp(key, "ls") == 0)
//       list_files(current_dir);
//     else if (strcmp(key, "dl") == 0)
//       printf("oi\n");
//       //transferência de arquivos
//     else if(strcmp(key, "exit")){
//       printf("Desligando...");
//       return 0;
//     } 
//     else if (strcmp(key, "help"))
//       print_commands();
//     else 
//       printf("Comando não encontrado: %s\n", key);
//   }

//   return 0;
// }


// ##################################################################

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

//   log_message(file_name);

  if (p == NULL) {
    perror("Could not create packet!");
    return -1;
  }

  /* Send packet for start backup single file */
  if (send_packet_and_wait_for_response(p, p, TIMEOUT, socket) != 0) {
    // log_message("Error while trying to reach server!\n");
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
      free_packet(p);
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
    free_packet(p);
    return -1;
  }

  free_packet(p);
//   log_message("File sent successfully!");
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