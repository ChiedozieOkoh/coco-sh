#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include "coconut.h"


int main(int argc , char** argv){
   if(argc > 1){ //because coconut will always be passed as an arguement
      // pass working directory to coconut shell
      if (!coco_set_cwd(argv[1])){
         exit(EXIT_FAILURE);
      } 
   }else{
      // we havent been given a path directly
      char tmp_cwd [PATH_MAX] = "\0";  
      if(getcwd(tmp_cwd,PATH_MAX)){ 
         coco_set_cwd(tmp_cwd);
      }else{
         COCO_PERROR(); 
         exit(EXIT_FAILURE);
      }
   }
   print_prompt(); 
   char user_input [PATH_MAX] = "\0"; 
   while(fgets(user_input,PATH_MAX,stdin) != NULL){
      if(coco_str_compare(user_input,"exit",4)){
         break; 
      }
      //printf("ehco'd: %s",user_input);
      struct CmdSequence* cmd = NULL; 
      cmd = cmd_parse(user_input);
      if(cmd){
         //cmd_print(cmd);
         cmd_execute(cmd);
      }
      print_prompt(); 
      if(cmd){
         cmd_destroy(&cmd);
      }
   } 
   printf("\n");
   printf("exiting...\n");
   return EXIT_SUCCESS; 
}
