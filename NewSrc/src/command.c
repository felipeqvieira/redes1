#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../lib/command.h"
#include "../lib/connection.h"
#include "../lib/log.h"
#include "../lib/utils.h"
#include <dirent.h>

int is_video_file(const char *filename)
{
    const char *video_extensions[] = { ".mp4", ".mkv", ".avi", ".mov", ".flv", ".wmv", NULL };
    const char **ext = video_extensions;

    while (*ext)
    {
        if (strstr(filename, *ext))
        {
            return 1;
        }
        ext++;
    }
    return 0;
}

int list_video_files_in_directory(char *directory, int socket)
{
    DIR *d;
    struct dirent *dir;
    d = opendir(directory); // Diretório atual do servidor
    if (d == NULL) 
    {
        perror("opendir");
        return -1;
    }

    char file_list[MAX_DATA_SIZE];
    memset(file_list, 0, sizeof(file_list));
    struct packet response_packet;
    struct packet *packet= NULL;

    while ((dir = readdir(d)) != NULL) 
    {
        // if (dir->d_type == 8)  // Considera apenas arquivos regulares
        // {
            size_t name_len = strlen(dir->d_name);  // Calcula o comprimento do nome do arquivo
            if (name_len + 1 < sizeof(file_list))  // Verifica se há espaço no buffer
            {
                memcpy(file_list, dir->d_name, name_len);  // Copia o nome do arquivo para o buffer
                packet = create_or_modify_packet(NULL, MAX_DATA_SIZE, 0, SHOW_IN_SCREEN, file_list);
                show_packet_data(packet);
                send_packet_and_wait_for_response(packet, &response_packet, TIMEOUT, socket);
                destroy_packet(packet);
                memset(file_list, 0, sizeof(file_list));  // Limpa o buffer
            }
            else
            {
                break;  // Sem espaço suficiente no buffer
            }
        // }
    }
    closedir(d);

    packet = create_or_modify_packet(NULL, 0, 0, END_FILE, NULL);
    send_packet_and_wait_for_response(packet, &response_packet, TIMEOUT, socket);
    // destroy_packet(packet);

    return 0;
}
    
// ###########################################################################################

/*
 * Sends a single file to the server.
 *
 * @param src_path The path of the file to be sent.
 * @param socket The socket to send the file.
 * @return 0 if the file was sent successfully, -1 otherwise.
 *
 */
int send_single_file(char *file_name, int socket)
{
    int long long packets_count = 0;

    if (file_name == NULL)
    {
        perror("src_path is NULL!");
        return -1;
    }

    FILE *file = fopen(file_name, "rb");

    if (file == NULL)
    {
        perror("Could not open file!");
        return -1;
    }


    #ifdef DEBUG
        log_message("Sending file...");
        log_message("Opening file...");
        log_message("File opened!");
    #endif

    struct packet *p = create_or_modify_packet(NULL, MAX_DATA_SIZE, 0, PT_BACKUP_ONE_FILE, file_name);

    log_message(file_name);
    if (p == NULL)
    {
        perror("Could not create packet!");
        return -1;
    }

    /* Send packet for start backup single file */
    if (send_packet_and_wait_for_response(p, p, TIMEOUT, socket) != 0)
    {
        log_message("Error while trying to reach server!\n");
        return -1;
    }

    #ifdef DEBUG
        log_message("Sending PT_BACKUP_ONE_FILE packet...");
        log_message("Response received!");
    #endif

    /* Create buffers */
    uint8_t data_buffer[63];
    struct packet p_buffer;

    int file_read_bytes = MAX_DATA_SIZE;
    long long file_size = get_file_size(file_name);
    long long packets_quantity = ceil(file_size / (float)(MAX_DATA_SIZE));

    int packet_sequence = 1;
    for (int i = 0; i < packets_quantity; i++)
    {
        printf("\r%s: %lld/%lld", file_name, packets_count, packets_quantity);
        fflush(stdout);
        /* Sometimes the last packet is not full,
         * so we need to know how many bytes we need to read.
         */

        if (i == packets_quantity - 1)
        {
            file_read_bytes = file_size % MAX_DATA_SIZE;
            p->data[file_read_bytes] = '\0';
        }

        /* Read bytes from file. */
        fread(data_buffer, file_read_bytes, 1, file);

        /* Put read data into packet. */
        create_or_modify_packet(p, file_read_bytes, packet_sequence, DATA, data_buffer);

        if (send_packet_and_wait_for_response(p, &p_buffer, TIMEOUT, socket) != 0)
        {
            printf("Error while sending file: %s \n", file_name);
            fclose(file);
            destroy_packet(p);
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
    create_or_modify_packet(p, 0, 0, END_FILE, NULL);
    if (send_packet_and_wait_for_response(p, &p_buffer, TIMEOUT, socket) != 0)
    {
#ifdef DEBUG
        log_message("Couldn't send end file packet!\n");
#endif
        destroy_packet(p);
        return -1;
    }

    destroy_packet(p);
    log_message("File sent successfully!");
    return 0;
}

/*
 * Receives a single file from the client.
 *
 * @param file_name The full path of the file to be received.
 * @param socket The socket to receive the file.
 *
 * @return 0 if the file was received successfully, -1 otherwise.
 *
 */
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

    struct packet packet_buffer;
    struct packet *response = create_or_modify_packet(NULL, 0, 0, ACK, NULL);
    long long int packets_received = 1;
    int end_file_received = 0;

    /* Listen for packets and process them */
    while (!end_file_received)
    {
        printf("\r%s: %lld", file_name, packets_received);
        printf("\rReceiving [%s]. Packets count [%lld]", file_name, packets_received);
        fflush(stdout);
        int listen_status = listen_packet(&packet_buffer, TIMEOUT, socket);

        if (packet_buffer.type == END_FILE)
            end_file_received = 1;

        if (listen_status != 0) 
        {
            printf("Is the other side still connected?\n");
        }
        else if (packet_buffer.type == DATA)
        {
            /* Write received data to the file */
            fwrite(packet_buffer.data, packet_buffer.size, 1, file);

            /* Send ACK packet */
            if (send_packet(response, socket) == -1)
            {
                perror("Error sending ACK packet");
                fclose(file);
                destroy_packet(response);
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

    create_or_modify_packet(response, 0, 0, OK, NULL);
    send_packet(response, socket);

    #ifdef DEBUG
        log_message("File received!");
    #endif

    printf("%s received!\n", file_name);

    destroy_packet(response);
    return 0;
}


void restore_single_file(char *file_name, int socket)
{
    struct packet *p = create_or_modify_packet(NULL, MAX_FILE_NAME_SIZE, 0, PT_RESTORE_ONE_FILE, file_name);

    #ifdef DEBUG
        log_message("Sending start restore single file packet");
    #endif
    /* Send packet for start restore single file */
    if (send_packet_and_wait_for_response(p, p, TIMEOUT, socket) != 0)
    {
        printf(" Error while sending start restore single file packet!\n"); // erro aqui
        return;
    }

    if (p->type == ERROR)
    {
        char *error_msg = uint8ArrayToString(p->data, p->size);

        #ifdef DEBUG
            log_message("Error while restoring file!");
            log_message("This file doesn't exist in the server!");
        #endif

        destroy_packet(p);
        free(error_msg);
        return;
    }

    // Listen for PT_BACKUP_ONE_FILE packet
    if (listen_packet(p, TIMEOUT, socket) != 0)
    {
        #ifdef DEBUG
            log_message("Error while listening for PT_BACKUP_ONE_FILE packet!");
        #endif
        destroy_packet(p);
        return;
    }

    // Send OK packet
    create_or_modify_packet(p, 0, 0, OK, NULL);
    send_packet(p, socket);

    receive_file(file_name, socket);

    /* Send end restore single file packet */
    create_or_modify_packet(p, 0, 0, END_FILE, NULL);
    if (send_packet_and_wait_for_response(p, p, TIMEOUT, socket) != 0)
    {
        // printf(" Error while sending end restore single file packet!\n");
        destroy_packet(p);
        return;
    }

    destroy_packet(p);
    return;
}