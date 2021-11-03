#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define COCO_PERROR() perror("[coco:err]") 
#define COCO_ERR(msg) fprintf(stderr,"[coco:err]:: "msg)
#define COND_EXEC_TOKEN "&&"
#define CNSEC_EXEC_TOKEN ';'
#define ASYNC_EXEC_TOKEN "&"
#define DETACH_EXEC_TOKEN "&>"
#define ARG_TOKEN " "
struct CmdSequence; 
typedef int(*cmd_handle)(const char**); 

void print_prompt(void); 
bool coco_set_cwd(const char*); 

struct CmdSequence* cmd_parse(char*);//split a string of commands into parts
void cmd_destroy(struct CmdSequence*); 

void cmd_print(struct CmdSequence*);
void cmd_execute(struct CmdSequence*);//execute a cmd with arguements
