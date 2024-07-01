#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <limits.h>
#include "../lib/socket.h"
#include "../lib/connection.h"
#include "../lib/command.h"
#include "../lib/utils.h"
#include "../lib/log.h"
#define PATH_MAX 100

void process_command(char *token, const char delimiter[], int type_flag, int sockfd);
void print_commands();

char *current_dir;

int main()
{
    int sockfd = create_socket("lo");
  	// int sockfd = create_socket("enp1s0f1");
    char input[100];
    char *token;
    const char delimiter[] = " \n";
    current_dir = malloc(sizeof(int) * MAX_DATA_SIZE);
    get_current_directory(current_dir, MAX_DATA_SIZE);
    struct packet buffer;
    struct packet *packet = create_or_modify_packet(NULL, 0, 0, ACK, NULL);
    char *file_name = NULL;
    // char *full_path_to_file = NULL;


    printf("Bem vindo ao Flix!\n\n");
    printf("Comandos disponíveis:\n");
    print_commands();
    printf("\n");

    while (1) 
    {
        printf("\nFlix:%s$ ",current_dir);
        fgets(input, sizeof(input), stdin);
        fflush(stdin);
        token = strtok(input, delimiter);
        if (token == NULL) {
            continue;
        }
        if (strcmp(token, "list") == 0)
            process_command(token, delimiter, LIST, sockfd);
        else if(strcmp(token, "download") == 0)
            process_command(token, delimiter, DOWNLOAD,sockfd);
        else if(strcmp(token, "help") == 0)
            print_commands();
        else if(strcmp(token, "exit") == 0)
        {
            printf("Tchau, espero que tenha gostado!\n");
            return 0;
        }
        else
            printf("Comando não existe: %s\n", token);
        
        while(buffer.type != END_FILE)
        {
            listen_packet(&buffer, 9999, sockfd);
            switch(buffer.type)  
            {
            case ACK:
                log_message("ACK recebido!");
                break;
            case NACK:
                log_message("NACK recebido!");
                break;
            case SHOW_IN_SCREEN:
                log_message("SHOW_IN_SCREEN recebido!");
                file_name = uint8ArrayToString(buffer.data, buffer.size);
                printf("%s\n", file_name);
                create_or_modify_packet(packet, 0, 0, ACK, NULL);
                send_packet(packet, sockfd);
                break;
            case EXIT:
                printf("Tchau, espero que tenha gostado!\n");
                return 0;
                break;
            case END_FILE:
                log_message("END_FILE recebido!");
                create_or_modify_packet(packet, 0, 0, ACK, NULL);
                send_packet(packet, sockfd);
                break;
            }
        }
        // Reset the buffer
        buffer.start_marker = 0;
    }

    close(sockfd);
    return 0;
}

void process_command(char *token, const char delimiter[], int type_flag, int sockfd)
{
    token = strtok(NULL, delimiter);
    struct packet packet;

    if(type_flag == LIST)
    {
        create_or_modify_packet(&packet, 0, 0, LIST, NULL);
        send_packet(&packet, sockfd);
    }
    else if(type_flag == DOWNLOAD)
    {
        if(token == NULL)
        {
            printf("--> Please provide arguments for the command.\n");
            return;
        }
        printf("--> Downloading file: %s\n", token);
        create_or_modify_packet(&packet, strlen(token), 0, DOWNLOAD, token);
        send_packet(&packet, sockfd);
    }
}

void print_commands()
{
    printf("- list : Lista os filmes disponiveís para download.\n");
    printf("- download <number> : Faz o download e inicia o video selecionado.\n");
    printf("- help: Mostra os comandos disponíveis!\n");
    printf("- exit: Sai do programa.\n");
}
