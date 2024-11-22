#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define NOB_IMPLEMENTATION
#include "./headers/nob.h"

#define SRC_NAME "main"
#define SRC_EXE "./build/" SRC_NAME
#define DEBUG_FLAGS "-g", "-Wall", "-Wextra", "-Wpedantic"
#define RELEASE_FLAGS "-O3", "-Oz", "-s"
#define LIB_FLAGS "-lm", "-L./lib/", "-l:libraylib.a"

#define CC "gcc"
#define BUILD_TYPE "shared"
#define THREADS "12"
#define BUILD_DIR "./build"
#define SRC "./src/" SRC_NAME ".c"
#define BIN BUILD_DIR "/" SRC_NAME
#define RAYLIB_DIR "lib/raylib-master"
#define RLIB "libraylib.a"
#define SRC_RLIB "./lib/libraylib.a"
#define RAYLIB_LIB "./lib/raylib-master/build/raylib/" RLIB
#define RAYLIB_ZIP "lib/raylib.zip"

enum BuildType { BShared = 0, BStatic };
enum Flags { FRelease, FDebug };
enum CleanKind { CSuper = -7, CDefault = 0, CAll = 1, CNob, CSetup };

bool bundle_on = false;


void run_command(const char *command) {
    nob_log(NOB_INFO, "%s", command);
    if (system(command) != 0) {
        fprintf(stderr, "Command failed: %s\n", command);
        exit(EXIT_FAILURE);
    }
}

void setup_raylib(enum BuildType type) {
    nob_log(NOB_INFO, "Setting up raylib...");
    if (!nob_file_exists(SRC_RLIB)) {
        nob_log(NOB_WARNING, "Raylib library not found. Building...");
        run_command("mkdir -p ./lib ./build/");
        if (!nob_file_exists(RAYLIB_ZIP)) {
            run_command("curl -L https://github.com/raysan5/raylib/archive/refs/heads/master.zip -o " RAYLIB_ZIP);
        }
        run_command("unzip -o " RAYLIB_ZIP " -d lib");
        run_command("mkdir -p " RAYLIB_DIR "/build");
        char build_command[512];
        snprintf(build_command, sizeof(build_command),
                 "cd %s/build && cmake .. && make %s -j%s",
                 RAYLIB_DIR, (type == BStatic) ? "" : "RAYLIB_LIBTYPE=SHARED", THREADS);
        run_command(build_command);
        run_command("mv " RAYLIB_LIB " ./build/" RLIB);
        run_command("rm -rf ./lib/*");
        run_command("mv ./build/" RLIB " ./lib/" RLIB);
    } else {
        nob_log(NOB_INFO, "Raylib library found.");
    }
}
void clean(enum CleanKind ckind) {
    switch (ckind) {
        case CSuper:
            run_command("rm -rf ./nob ./lib/ " BUILD_DIR);
            break;
        case CAll:
            run_command("rm -rf ./lib/" RLIB " " BUILD_DIR);
            break;
        case CSetup:
            run_command("rm -rf " RAYLIB_DIR " " RAYLIB_ZIP);
            break;
        case CNob:
            if (nob_file_exists("./nob.old")) {
                run_command("rm -rf ./nob.old");
            }
            break;
        case CDefault:
        default:
            run_command("rm -rf " BUILD_DIR);
            break;
    }
}
void write_c_code_to_file(const char *filename, const char *c_code) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        return;
    }
    while (*c_code) {
        if (*c_code == '\\' && *(c_code + 1) == 'n') {
            fputc('\n', file);
            c_code += 2;
        } else {
            fputc(*c_code, file);
            c_code++;
        }
    }
    fclose(file);
}
int compile(enum Flags flag) {
    if (!nob_mkdir_if_not_exists("./build")) return -1;
        nob_log(NOB_INFO, "Compiling...");
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, CC);
    if (flag == FRelease) {
        nob_cmd_append(&cmd, RELEASE_FLAGS);
    } else {
        nob_cmd_append(&cmd, DEBUG_FLAGS);
    }
    nob_cmd_append(&cmd, "-o", SRC_EXE, SRC);
    nob_cmd_append(&cmd, LIB_FLAGS);
    return nob_cmd_run_sync(cmd);
}
int run_program(enum Flags flag) {
    nob_log(NOB_INFO, "Running program...");
    if (!nob_file_exists(SRC_EXE)) {
        if (compile(flag) != 0) return -1;
    }
    run_command(SRC_EXE);
    return 0;
}

typedef struct {
    const char *short_flag;
    const char *long_flag;
    const char *description;
    void (*action)(void);
} Flag;


void setup_bundle(void){
 if (bundle_on) {
    Nob_Cmd bundle={0};;;
    Nob_Cmd exe_bundle={0};
    nob_cmd_append(&bundle,CC);
    nob_cmd_append(&bundle,"-o","./build/bundle");
    nob_cmd_append(&bundle,"./src/bundle.c");
    nob_cmd_run_sync(bundle); 
    nob_cmd_append(&exe_bundle,"./build/bundle");
    nob_cmd_run_sync(exe_bundle); 
 }
}
void compile_debug(void) {
    setup_bundle();
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, CC, "-g", "-Wall", "-Wextra", "-Wpedantic", "-o", SRC_EXE, SRC, LIB_FLAGS);
    if (!nob_cmd_run_sync(cmd)) {
        nob_log(NOB_ERROR, "Compilation failed.");
        exit(EXIT_FAILURE);
    }
}
void compile_release(void) {
    setup_bundle();
    Nob_Cmd cmd = {0};
    nob_cmd_append(&cmd, CC, "-O3", "-Oz", "-s", "-o", SRC_EXE, SRC, LIB_FLAGS);
    if (!nob_cmd_run_sync(cmd)) {
        nob_log(NOB_ERROR, "Compilation failed.");
        exit(EXIT_FAILURE);
    }
}
void setup_build_dir(void) {
    if (system("mkdir -p ./build") != 0) {
        nob_log(NOB_ERROR, "Failed to create build directory.");
        exit(EXIT_FAILURE);
    }
    nob_log(NOB_INFO, "Build directory created.");
}
void clean_default(void) {
    run_command("rm -rf ./build");
}
void clean_all(void) {
    run_command("rm -rf ./lib/libraylib.a ./build");
}
void enable_bundle(void) {
    bundle_on = true;
    nob_log(NOB_INFO, "Bundle mode enabled.");
}

void print_help(void);
Flag flags[] = {
    {"R", "release", "Compile with release flags.", compile_release},
    {"r", "run", "Run the program (compiles if missing).", compile_debug},
    {"C", "clean-all", "Clean all build artifacts.", clean_all},
    {"c", "clean", "Clean default build directory.", clean_default},
    {"b", "bundle-on", "Enable bundle mode.", enable_bundle},
    {"h", "help", "Display help message.", print_help},
};
void print_help(void) {
    nob_log(NOB_INFO, "Available Flags:");
    for (size_t i = 0; i < sizeof(flags) / sizeof(Flag); ++i) {
        printf("  -%s, --%s: %s\n", flags[i].short_flag, flags[i].long_flag, flags[i].description);
    }
}
int main(int argc, char *argv[]) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    if (argc < 2) {
        print_help();
        return EXIT_FAILURE;
    }
    if ((strcmp(argv[1],"-super"))==0) {
      clean(CSuper);
      return 0;
    }
    if (!nob_file_exists("./lib/" RLIB)) {
        if ((strcmp(BUILD_TYPE,"static"))==0) {
            setup_raylib(BStatic);
        }else if((strcmp(BUILD_TYPE,"shared"))==0){
            setup_raylib(BShared);
        }
        clean(CSetup);
    }
    clean(CNob);
    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        bool matched = false;
        for (size_t j = 0; j < sizeof(flags) / sizeof(Flag); ++j) {
            if ((arg[0] == '-' && strcmp(arg + 1, flags[j].short_flag) == 0) ||
                (strncmp(arg, "--", 2) == 0 && strcmp(arg + 2, flags[j].long_flag) == 0)) {
                flags[j].action();
                matched = true;
                break;
            }
        }
        if (!matched) {
            fprintf(stderr, "Unknown flag: %s\n", arg);
            print_help();
            return EXIT_FAILURE;
        }
    }
    
    return EXIT_SUCCESS;
}
