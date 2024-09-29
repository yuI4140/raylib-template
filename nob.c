#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NOB_IMPLEMENTATION
#include "./headers/nob.h"

#define SRC_NAME "main"
#define SRC_EXE "./build/" SRC_NAME
#define DEBUG_FLAGS  "-g","-Wall","-Wextra","-Wpedantic"
#define RELEASE_FLAGS "-O3","-Oz","-s"
#define LIB_FLAGS "-lm","-L./lib/","-l:libraylib.a"  


// building
#define CC "gcc"
#define BUILD_TYPE "shared"
#define THREADS "12"
#define BUILD_DIR "./build"
#define SRC "./src/" SRC_NAME ".c"
#define BIN BUILD_DIR "./" SRC_NAME
#define RAYLIB_DIR "lib/raylib-master"
#define RLIB "libraylib.a"
#define SRC_RLIB "./lib/libraylib.a"
#define RAYLIB_LIB "./lib/raylib-master/build/raylib/" RLIB
#define RAYLIB_ZIP "lib/raylib.zip"

void run_command(const char *command) {
    nob_log(NOB_INFO,"%s",command);
    int ret = system(command);
    if (ret != 0) {
        fprintf(stderr, "Command failed: %s\n", command);
        exit(EXIT_FAILURE);
    }
}

enum BuildType{
    BShared=0,
    BStatic
};
void setup_raylib(enum BuildType type) {
    nob_log(NOB_INFO,"Setting up raylib...\n");
    if (!nob_file_exists(SRC_RLIB)) {
        nob_log(NOB_WARNING,"Raylib library not found. Building...\n");
        run_command("mkdir -p ./lib ./build/");
        if (!nob_file_exists(RAYLIB_ZIP)) {
            run_command("curl -L https://github.com/raysan5/raylib/archive/refs/heads/master.zip -o " RAYLIB_ZIP);
        }
        run_command("unzip -o " RAYLIB_ZIP " -d lib");
        run_command("mkdir -p " RAYLIB_DIR "/build");
        if (type==BStatic) {
           run_command("cd " RAYLIB_DIR "/build && cmake .. && make -j" THREADS);
        }else {
           run_command("cd " RAYLIB_DIR "/build && cmake .. && make RAYLIB_LIBTYPE=SHARED -j" THREADS);
        }
        run_command("mv  " RAYLIB_LIB " ./build/" RLIB);
        run_command("rm -vrf ./lib/*");
        run_command("mv ./build/" RLIB " ./lib/" RLIB);
    }else {
        nob_log(NOB_INFO,"Raylib library was found.\n");
    }
}
enum Flags{
    FRelease,
    FDebug
};
int compile(enum Flags flag) {
    if (!nob_mkdir_if_not_exists("./build")) { return -1; }
    printf("Compiling %s...\n", SRC_NAME);
    Nob_Cmd cmd={0};;;
    nob_cmd_append(&cmd,CC);
    if (flag==FRelease) {
        nob_cmd_append(&cmd,RELEASE_FLAGS);
    }else if (flag==FDebug) {
        nob_cmd_append(&cmd,DEBUG_FLAGS);
    }else {
        nob_log(NOB_ERROR,"unknown compile flag.");
    }
    nob_cmd_append(&cmd,"-o",SRC_EXE,SRC);
    nob_cmd_append(&cmd,LIB_FLAGS);
    return nob_cmd_run_sync(cmd);
}

int run_program(enum Flags flag) {
    printf("Running %s...\n", SRC_EXE);
    if (!nob_file_exists(SRC_EXE)) {
       if(!compile(flag)){ return -1;}; 
    }else {
        run_command(SRC_EXE);
    }
    return 0;
}
enum CleanKind{
    CSuper=-7,
    CDefault=0,
    CAll=1,
    CNob,
    CSetup
};
void clean(enum CleanKind ckind) {
    if (ckind==CSuper) {
        run_command("rm -rf " "./nob" " " "./lib/" " "  BUILD_DIR);
        return;
    }else if (ckind==CNob) {
        goto cnob; 
    }else {
        nob_log(NOB_INFO,"Cleaning up:");
    }
    if (ckind==CAll) {
        
        nob_log(NOB_INFO,"all...\n");
        run_command("rm -rf " "./lib/" RLIB " " BUILD_DIR);
    }else if (ckind==CSetup) {
        nob_log(NOB_INFO,"setup...\n");
        run_command("rm -rf " RAYLIB_DIR " " RAYLIB_ZIP);
    }else if (ckind==CNob) {
cnob:
        if (nob_file_exists("./nob.old")) {
            run_command("rm -rf " "./nob.old");
        }
    }else {
        run_command("rm -rf " BUILD_DIR);
    }
}
bool is_flag(const char *flag){
   return flag[0]=='-'; 
}
bool is_simple_flag(const char *flag){
   return is_flag(flag)&&flag[2]==0; 
}
int main(int argc, char *argv[]) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    bool args_on=false;
    if (argc>1) {
      args_on=true; 
    }
    if (args_on) {
      if ((strcmp(argv[1],"-super"))==0) {
      clean(CSuper);
      return 0;
      }
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
    if (args_on) {
     for (size_t i=0;i<argc;++i) { 
         const char *flag=argv[i];
         if (is_simple_flag(flag)) {
             char sf=flag[1];
             if (sf=='R') {
                compile(FRelease);
             }else if (sf=='r') {
                run_program(FDebug);
             }else if (sf=='C') {
                clean(CAll);
             }else if (sf=='c') {
                clean(CDefault);
             }else {
                nob_log(NOB_WARNING,"unknown flag:%s",sf);
                nob_log(NOB_INFO,"flags:");
                nob_log(NOB_INFO,"R:\ncompile with release");
                nob_log(NOB_INFO,"r:\nrun and if program doesn't exist it compiles with debug flags\n with a simple flag");
                nob_log(NOB_INFO,"C:\nclean: all");
                nob_log(NOB_INFO,"c:\nclean: default");
                return -1;
             }
         }
     }
    }else {
        if(!compile(FDebug)){
            nob_log(NOB_ERROR,"compiling fail.");
            return -1;
        };
    }
    return EXIT_SUCCESS;
}
