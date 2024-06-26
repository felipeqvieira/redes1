#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <dirent.h>
#include "../lib/socket.h"
#include "../lib/connection.h"
#include "../lib/command.h"
#include "../lib/log.h"
#include "../lib/utils.h"

#define FILE_MAP_NAME_TO_PATH "name_to_path.txt"

int main()
{
    log_message("Criando soquete...");
    int socket = create_socket("lo");
    log_message("Soquete criado!");
    log_message("Servidor ativo e funcionando!");
    char current_directory[100];
    get_current_directory(current_directory, sizeof(current_directory));

    // log_message(current_directory);


    struct packet buffer;
    struct packet *packet = create_or_modify_packet(NULL, 0, 0, ACK, NULL);
    char *file_name = NULL;
    char *full_path_to_file = NULL;
    while (1)
    {
        log_message("Esperando requisição...");
        listen_packet(&buffer, 9999, socket);
        switch(buffer.type)  
        {
        case ACK:
            log_message("ACK recebido!");
            break;
        case NACK:
            log_message("NACK recebido!");
            break;
        case LIST:
            log_message("LIST recebido!");
            list_video_files_in_directory(current_directory, socket);
            break;
        case DOWNLOAD:
            log_message("RESTORE_FILE recebido!");

            // Receive file name
            file_name = uint8ArrayToString(buffer.data, buffer.size);
            log_message("Arquivo para fazer o download:");
            log_message(file_name);

            if (access(file_name, F_OK) != 0)
            {
                log_message("Arquivo não existe!");

                create_or_modify_packet(packet, MAX_DATA_SIZE, 0, ERROR, "Arquivo não existe");
                send_packet(packet, socket); // send ERROR

                free(file_name);
                break;
            }
            else
            {
                log_message("Requested file exists!");
            }

            // Send OK
            create_or_modify_packet(packet, 0, 0, OK, NULL);
            send_packet(packet, socket);

            log_message("Sending:");
            log_message(file_name);

            if (send_single_file(file_name, socket) != 0)
            {
                log_message("Error sending file!");

                // Error sending file
                create_or_modify_packet(packet, 0, 0, ERROR, "Error sending file!");
                send_packet(packet, socket); // send ERROR

                free(file_name);
                free(full_path_to_file);
                break;
            }

            free(file_name);
            free(full_path_to_file);
            break;

        // case END_FILE:
            // create_or_modify_packet(packet, 0, 0, OK, NULL);
            // send_packet(packet, socket);
            // break;
        default:
            show_packet_data(&buffer);
            printf("Pacote invalido recebido!\n");
            break;
        }
        // Reset the buffer
        buffer.start_marker = 0;
    }

    // Fecha o soquete
    close(socket);
    return 0;
}