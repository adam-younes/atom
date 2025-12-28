
#include <electron/token.h>
#include <stdio.h>
#include <stdlib.h>

void token_list_init(token_list *list) {

}


void token_list_push(token_list *list, token tok) {
  
}

void token_free(token *token) {
  free((void *) token->start);
  free(token->value.string_value.data);
}

void token_list_free(token_list *list) {
  for(size_t i = 0 ; i < list->count ; i++) {
    token_free(&list->tokens[i]);
  }
  free(list);
}

void token_print(const token *tok) {
  printf("%s\n", tok->start);
}

void token_list_print(const token_list *list) {
  for(size_t i = 0 ; i < list->count ; i++) {
    printf("%s\n", list->tokens[i].start);
  }
}
