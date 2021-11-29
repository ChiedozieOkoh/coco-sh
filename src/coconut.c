#include "coconut.h"
#include <errno.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/wait.h>
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
      //printf("cmd seq malloc failed 0 arguements expected\n");
      return NULL; 
   }
   new_cmds->cmd_block = NULL; 
   new_cmds->cmd_block = malloc((n_cmd_blocks + 1) * sizeof(char*));
   if (new_cmds->cmd_block == NULL){
      free(new_cmds);
      return NULL; 
   }
   new_cmds->n_cmds = n_cmd_blocks + 1; 
   /*for(unsigned int i = 1 ; i < new_cmds->n_cmds; i++){
      new_cmds->cmd_block[i] = NULL; 
   }*/
   new_cmds->cmd_block[n_cmd_blocks] = NULL;// execv requires the last pointer to be null 
   return new_cmds; 
}

void cmd_destroy(struct CmdSequence** cmd){
   for(unsigned int i = 0; i < (*cmd)->n_cmds; i++){
      free((*cmd)->cmd_block[i]);
      (*cmd)->cmd_block[i] = NULL; 
   }
   free(*cmd);
   *cmd = NULL; 
}
bool is_exit(const char* input){
   while(*input == ' '){
      input++; 
   }
   if(strncmp(input,"exit",4) !=0){
      return false; 
   }
   input += 4;
   if(isalnum(*input)){
      return false; 
   }
   return true; 
}
static unsigned int str_n_arg(const char* str){
   
   char test_0 = '\0'; 
   sscanf(str," %c",&test_0); 
   if(test_0 == '\0' ){
      //printf("detected empty input\n");
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
   //printf(ESC_BLUE"coco[%s]>>"ESC_RESET,coco_cwd);
   printf(ESC_BLUE"coco["ESC_RESET); 
   printf("%s",coco_cwd); 
   printf(ESC_BLUE"]>>"ESC_RESET);
}
struct CmdSequence* cmd_parse(const char* const  cmd_str){
   //TODO handle expanding '~' '.' and '..' symbols 
   unsigned int arg_count = 0; 
   // ARG_TOKEN is defined in coconut.h
   const char ARG_CHARS [] = ARG_TOKEN; 
   //printf("[coco:info]cmd string given: %s\n",cmd_str);
   char temp_cmd_str [PATH_MAX] = "\0"; 
   memcpy(temp_cmd_str,cmd_str,strlen(cmd_str));
   arg_count = str_n_arg(temp_cmd_str);
   
   //printf("[coco:info] %u arguement/s given\n",arg_count);
   
   //printf("[coco:info] cmd string after arg count opr: %s\n",cmd_str);
   
   struct CmdSequence* new_cmd = cmd_create_seq(arg_count);
   
   
   if(new_cmd == NULL){
      //COCO_ERR("memory alloc failed\n");
      return NULL; 
   }
   // split string by ARG_CHARS and store the splits inside the cmd sequence obj
   char* tmp_token = NULL;  
   unsigned int i = 0; 
   tmp_token = strtok(temp_cmd_str,ARG_CHARS); 
   while(tmp_token != NULL && i < arg_count){
      //printf("[coco:info] :: cmd identified: <<%s>> \n",tmp_token);
      //(strlen(tmp_token) + 1) includes space for null terminator
      new_cmd->cmd_block[i] = malloc(sizeof(char) * (strlen(tmp_token)+1)); 
      strncpy(new_cmd->cmd_block[i],tmp_token,strlen(tmp_token) + 1);
      tmp_token = strtok(NULL,ARG_CHARS);
      i++; 
   }
   // replace trailing \n with \0
   //n_cmds -1 would point to the last pointer 
   // the last pointer is always NULL so that the array is NULL terminated
   // thus the n_cmds -2
   char* cmd_alias = new_cmd->cmd_block[new_cmd->n_cmds -2]; 
   if(cmd_alias[strlen(cmd_alias) -1] == '\n'){
      cmd_alias[strlen(cmd_alias) -1] = '\0'; 
   }

   return new_cmd; 
}
void cmd_execute(struct CmdSequence* cmd){
   if(cmd == NULL){
      return; 
   }
   char final_cmd_path [256] = COCO_PATH; 
   strncat(final_cmd_path,cmd->cmd_block[0],30);
   // only the parent process(the shell) should change directory 
   if(strncmp(cmd->cmd_block[0],"cd",2) == 0){
      if(cmd->cmd_block[1] == NULL){
         coco_set_cwd("/home"); 
         return; 
      }
      if(cmd->cmd_block[2] != NULL){
         COCO_ERR("cd : too many arguements\n");
         return; 
      }
      coco_set_cwd(cmd->cmd_block[1]);
      return; 
   } 
   pid_t pid = fork(); 
   if(pid < 0){
      COCO_PERROR();
      return;
   }
   if(pid == 0){
      // run cmd in child process
         //printf("child process : %i\n",pid);
         int result = execv(&final_cmd_path[0],cmd->cmd_block); 
         if(result < 1){
            //COCO_PERROR_MSG("Child process error");
            COCO_ERR("unknown command\n");
            cmd_destroy(&cmd);
            exit(EXIT_FAILURE);
         }else{
            cmd_destroy(&cmd);
            exit(EXIT_SUCCESS); 
         }
   }
   //block parent until child status changes 
   if(pid > 0){
      int status; 
         //printf("parent process : %i\n",pid);
      pid_t wait_status; 
      wait_status = waitpid(pid,&status,0);
      if(wait_status == -1){
         perror("wait pid status"); 
      }
      if(WIFEXITED(status)){
         //printf("child exited normally status code:%d\n",WEXITSTATUS(status));
      }
   }
}
struct CmdSequence** syntax_parse(const char* string,int* cmd_number){
   char* cmd_branch = NULL; 
   //char* CNSEC_TOKEN = CNSEC_EXEC_TOKEN; 
   int count = 0;  
   char temp_cmd_string [PATH_MAX] = "\0"; 
   char* temp_cmd_store[MAX_CMD_BRANCHES] = {0}; 
   struct CmdSequence** cmd_array = NULL;  
   memcpy(temp_cmd_string,string,strlen(string) ); 
   cmd_branch = strtok(temp_cmd_string,CNSEC_EXEC_TOKEN); 
   while(cmd_branch != NULL && count < MAX_CMD_BRANCHES){
      printf("cmd branch --[%s]--\n",cmd_branch);
      if(str_n_arg(cmd_branch)){
         temp_cmd_store[count] = cmd_branch;
         count++; 
      }
      cmd_branch = strtok(NULL,CNSEC_EXEC_TOKEN); 
   } 
   if (count == 0 ){
      return NULL; 
   }

   printf(ESC_GREEN"parsed cmd sequences ..."ESC_RESET"\n");
   cmd_array = malloc(sizeof(struct CmdSequnce*) * count); 
   for(int i = 0; i < count ; i++){
      //struct CmdSequence* temp_cmd = NULL; 
      cmd_array[i] = cmd_parse(temp_cmd_store[i]);
      /*if(cmd_array[i]){
         cmd_print(cmd_array[i]);
      }
      cmd_destroy(&cmd_array[i]);
      printf("\n");*/
   }
   //free(cmd_array);
   *cmd_number = count; 
   return cmd_array; 
}
