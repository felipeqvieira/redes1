#include "define.h"

char *current_dir;

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

void print_commands() {
  printf("- ls : Lista os videos do servidor.\n");
  printf("- ls -e <extensao> : Listar os videos da extensao selecionada .mp4 ou .avi.\n");
  printf("- dl <arquivo> : Faz o download do arquivo selecionado.\n");
  printf("- dl -e <extensao> : Faz o download dos videos da extensao selecionada..\n");
  printf("- dl : Faz o download de todos os arquivos\n");
  printf("- help : Mostra os comando disponiveis!\n");
  printf("- exit : Sai da programa.\n");
}

int main(){

  int sockfd = openRSocket("enp1s0f1");

  char input[100];
  
  char *key;
  char file_name[MAX_NUM_FILES][MAX_FILE_NAME_SIZE];
  const char delimiter[] = " \n";

  current_dir = malloc(sizeof(int) * MAX_DATA_SIZE);
  get_current_directory(current_dir, MAX_DATA_SIZE);

  printf("Bem vindo ao Streaming de Videos!\n");
  printf("\nComandos disponíveis:\n");
  print_commands();

  printf("\n");

  while(1){
    
    printf("\nclient:%s$ ", current_dir);
    fgets(input, sizeof(input), stdin);

    key = strtok(input, delimiter);

    if(!key)
      continue;

    if(strcmp(key, "ls") == 0)
      list_files(current_dir);
    else if (strcmp(key, "dl") == 0)
      printf("oi\n");
      //transferência de arquivos
    else if(strcmp(key, "exit")){
      printf("Desligando...");
      return 0;
    } 
    else if (strcmp(key, "help"))
      print_commands();
    else 
      printf("Comando não encontrado: %s\n", key);
  }

  return 0;
}

