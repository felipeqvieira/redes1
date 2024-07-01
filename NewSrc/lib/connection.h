#ifndef CONNECTION_H 
#define CONNECTION_H 


#define START_MARKER 0x7E

/* Protocol type codes */
#define LIST 0x0A
#define DOWNLOAD 0X0B
#define DATA 0x12
#define END_FILE 0x9
#define ERROR 0x1F
#define OK 0xD
#define ACK 0x00
#define NACK 0x01
#define SHOW_IN_SCREEN 0x10
#define TIMEOUT 10 //seconds

// remover
#define PT_RESTORE_ONE_FILE 5
#define PT_BACKUP_ONE_FILE 6

#define MAX_PACKET_SIZE 72
#define MAX_DATA_SIZE 63
#define MAX_SEQUENCE 31
#define MAX_TYPE 15

/* Erros */
#define EXEC_ABORTATION -1
#define ACESS_DENIED 1
#define FILE_NOT_FOUND 2
#define DISK_FULL 3
#define INVALID_PARAMETER 4
#define CRC_ERROR 5

struct packet {
    uint8_t start_marker; // 1 byte
    uint8_t size; // 6 bits
    uint8_t sequence; // 6 bits -> 5 bits 
    uint8_t type; // 4 bits -> 5 bits
    //uint8_t parity; // 1 byte for parity 
    uint8_t crc8; // 8 bits (tam, seq, tipo, dados)
    uint8_t data[63]; // Max length of data is 63 bytes
};

int send_packet(struct packet *p, int socket);
int send_packet_and_wait_for_response(struct packet *packet, struct packet *response, int timeout, int socket);
int is_a_valid_packet(struct packet *p);
int listen_packet(struct packet *buffer, int timeout, int socket);
void destroy_packet(struct packet *p);
struct packet *create_or_modify_packet(struct packet *packet, uint8_t size, uint8_t sequence, uint8_t type, void *data); 

#endif