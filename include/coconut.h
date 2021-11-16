#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define COCO_PERROR() perror("[coco:err]") 
#define COCO_PERROR_MSG(msg) perror("[coco:err]:: "msg)
#define COCO_ERR(msg) fprintf(stderr,"[coco:err]:: "msg)
#define COCO_INFO(msg) printf("[coco:info]:: " msg)
#define COND_EXEC_TOKEN "&&"
#define CNSEC_EXEC_TOKEN ";"
#define ASYNC_EXEC_TOKEN "&"
#define DETACH_EXEC_TOKEN "&>"
#define ARG_TOKEN " "
#define PIPE_EXEC_TOKEN "|"
#define COCO_PATH "/usr/bin/"
#define COCO_PATH_LEN 10
#define ESC_RED "\x1b[1;31m"
#define ESC_BLUE "\x1b[1;34m"
#define ESC_YELLOW "\x1b[1;33m"
#define ESC_GREEN "\x1b[1;32m"
#define ESC_MAGENTA "\x1b[1;35m"
#define ESC_RESET "\x1b[1;0m"
#define MAX_CMD_BRANCHES 100
struct CmdSequence; 
typedef int(*cmd_handle)(const char**); 

void print_prompt(void); 
bool coco_set_cwd(const char*); 

struct CmdSequence* cmd_parse(const char* const);//split a string  into a CmdSequence object
void cmd_destroy(struct CmdSequence**); 

void cmd_print(struct CmdSequence*);
void cmd_execute(struct CmdSequence*);//execute a cmd with arguements

struct CmdSequence**  syntax_parse(const char* const,int*); // split command chain into seperate commands 
//e.g syntax_parse("ls /home/ ; ls /home/joeblogs",(int*)foo); 
//will effectively return "ls /home/" , "ls /home/joeblogs" 
//the number of Cmd Sequences found will be written to the int* parameter
// a pointer to the beginning of the array of CmdSequences is returned
