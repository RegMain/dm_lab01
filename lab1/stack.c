#include "stack.h"
#include <stdlib.h>
#include <string.h>

void stack_push(stack_t **stack, char *value) {
  stack_t *ptr;
  ptr = (stack_t *)malloc(sizeof(stack_t));
  if (ptr != NULL) {
    ptr->value = strdup(value);
    ptr->next = *stack;
    *stack = ptr;
  }
}

char* stack_pop(stack_t **stack) {
  if (!stack || !*stack) return "";
  stack_t *temp;
  char *pop_value;
  temp = *stack;
  pop_value = (*stack)->value;
  *stack = (*stack)->next;
  free(temp);
  return pop_value;
}

int stack_is_empty(stack_t *stack) {
  return (stack == NULL);
}

char* stack_top(stack_t *stack) {
  if (stack_is_empty(stack)) return "";
  return (stack->value);
}
