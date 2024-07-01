#ifndef COMMAND_H
#define COMMAND_H

#define MAX_FILE_NAME_SIZE 63 
#define C_LIST 0
#define C_DOWNLOAD 1
#define HELP 2
#define EXIT 3


int list_video_files_in_directory(char *directory, int socket);
int send_single_file(char *file_name, int socket);
void restore_single_file(char *file_name, int socket);
int receive_file(char *file_path,  int socket);

#endif
