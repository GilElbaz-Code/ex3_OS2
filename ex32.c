#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/unistd.h>
#include <ctype.h>
#include <sys/wait.h>
#include <dirent.h>

void execute(char **argv) {
    pid_t pid;
    int status;
    int ret = -1;
    if ((pid = fork()) < 0) {     /* fork a child process           */
        printf("*** ERROR: forking child process failed\n");
        exit(1);
    } else if (pid == 0) {          /* for the child process:         */
        ret = execvp(*argv, argv);
        if (ret < 0) {     /* execute the command  */
            printf("*** ERROR: exec failed\n");
            exit(1);
        }
    } else {
        printf("%d\n", ret);/* for the parent:      */
        while (wait(&status) != pid)       /* wait for completion  */;
    }
}

size_t file_size_stat(const char *path) {
    struct stat stat_buf;
    // in case of stat failure
    if (lstat(path, &stat_buf) == -1) {
        perror("Error in: stat\n");
        return -1;
    }
    return stat_buf.st_size;
}

int main(int argc, char **argv) {
    int fd;
    int ret_val = 0;
    size_t length1 = file_size_stat(argv[1]);
    char *buf = (char *) malloc(sizeof(char) * length1);
    char *token;
    DIR *main_directory;
    struct dirent *subdir;
    char **args = (char **) malloc(sizeof(char *) * 3);
    int i = 0;
    size_t temp_len = 0;
    memset(buf, 0, length1);
    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        return -1;
    }
    if (read(fd, buf, file_size_stat(argv[1])) == -1) {
        return -1;
    }
    close(fd);

    token = strtok(buf, "\n");

    /* walk through other tokens */
    while (token != NULL) {
        temp_len = strlen(token);
        args[i] = (char *) malloc(sizeof(char) * temp_len);
        strncpy(args[i++], token, temp_len);
        token = strtok(NULL, "\n");
    }
    //char *args[] = {"gcc", "-o", "comp.out", "ex31.c", NULL}; //compile ex31
    //execvp("gcc", args);
    //char* run[] = {"./comp.exe",file_path_with_txt, correct_output_path,NULL}; //run ex31 to compare
    //int result = execve(run); //get result from output and correct output
    //return result;
    main_directory = opendir(args[0]);
    if (main_directory == NULL) {
        ret_val = -1;
        goto cleanup;
    }
    while ((subdir = readdir(main_directory)) != NULL) {
        if (subdir->d_type == DT_DIR && strcmp(subdir->d_name, ".") != 0 && strcmp(subdir->d_name, "..") != 0)
            printf("%s\n", subdir->d_name);
    }
    // Enter sub-folders and look for c files - 

    cleanup:
    closedir(main_directory);
    free(buf);
    for (i = 0; i < 3; i++) {
        free(args[i]);
    }
    free(args);
    return ret_val;
}
