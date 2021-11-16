#include <stdio.h>
#include <stdlib.h>
#include "coconut.h"
#include <assert.h>
void test_syntax(char*string){
   int branches = 0; 
   struct CmdSequence** cmd_array = NULL; 
   printf("input string: [%s]\n",string); 
   cmd_array = syntax_parse(string,&branches); 
   printf("total branches %d\n",branches);
   for(int i = 0; i < branches; i++){
      if(cmd_array[i]){
         printf("cmd sequence[%i]\n",i);
         cmd_print(cmd_array[i]);
         cmd_destroy(&cmd_array[i]);
      }
   }
   if(cmd_array){
      free(cmd_array);
   }
}
/*void test_cmd_create(){
   struct CmdSequence* temp_cmd = NULL ; 
   temp_cmd = cmd_create_seq(0);
   assert(temp_cmd == NULL);
   temp_cmd = cmd_create_seq(1);
   //cmd_print(temp_cmd);
   cmd_destroy(&temp_cmd);
}*/
int main(void){
   //test_cmd_create(); 
   char test_arr[]  = "ls ; sl; al; ;prior empty\n"; 
   test_syntax(test_arr);
   
   return EXIT_SUCCESS; 
}
