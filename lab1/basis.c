#pragma once
#include "basis.h"
#include "formula.h"
#include <stdlib.h>
#include "string.h"

char* format_string(const char *template, const char *A, const char *B) {
  if (!template) {
    return NULL;
  }
  size_t len_A = A ? strlen(A) : 0;
  size_t len_B = B ? strlen(B) : 0;

  size_t total_len = 0;
  for (const char *p = template; *p; ) {
    if (p[0] == '{' && p[1] == 'A' && p[2] == '}') {
      total_len += len_A;
      p += 3;
    } else if (p[0] == '{' && p[1] == 'B' && p[2] == '}') {
      total_len += len_B;
      p += 3;
    } else {
      total_len++;
      p++;
    }
  }

  char *result = (char *)malloc(total_len + 1);
  if (!result) {
    return NULL;
  }

  char *dst = result;
  for (const char *p = template; *p;) {
    if (p[0] == '{' && p[1] == 'A' && p[2] == '}') {
      if (len_A > 0) {
        memcpy(dst, A, len_A);
        dst += len_A;
      }
      p += 3;
    } else if (p[0] == '{' && p[1] == 'B' && p[2] == '}') {
      if (len_B > 0) {
        memcpy(dst, B, len_B);
        dst += len_B;
      }
      p += 3;
    } else {
      *dst++ = *p++;
    }
  }
  *dst = '\0';

  return result;
}

/*
  0 - and, or, not
  1 - nor
  2 - nand
*/

char *basis_zero(int basis) {
  switch (basis) {
    case 0:
      return "($A [*] [!]$A)";
    case 1:
      return "(($A [!+] $A) [!+] (($A [!+] $A) [!+] ($A [!+] $A))";
    case 2:
      return "(($A [!*] ($A [!*] $A)) [!*] ($A [!*] ($A [!*] $A)))";
  }
}

char *basis_one(int basis) {
  switch (basis) {
    case 0:
      return "($A [+] [!]$A)";
    case 1:
      return "(((($A [!+] $A) [!+] $A)) [!+] ((($A [!+] $A) [!+] $A)))";
    case 2:
      return "($A [!*] ($A [!*] $A))";
  }
}

char *basis_to_and_or(int opcode, char *op1, char *op2) {
  char *result;
  switch (opcode) {
    case 0: // !A = not A
      result = format_string("([!]{A})", op2, NULL); break;
    case 1: // A+B = A not B
      result = format_string("({A} [+] {B})", op1, op2); break;
    case 2: // A*B = A and B
      result = format_string("({A} [*] {B})", op1, op2); break;
    case 3: // A^B = (A and not B) or (not A and B)
      result = format_string("(({A} [*] [!]{B}) [+] ([!]{A} [+] {B}))", op1, op2); break;
    case 4: // A=B = A and B or not A and not B
      result = format_string("(({A} [*] {B}) [+] ([!]{A} [*] [!]{B}))", op1, op2); break;
    case 5: // A->B = not A or B
      result = format_string("([!]{A} [+] {B})", op1, op2); break;
    case 6: // A!->B = A and not B
      result = format_string("({A} [*] [!]{B})", op1, op2); break;
    case 7: // A!+B = not (A or B)
      result = format_string("([!]({A} [+] {B}))", op1, op2); break;
    case 8: // A!*B = not (A and B)
      result = format_string("([!]({A} [*] {B}))", op1, op2); break;
    default:
      result = NULL;
  }
  return result;
}

char *basis_to_nor(int opcode, char *op1, char *op2) {
  char *result;
  switch (opcode) {
    case 0: // !A = A nor A
      result = format_string("({A} [!+] {A})", op2, NULL); break;
    case 1: // A+B = (A nor B) nor (A nor B)
      result = format_string("(({A} [!+] {B}) [!+] ({A} [!+] {B}))", op1, op2); break;
    case 2: // A*B = (A nor A) nor (B nor B)
      result = format_string("(({A} [!+] {A}) [!+] ({B} [!+] {B}))", op1, op2); break;
    case 3: // A^B = (A = B) nor (A = B)
      result = format_string("(((({A} [!+] {B}) [!+] {B}) [!+] (({A} [!+] {B}) [!+] {A})) [!+] ((({A} [!+] {B}) [!+] {B}) [!+] (({A} [!+] {B}) [!+] {A})))", op1, op2); break;
    case 4: // A=B = ((A nor B) nor B) NOR ((A nor B) nor B)
      result = format_string("((({A} [!+] {B}) [!+] {B}) [!+] (({A} [!+] {B}) [!+] {A}))", op1, op2); break;
    case 5: // A->B = not (A!->B)
      result = format_string("((({A} [!+] {B}) [!+] {B}) [!+] (({A} [!+] {B}) [!+] {B}))", op1, op2); break;
    case 6: // A!->B = (A nor B) nor B
      result = format_string("(({A} [!+] {B}) [!+] {B})", op1, op2); break;
    case 7: // A!+B= A nor B
      result = format_string("({A} [!+] {B})", op1, op2); break;
    case 8: // A!*B = not (A and B)
      result = format_string("((({A} [!+] {A}) [!+] ({B} [!+] {B})) [!+] (({A} [!+] {A}) [!+] ({B} [!+] {B})))", op1, op2); break;
    default:
      result = NULL;
  }
  return result;
}

char *basis_to_nand(int opcode, char *op1, char *op2) {
  char *result;
  switch (opcode) {
    case 0: // !A = A nand A
      result = format_string("({A} [!*] {A})", op2, NULL); break;
    case 1: // A+B = (A nand A) nand (B | B)
      result = format_string("(({A} [!*] {A}) [!*] ({B} [!*] {B}))", op1, op2); break;
    case 2: // A*B = (A nand B) nand (A nand B)
      result = format_string("(({A} [!*] {B}) [!*] ({A} [!*] {B}))", op1, op2); break;
    case 3: // A^B = ((A nand B) nand A) nand ((A nand B) nand B)
      result = format_string("((({A} [!*] {B}) [!*] {A}) [!*] (({A} [!*] {B}) [!*] {B}))", op1, op2); break;
    case 4: // A=B = (((A nand B) nand A) nand ((A nand B) nand B) nand ((A nand B) nand A) nand ((A nand B) nand B))
      result = format_string("(((({A} [!*] {B}) [!*] {A}) [!*] (({A} [!*] {B}) [!*] {B})) [!*] ((({A} [!*] {B}) [!*] {A}) [!*] (({A} [!*] {B}) [!*] {B}))))", op1, op2); break;
    case 5: // A->B = not A and B = A nand (B nand B)
      result = format_string("({A} [!*] ({B} [!*] {B}))", op1, op2); break;
    case 6: // A!->B = A and not B = (A nand (B nand B)) nand (A nand (B nand B))
      result = format_string("(({A} [!*] ({B} [!*] {B})) [!*] ({A} [!*] ({B} [!*] {B})))", op1, op2); break;
    case 7: // A!+B = ((A nand A) nand (B nand B)) nand ((A nand A) nand (B nand B))
      result = format_string("((({A} [!*] {A}) [!*] ({B} [!*] {B})) [!*] (({A} [!*] {A}) [!*] ({B} [!*] {B})))", op1, op2); break;
    case 8: // A!*B = A nand B
      result = format_string("({A} [!*] {B})", op1, op2); break;
    default:
      result = NULL;
  }
  return result;
}

void basis_change(stack_t *formula, int basis) {
  stack_t *stack = NULL;
  stack_t *formula_ptr = formula;
  while (formula_ptr != NULL) {
    // We just change A B + to (A + B) and repeat
    if (is_operator(formula_ptr->value)) {
      char *op1 = NULL;
      char *op2 = NULL;
      op2 = stack_pop(&stack);
      if (get_opcode(formula_ptr->value)) {
        op1 = stack_pop(&stack);
      }
      char *op_res;
      switch (basis) {
        case 0: op_res = basis_to_and_or(get_opcode(formula_ptr->value), op1, op2); break;
        case 1: op_res = basis_to_nor(get_opcode(formula_ptr->value), op1, op2); break;
        case 2: op_res = basis_to_nand(get_opcode(formula_ptr->value), op1, op2); break;
      }
      free(op2);
      if (get_opcode(formula_ptr->value)) {
        free(op1);
      }
      stack_push(&stack, op_res);
      free(op_res);
    } else {
      if (formula_ptr->value[0] == '0') {
        stack_push(&stack, basis_zero(basis));
      } else if (formula_ptr->value[0] == '1') {
        stack_push(&stack, basis_one(basis));
      } else {
        char token_operand[3];
        token_operand[0] = '$';
        token_operand[1] = formula_ptr->value[0];
        token_operand[2] = '\0';
        stack_push(&stack, token_operand);
      }
    }
    formula_ptr = formula_ptr->next;
  }
  if (stack_is_empty(stack)) {
    return;
  }
  char *result = stack_pop(&stack);
  switch (basis) {
    case 0: printf("AND, OR, NOT basis: "); break;
    case 1: printf("NOR basis: "); break;
    case 2: printf("NAND basis: "); break;
    case 3: printf("XOR, AND, 1 basis: "); break;
  }
  printf("%s\n", result);
  free(result);
}
