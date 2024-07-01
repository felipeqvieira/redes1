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

    char input[100];
    char *token;
    const char delimiter[] = " \n";
    current_dir = malloc(sizeof(int) * MAX_DATA_SIZE);
    get_current_directory(current_dir, MAX_DATA_SIZE);


    printf("Bem vindo ao Flix!\n\n");
    printf("Comandos disponíveis:\n");
    print_commands();
    printf("\n");

    while (1) 
    {
        printf("\nFlix:%s$ ",current_dir);
        fgets(input, sizeof(input), stdin);
        
        // switch ()
        // {
        // case: LIST
            
        //     break;
        // case: DOWNLOAD
        //     break;
        // case: HELP
        //     print_commands();
        //     break;
        // case: EXIT
        //     printf("Tchau, espero que tenha gostado!\n");
        //     return 0;
        //     break;
        // default:
        //     printf("Comando não existe: %s\n", token);
        //     break;
        // }

        token = strtok(input, delimiter);
        if (token == NULL) {
            continue;
        }
        if (strcmp(token, "list") == 0)
        {
            process_command(token, delimiter, LIST, sockfd);
            
            // struct packet response;
            // listen_packet(&response, 9999, sockfd);
            // show_packet_data(&response);
            // if (response.type == LIST)
            // {
            //     printf("Lista de arquivos disponíveis:\n%s\n", response.data);
            // }
            // else
            // {
            //     printf("Falha ao receber a lista de arquivos.\n");
            // }
            
        }
        else if(strcmp(token, "download") == 0)
        {
            process_command(token, delimiter, DOWNLOAD,sockfd);
        }
        else if(strcmp(token, "help") == 0)
        {
            print_commands();
        }
        else if(strcmp(token, "exit") == 0)
        {
            printf("Tchau, espero que tenha gostado!\n");
            return 0;
        }
        else 
        {
            printf("Comando não existe: %s\n", token);
        }
    }

    return 0;
}

void process_command(char *token, const char delimiter[], int type_flag, int sockfd)
{
    token = strtok(NULL, delimiter);

    if(type_flag == LIST)
    {
        struct packet list_packet;
        create_or_modify_packet(&list_packet, 0, 0, LIST, NULL);
        send_packet(&list_packet, sockfd);
    }
    else if(type_flag == DOWNLOAD)
    {
        if(token == NULL)
        {
            printf("--> Please provide arguments for the command.\n");
            return;
        }
        restore_single_file(token, sockfd);
    }
}

void print_commands()
{
    printf("- list : Lista os filmes disponiveís para download.\n");
    printf("- download <number> : Faz o download e inicia o video selecionado.\n");
    printf("- help: Mostra os comandos disponíveis!\n");
    printf("- exit: Sai do programa.\n");
}