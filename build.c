#include <stdbool.h>
#include <stdlib.h>
#define PKG_ENABLE 0
#define NOB_IMPLEMENTATION
#include "./headers/nob.h"
char* run_pkg_config(const char* package, const char* flags) {
    FILE *fp;
    char command[1024];
    char path[4096] ={0};
    char* result = NULL;
    size_t len = 0;
    snprintf(command, sizeof(command), "pkg-config %s %s", flags, package);
    fp = popen(command, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to run command\n");
        return NULL;
    }
    while (fgets(path, sizeof(path), fp) != NULL) {
        len += strlen(path);
        result = realloc(result, len + 1);
        if (result == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            pclose(fp);
            return NULL;
        }
        strcat(result, path);
    }
    pclose(fp);
    return result;
}
char* remove_spaces(const char* input) {
    size_t len = strlen(input);
    char* result = malloc(len + 1); // Allocate memory for the result
    if (result == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        if (input[i] != ' ') {
            result[j++] = input[i];
        }
    }
    result[j] = '\0';
    return result;
}
int main(int argc, char *argv[]) {
  Nob_Cmd cmd = {0};
  NOB_GO_REBUILD_URSELF(argc, argv);
  if (!nob_mkdir_if_not_exists("./bin")) return -1;
  nob_cmd_append(&cmd, "gcc");
  nob_cmd_append(&cmd,"-Wall", "-Wextra","-Wpedantic");
  nob_cmd_append(&cmd,"-I./headers");
#ifdef _WIN32
  nob_cmd_append(&cmd, "-L./lib");
  nob_cmd_append(&cmd, "-o", "./bin/main", "./src/main.c");
  nob_cmd_append(&cmd,"-lopengl32", "-lgdi32", "-lwinmm");
#endif /* ifdef _WIN32 */
#if PKG_ENABLE==1 
//TODO: implement PKG_ENABLE
#endif /* if PKG_ENABLE==1 */
#ifdef _POSIX_C_SOURCE
nob_cmd_append(&cmd,"-L./lib/lib_linux");
nob_cmd_append(&cmd, "-o", "./bin/main", "./src/main.c");
nob_cmd_append(&cmd,"-lm");
#endif /* ifdef _POSIX_C_SOURCE */
  nob_cmd_append(&cmd,"-lraylib");
  nob_cmd_run_sync(cmd);
  return EXIT_SUCCESS;
}
