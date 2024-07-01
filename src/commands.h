int receive_file(char *file_name, int socket); //recebe arquivo

void get_current_directory(char *buffer, size_t size); // pega diretório corrente

void change_directory(char *key, const char delimiter[]); // muda de diretório

void process_command(char file_names[][MAX_FILE_NAME_SIZE], char *key, const char delimiter[], int type_flag, int sockfd); // identifica comandos

void list_files(const char* directory); // lista os arquivos locais

void list_files_remote(int socket); // lista os arquivos do server 

void print_commands(); // mostra comandos

int send_single_file(char *file_name, int socket); // envia um único arquivo

void download(char *videoName); // Baixar arquivos do Servidor para o Cliente

void list(char *dir); // Lista arquivos