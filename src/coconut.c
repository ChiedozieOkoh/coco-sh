#include "coconut.h"
#include <errno.h>
#include <unistd.h>
#include <linux/limits.h>
#include <stdlib.h>
//static bool has_child = false; 
static char coco_cwd [PATH_MAX] = "\0";  

struct CmdSequence{
   char** cmd_block;
   unsigned int n_cmds;
}; 
//helper function to automate malloc checks for struct
static struct CmdSequence* cmd_create_seq(unsigned int n_cmd_blocks){
   struct CmdSequence* new_cmds; 
   new_cmds = malloc(sizeof(*new_cmds)); 
   if (new_cmds == NULL){
      return NULL; 
   }
   if (n_cmd_blocks == 0){
      free(new_cmds);
      return NULL; 
   }
   new_cmds->cmd_block = malloc(sizeof(char*) * n_cmd_blocks);
   if (new_cmds->cmd_block == NULL){
      free(new_cmds);
      return NULL; 
   }
   new_cmds->n_cmds = n_cmd_blocks; 
   return new_cmds; 
}

void cmd_destroy(struct CmdSequence* cmd){
   for(unsigned int i = 0; i < cmd->n_cmds; i++){
      free(cmd->cmd_block[i]);
   }
   free(cmd);
}

static unsigned int str_find_char_n(const char* str, char  c,unsigned int size){
   unsigned int count = 0; 
   unsigned int i = 0; 
   while (*str != '\0' && i < size){
      if(*str == c){
         count++; 
      }
      str++; 
      i++; 
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
   printf("[coco:info]cmd string given: %s\n",cmd_str);
   const char CNSEC_CHAR = CNSEC_EXEC_TOKEN; 
   //TODO consider making str_find_char_n return -1 for empty strings
   unsigned int cmd_count = str_find_char_n(cmd_str,CNSEC_EXEC_TOKEN,PATH_MAX) + 1;
   printf("%u commands\n",cmd_count); 
   printf("[coco:info] string after substr operation %s\n",cmd_str);
   struct CmdSequence* new_cmd =  cmd_create_seq(cmd_count);
   if(new_cmd == NULL){
      COCO_ERR("memory alloc failed\n");
      return NULL; 
   }
   char* tmp_token; 
   
   tmp_token = strtok(cmd_str,&CNSEC_CHAR); 
   unsigned int i =0; 
   
   while(tmp_token != NULL && i < cmd_count){
      printf("[coco:info] :: cmd identified: %s \n",tmp_token);

      new_cmd->cmd_block[i] = malloc(sizeof(char) * strlen(tmp_token)); 
      strncpy(new_cmd->cmd_block[i],tmp_token,strlen(tmp_token));

      tmp_token = strtok(NULL,&CNSEC_CHAR);
      i++; 
   }
   
   return new_cmd; 
}
