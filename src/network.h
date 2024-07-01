// Libera a memória alocada para a mensagem
void free_packet(protocolo *msg); 

// Retorna o próximo número de sequência (Evita overflow)
uint8_t prox_seq(protocolo *msg); 

// Recebe a mensagem
protocolo *recebe_msg(int socket, int n_msgs);

// Aloca a memória para a mensagem
protocolo *aloca_msg(); 
//protocolo *cria_msg(uint8_t seq, uint8_t tipo, uint8_t *dados, ...); // Cria a mensagem

// Envia a mensagem para o socket
void envia_msg(protocolo *msg, int socket); 

// Lê o tipo da mensagem
uint8_t ler_msg(protocolo *msg); 

// Imprime a mensagem
void imprime_msg(protocolo *msg);

// Mostas os possíveis erros de socket
void socket_errors(short errno, interface ifc);

// Faz o cálculo do CRC8
uint8_t CRC8_calc( uint8_t data, uint8_t size);

// informações da mensagem
void log_message(const char* message); 


int verify_packet_parameters(uint8_t size, uint8_t sequence, uint8_t type); //verifica parâmetros do pacote

char* arrayToString(uint8_t* array, size_t length);//converte array to string

protocolo *cria_msg(protocolo *packet, uint8_t size, uint8_t sequence, uint8_t type, void *data);//cria pacote

int send_packet_and_wait_for_response(struct protocolo *packet, struct protocolo *response, int timeout, int socket);//para e espera

long long int get_file_size(char *path);//tamanho do arquivo

double time_passed(clock_t start, clock_t end);//calcula quanto tempo passou

int listen_packet(protocolo *buffer, int timeout, int socket); // recebe pacotes e verifica

char *get_file_path(char *log_file, char *file_name);// pega caminho do arquivo

int conv_comando(char *comando, char *parametro);//quebra a linha e verifica comando

void timeout(int socket,protocolo *msg); // timeout