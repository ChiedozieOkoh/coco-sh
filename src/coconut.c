#include "coconut.h"
#include <errno.h>
#include <unistd.h>
#include <linux/limits.h>
#include <stdlib.h>
#include <ctype.h>
//static bool has_child = false; 
static char coco_cwd [PATH_MAX] = "\0";  

struct CmdSequence{
   char** cmd_block;
   unsigned int n_cmds;
}; 

void cmd_print(struct CmdSequence* cmd){
   for(unsigned int i = 0; i < cmd->n_cmds; i++){
      printf("cmd[%u] :: <<%s>>\n",i,cmd->cmd_block[i]);
   }
}

//helper function to automate malloc checks for struct
static struct CmdSequence* cmd_create_seq(unsigned int n_cmd_blocks){
   struct CmdSequence* new_cmds; 
   new_cmds = malloc(sizeof(*new_cmds)); 
   if (new_cmds == NULL){
      return NULL; 
   }
   if (n_cmd_blocks == 0){
      free(new_cmds);
      printf("cmd seq malloc failed 0 arguements expected\n");
      return NULL; 
   }
   new_cmds->cmd_block = malloc(sizeof(char*) * (n_cmd_blocks + 1));
   if (new_cmds->cmd_block == NULL){
      free(new_cmds);
      return NULL; 
   }
   new_cmds->n_cmds = n_cmd_blocks + 1; 
   new_cmds->cmd_block[n_cmd_blocks] = NULL;// execv requires the last pointer to be null 
   return new_cmds; 
}

void cmd_destroy(struct CmdSequence* cmd){
   for(unsigned int i = 0; i < cmd->n_cmds; i++){
      free(cmd->cmd_block[i]);
   }
   free(cmd);
}
static unsigned int str_n_arg(const char* str){
   
   char test_0 = '\0'; 
   sscanf(str," %c",&test_0); 
   if(test_0 == '\0' ){
      printf("detected empty input\n");
      return 0; 
   }

   unsigned int count = 0; 
   bool onWord = false; 
   while(*str != '\0'){
      if(!isspace(*str) && (onWord ==false)){
         count++; 
         onWord = true; 
      }
      if(isspace(*str)){
         onWord = false; 
      }
      str++; 
   }
   return count;  
}
bool coco_set_cwd(const char* src_dir){
   if(src_dir == NULL){
      fprintf(stderr,"directory given was null\n");
      return false; 
   }
   int error = chdir(src_dir); 
   if(error){
      COCO_PERROR(); 
      return false; 
   }


   strncpy(coco_cwd,src_dir,PATH_MAX -1);
   coco_cwd[PATH_MAX -1] = '\0'; 
   return true; 
}
void print_prompt(void){
   printf("coco[%s]>>",coco_cwd);
}
struct CmdSequence* cmd_parse(char* cmd_str){
   //TODO use valgrind to look at memoroy leak int this function
   unsigned int arg_count = 0; 
   // ARG_TOKEN is defined in coconut.h
   const char ARG_CHARS [] = ARG_TOKEN; 
   printf("[coco:info]cmd string given: %s\n",cmd_str);
   arg_count = str_n_arg(cmd_str);
   printf("[coco:info] %u arguement/s given\n",arg_count);
   
   printf("[coco:info] cmd string after arg count opr: %s\n",cmd_str);
   
   struct CmdSequence* new_cmd = cmd_create_seq(arg_count);
   
   
   if(new_cmd == NULL){
      COCO_ERR("memory alloc failed\n");
      return NULL; 
   }
   // split string by ARG_CHARS and store the splits inside the cmd sequence obj
   char* tmp_token = NULL;  
   unsigned int i = 0; 
   tmp_token = strtok(cmd_str,ARG_CHARS); 
   while(tmp_token != NULL && i < arg_count){
      printf("[coco:info] :: cmd identified: <<%s>> \n",tmp_token);

      //(strlen(tmp_token) + 1) includes space for null terminator
      new_cmd->cmd_block[i] = malloc(sizeof(char) * (strlen(tmp_token)+1)); 
      strncpy(new_cmd->cmd_block[i],tmp_token,strlen(tmp_token) + 1);

      tmp_token = strtok(NULL,ARG_CHARS);
      i++; 
   }
   return new_cmd; 
}
/*
void cmd_execute(struct CmdSequence* cmd){
   

}*/
