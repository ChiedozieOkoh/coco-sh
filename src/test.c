#include <stdio.h>
#include <stdlib.h>
#include "coconut.h"
#include <assert.h>
void test_str_compare(void){
   bool result_a , result_b , result_c; 
   char* test_str_a = "x"; 
   char* test_str_b = " x"; 
   char* test_str_c = " x "; 
   char* cmp_str = "x";
   result_a = coco_str_compare(test_str_a,cmp_str,1);  
   result_b = coco_str_compare(test_str_b,cmp_str,1); 
   result_c = coco_str_compare(test_str_c,cmp_str,1);
   assert(result_a == true);
   assert(result_b == true);
   assert(result_c == true);
   printf(ESC_GREEN"str_compare tests passed\n"ESC_RESET);
   printf("[%s] is equivalent to: [%s][%s][%s]\n",cmp_str,test_str_a,test_str_b,test_str_c);
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
   
   return EXIT_SUCCESS; 
}
