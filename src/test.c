#include <stdio.h>
#include <stdlib.h>
#include "coconut.h"
#include <assert.h>

void test_async_exec(void){
   setlinebuf(stdout); 
   setlinebuf(stdin);
   struct sigaction sa; 
   init_handlers(&sa);
   char test_str[] = "ping www.google.com -c 10 &";
   char user_input[64] = "\0"; 
   struct CmdSequence* cmd = NULL; 
   cmd = cmd_parse(test_str);
   //cmd_print(cmd);
   cmd_exec_async(cmd);
   //print_prompt(); 
   while(fgets(user_input,64,stdin) != NULL){
      if(coco_str_compare(user_input,"exit",4)){
         printf("%s",user_input);
         break; 
      }
      print_prompt(); 
   }
   if(cmd){
      cmd_destroy(&cmd);
   }
}
void test_str_compare(void){
   bool result_a , result_b , result_c , result_d , result_e , result_f; 
   char* test_str_a = "x"; 
   char* test_str_b = " x"; 
   char* test_str_c = " x "; 
   char* test_str_d = " xd"; 
   char* test_str_e = " x$"; 
   char* test_str_f = "y"; 
   char* cmp_str = "x";
   
   result_a = coco_str_compare(test_str_a,cmp_str,1);  
   result_b = coco_str_compare(test_str_b,cmp_str,1); 
   result_c = coco_str_compare(test_str_c,cmp_str,1);
   result_d = coco_str_compare(test_str_d,cmp_str,1);
   result_e = coco_str_compare(test_str_e,cmp_str,1);
   result_f = coco_str_compare(test_str_f,cmp_str,1);
   assert(result_a == true);
   assert(result_b == true);
   assert(result_c == true);
   assert(result_d == false); 
   assert(result_e == false); 
   assert(result_f == false);

   printf(ESC_GREEN"str_compare tests passed\n"ESC_RESET);
   printf("[%s] is equivalent to: [%s][%s][%s]\n",cmp_str,test_str_a,test_str_b,test_str_c);
   printf("[%s] is not equivalent to: [%s][%s][%s]\n",cmp_str,test_str_e,test_str_d,test_str_f);
}
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
   test_str_compare();
   test_async_exec();  
   return EXIT_SUCCESS; 
}
