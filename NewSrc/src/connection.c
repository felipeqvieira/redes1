#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include "../lib/connection.h"
#include "../lib/log.h"
#include "../lib/utils.h"

double time_passed(clock_t start, clock_t end);
void shift_bits(struct packet *packet);
void unshift_bits(struct packet* p);
int is_start_marker_correct(struct packet *p);
int is_crc8_right(struct packet *p);

// Calcula o CRC8
uint8_t crc8_calc(uint8_t *data, size_t size) 
{

  uint8_t valor = *data;
  uint8_t crc = 0;
  const uint8_t generator = 0x07; // Polynomial

  for (int j = 0; j < (int)size; j++) {
    crc ^= valor;

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

/* Verify packet parameters */
int verify_packet_parameters(uint8_t size, uint8_t sequence, uint8_t type) 
{
    if ((size > MAX_DATA_SIZE) || (sequence > MAX_SEQUENCE) || (type > MAX_TYPE)) 
        return 0;

    return 1;
}

/* 
 * Creates or modifies a packet with the given parameters.
 * 
 * @param packet The packet pointer, if NULL creates a new packet, otherwise modifies the existing packet.
 * @param size The size of the data in the packet.
 * @param sequence The sequence number of the packet.
 * @param type The type of the packet.
 * @param data The data to be sent in the packet.
 * @return A pointer to the created or modified packet.
 * 
 * @see destroy_packet
*/
struct packet *create_or_modify_packet(struct packet *packet, uint8_t size, uint8_t sequence, uint8_t type, void *data) 
{   

    if (packet == NULL) 
    {
        packet = calloc(1, sizeof(struct packet));
        if (packet == NULL) 
        {
            perror("Alloc failed!");
            exit(EXIT_FAILURE);
        }
    }
    else if (!verify_packet_parameters(size, sequence, type)) 
    {
        perror("Invalid packet!");
        destroy_packet(packet);
        return NULL;
    }

    packet->start_marker = START_MARKER;
    packet->size = size;
    packet->sequence = sequence;
    packet->type = type;
    packet->crc8 = 0;

    if (data != NULL) 
    {
        memcpy(&packet->data, data, size);
        packet->crc8 = crc8_calc(data, size);
    }

    return packet;
}

/* 
 * Destroys a packet.
 * 
 * @param p The packet to be destroyed.
 * 
 * @see create_or_modify_packet
*/

/* Liberar o pacote se existir */
void destroy_packet(struct packet *p) 
{
    if (p == NULL) 
        return;
    free(p);
}

int send_packet(struct packet *packet, int socket)
{

    shift_bits(packet);
    if(send(socket, packet, sizeof(struct packet), 0) == -1) 
    {
        perror("send");
        close(socket);
        exit(EXIT_FAILURE);
    }
    unshift_bits(packet);
    return 0;
}

/* Sends a packet and waits for a ACK or OK.
 * If an NACK is received, the packet is sent again.
 * 
 * @param packet The packet to be sent. 
 * @param response The response packet to be filled.
 * @param timeout The timeout in seconds.
 * @param socket The socket to be used.
 * 
 * @return 0 if the packet was sent and the response was received.
 * @return -1 if an error occurred.
 * @return -2 if the timeout expired.
 * 
*/
int send_packet_and_wait_for_response(struct packet *packet, struct packet *response, int timeout, int socket)
{
    int response_confirmation = 0;
    struct packet *response_aux = create_or_modify_packet(NULL, 0, 0, ACK, NULL);

    while(!response_confirmation)
    {
        send_packet(packet, socket);
        show_packet_data(packet);
        int listen_response = listen_packet(response_aux, timeout, socket);

        if(listen_response == -1) 
            return -1;
        else if(response_aux->type == ACK || response_aux->type == OK || response_aux->type == ERROR)
            response_confirmation = 1;
    }

    create_or_modify_packet(response, response_aux->size, response_aux->sequence, response_aux->type, response_aux->data);

    return 0;
}

/* Shift bits function */
void shift_bits(struct packet* p) {
    p->size = p->size << 2; // shift left 2 bits to use upper 6 bits
    p->sequence = p->sequence << 3; // shift left 3 bits to use upper 5 bits
    p->type = p->type << 3; // shift left 3 bits to use upper 5 bits
}

void unshift_bits(struct packet* p) {
    p->size = p->size >> 2; // shift right 2 bits to get back original 6 bits
    p->sequence = p->sequence >> 3; // shift right 3 bits to get back original 5 bits
    p->type = p->type >> 3; // shift right 3 bits to get back original 5 bits
}

/* clear buffer */
void clear_buffer(struct packet *packet)
{
    memset(packet, 0, sizeof(struct packet));
}

/* 
 * Listens for a valid packet.
 * 
 * @param buffer The buffer to be filled.
 * @param timeout The timeout in seconds. 
 * @param socket The socket to be listened.
 * 
 * @return 0 if the packet was received.
 * @return -1 if an error occurred. 
 * @return -2 if the timeout expired.
 * 
*/
int listen_packet(struct packet *buffer, int timeout, int socket)
{
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
    while(time_passed(start, now) < timeout)
    {
        clear_buffer(buffer);
        t_out.tv_sec = timeout - time_passed(start, now);

        int ready = select(socket + 1, &read_fds, NULL, NULL, &t_out);
        
        if (ready == -1) 
        {
            return -1;
        } 
        else if (ready == 0) 
        {
            return -2; // Timeout
        } 
        else 
        {
            ssize_t bytes_received = recvfrom(socket, buffer, sizeof(struct packet), 0, NULL, NULL);
            unshift_bits(buffer);
            
            if (bytes_received == -1) 
            {
                return -1;
            }

            /* Checks if the packet is from client and if it's crc8 is right  */ 
            if(is_start_marker_correct(buffer) )
            {
                if (is_crc8_right(buffer) == 0)
                {
                    struct packet *nack = create_or_modify_packet(NULL, 0, 0, NACK, NULL);
                    send_packet(nack, socket);
                    destroy_packet(nack);
                }
                else
                {
                    // valid packet
                    return 0;
                }
            }
        }
        now = clock();
    }
    return -2; 
}

/* 
 * Checks if the start marker is correct and if the crc8 is correct.
 * 
 * @param p The packet to be checked.
*/
int is_start_marker_correct(struct packet *p)
{
    if(p->start_marker != (uint8_t)(START_MARKER))
    {
        return 0;
    }
    return 1;
}

// Verificar se o CRC8 é o correto
int is_crc8_right(struct packet *p)
{
    if(p->crc8 != crc8_calc(p->data, p->size))
    {
        #ifdef DEBUG
            log_message("Vertical crc8 erro!");
        #endif
        return 0;
    }

    return 1;
}

double time_passed(clock_t start, clock_t end)
{
    return ((double)(end - start) / CLOCKS_PER_SEC);
}