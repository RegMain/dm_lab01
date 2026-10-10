#include "table.h"
#include "formula.h"
#include <ctype.h>
#include <stdlib.h>

char* print_table(FILE *file, stack_t *formula, int *symbols_cnt, char *symbols) {
  char tmp;
  char symbols_t[] = "00000000000000000000000000"; // Is there symbol ABC...Z?
  *symbols_cnt = 0;
  while (!feof(file)) { // Searching for variables in formula
    fscanf(file, "%c", &tmp);
    if (isalpha(tmp)) {
      tmp = toupper(tmp);
      if (symbols_t[tmp-'A'] == '0') {
        symbols_t[tmp-'A'] = '1';
        ++*symbols_cnt;
      }
    }
  }
  rewind(file);
  if (*symbols_cnt == 0) {
    printf("The result of formula is always %c\n", compute_formula(formula, 0, symbols_t));
    char *result = malloc(2);
    result[0] = '1';
    result[1] = '\0';
    return result;
  }
  // Printing the header
  for (int i = 0; i < 26; ++i) {
    if (symbols_t[i] == '1') {
      printf(" %c |", i+'A');
    }
  }
  printf(" Result\n");
  // Print the table itself
  // Bit magic for getting some bit from our string (000, 001, 010, ...,
  // 1 << symbols_cnt - 1 (amount of possible prompts to our formula is 2**symbols_cnt))
  char *table_of_truth = (char *)malloc((1 << *symbols_cnt) + 1);
  for (int i = 0; i < (1 << *symbols_cnt); ++i) {
    for (int j = 1; j <= *symbols_cnt; ++j) {
      printf(" %d |", (i >> (*symbols_cnt - j)) & 1);
    }
    int result = compute_formula(formula, i, symbols_t);
    printf(" %c\n", result);
    table_of_truth[i] = result;
  }
  table_of_truth[1 << *symbols_cnt] = '\0';
  symbols = symbols_t;
  return table_of_truth;
}
